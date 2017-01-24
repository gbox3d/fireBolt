using UnityEngine;

using System;
using System.Text;
using System.Net;
using System.Net.Sockets;

using System.Collections;
using System.Collections.Generic;


using UniRx;
using UniRx.Triggers;

using UnityEngine.UI;

using LitJson;

public class mainObject : MonoBehaviour
{

	UdpClient mUdpClient;
	IPEndPoint remoteEndPoint;

	Dlg_scanDevice m_dlgScanDevice;

	delegate void dg_PacketCallback(JsonData jsonDat);

	dg_PacketCallback m_rsPacketCallback = null;

	string m_strIp = "192.168.9.1";
	int m_nBCPort = 1999;
	int m_nPort = 2012;

	// sendData
	private void sendString(string message)
	{
		try
		{
			//if (message != "")
			//{

			// Daten mit der UTF8-Kodierung in das Binärformat kodieren.
			byte[] data = Encoding.UTF8.GetBytes(message);

			// Den message zum Remote-Client senden.
			mUdpClient.Send(data, data.Length, remoteEndPoint);
			//}
		}
		catch (Exception err)
		{
			print(err.ToString());
		}
	}

	Dictionary<int,Subject<JsonData>> m_dicOnRsPacketReceive = new Dictionary<int,Subject<JsonData>>();

	// Use this for initialization
	void Start()
	{
		//Debug.Log ( LocalIPAddress() );
		//local ip address
		Debug.Log (Network.player.ipAddress);

		//m_strLocalIp = Network.player.ipAddress;
		//remoteEndPoint = new IPEndPoint(IPAddress.Parse("192.168.9.15"), 2012);

		transform.FindChild("target_ip/InputField").GetComponent<InputField>().text = "192.168.9.11";

		//rs packet process
		m_rsPacketCallback = (jsonDat) => { 
			Debug.Log(jsonDat.ToJson()); 

			m_dicOnRsPacketReceive[ (int)jsonDat["id"]].OnNext(jsonDat);

		};

		m_dlgScanDevice = transform.FindChild("Dlg_scanDevice").GetComponent<Dlg_scanDevice>();
		m_dlgScanDevice.gameObject.SetActive(false);

		m_dlgScanDevice.m_callback = new Dlg_scanDevice.MyDelegateType((item) =>
		{
			m_strIp = transform.FindChild("target_ip/InputField").GetComponent<InputField>().text = item.m_strIP;
		});

		InputField inputfield_msgbox = transform.FindChild("InputField_msg").GetComponent<InputField>();
		Text Text_logbox = transform.FindChild("msg_log/Text").GetComponent<Text>();

		//InputField inpf_ipaddress = transform.FindChild("ip_address").GetComponent<InputField>();
		//inpf_ipaddress.text = m_strIp;

		//listen broad casting
		mUdpClient = new UdpClient(m_nBCPort);
		IObservable<JsonData> heavyMethod = Observable.Start(() =>
			{
				try
				{
					// Bytes empfangen.
					IPEndPoint anyIP = new IPEndPoint(IPAddress.Any, 0);
					byte[] data = mUdpClient.Receive(ref anyIP);
					// Bytes mit der UTF8-Kodierung in das Textformat kodieren.
					string text = Encoding.UTF8.GetString(data);

					JsonData jsonObj = JsonMapper.ToObject(text);
					//get remote ip,port 
					//http://stackoverflow.com/questions/1314671/how-do-i-find-the-port-number-assigned-to-a-udp-client-in-net-c
					jsonObj["ip"] = anyIP.Address.ToString();
					jsonObj["port"] = anyIP.Port;

					return jsonObj;
				}
				catch (Exception err)
				{
					print(err.ToString());
				}

				return null;
			});

		// Join and await two other thread values
		Observable.ObserveOnMainThread(heavyMethod) // return to main thread
			.Repeat()
			.TakeUntilDestroy(this)
			.Subscribe(xs =>
		{
			if (xs["type"].Equals("bc"))
			{
				m_dlgScanDevice.processPaket(xs);

				//m_strIp = xs["ip"].ToString();
			}
			else if (xs["type"].Equals("rs"))
			{
				if (m_rsPacketCallback != null)
				{
					m_rsPacketCallback(xs);
				}


			}
			else {
				Text_logbox.text = xs.ToJson() + "\n" + Text_logbox.text;

			}

			//text_displog.text = xs.ToJson();

		}).AddTo(this.gameObject);


		transform.FindChild("Button_send").GetComponent<Button>().onClick.AsObservable()
				 .Subscribe((obj) =>
				 {
					 //inputfield_msgbox.text = "test";
					 JsonData jsonObj = new JsonData();
					 jsonObj["cmd"] = "eval";
					 jsonObj["code"] = inputfield_msgbox.text;

					 remoteEndPoint = new IPEndPoint(IPAddress.Parse(m_strIp), m_nPort);

					 sendString(jsonObj.ToJson());

				 });

		transform.FindChild("btn_group/Button_1").GetComponent<Button>().onClick.AsObservable()
				 .Subscribe((obj) =>
				 {
					 inputfield_msgbox.text = "local rt = {type=\"rs\",result=\"echo back test\"} udp_safe_sender(cjson.encode(rt))";

				 });
		transform.FindChild("btn_group/Button_2").GetComponent<Button>().onClick.AsObservable()
				 .Subscribe((obj) =>
				 {
					 inputfield_msgbox.text = @"lf_mt(0)udp_safe_sender(cjson.encode({type='rs',result='ok'}))";
				 });
		transform.FindChild("btn_group/loadscript/Button").GetComponent<Button>().onClick.AsObservable()
				 .Subscribe((obj) =>
				 {
					 string filename = transform.FindChild("btn_group/loadscript/InputField").GetComponent<InputField>().text;
					 inputfield_msgbox.text = myFileUtils.readStringFromFile("../lua_script/" + filename);

				 });

		//text editor -> esp 
		transform.FindChild("btn_group/savescript/Button").GetComponent<Button>().onClick.AsObservable()
				 .Subscribe((obj) =>
				 {
					 string filename = transform.FindChild("btn_group/savescript/InputField").GetComponent<InputField>().text;

					 string strCode = @"(function(fn,data)
local rt={type=""rs"",result=""ok""}
file.open(fn,""w"")
file.write(data)
file.close()
udp_safe_sender(cjson.encode(rt) )
end)([[" + filename + "]],[["
				+ inputfield_msgbox.text
				+ "]])";

					 JsonData jsonObj = new JsonData();
					 jsonObj["cmd"] = "eval";
					 jsonObj["code"] = strCode;
					 remoteEndPoint = new IPEndPoint(IPAddress.Parse(m_strIp), m_nPort);
					 sendString(jsonObj.ToJson());

				 });

		//file -> esp 
		transform.FindChild("btn_group/upload_script/Button").GetComponent<Button>().OnClickAsObservable().Subscribe(_ =>
		{

			int nFsm = 0;
			int nStartIndex = 0;
			int div_size = 128;

			//save_test.lua
			//string filename = "test.lua";
			string filename = "three.js";//transform.FindChild("btn_group/savescript_2/InputField").GetComponent<InputField>().text;
			string strTemp = myFileUtils.readStringFromFile("../lua_script/" + filename);

			//Debug.Log(strTemp);

			m_strIp = transform.FindChild("target_ip/InputField").GetComponent<InputField>().text;
			remoteEndPoint = new IPEndPoint(IPAddress.Parse(m_strIp), m_nPort);


			IDisposable update_stream = null;
			update_stream = this.UpdateAsObservable().Subscribe((__) =>
		  {
			  switch (nFsm)
			  {
				  case 0:
					  {
						  Debug.Log("transfer start");

						  //filename = "save_test.lua";
							//filename = "three.js";

						  string strCode = @"do local rt={type=""rs"",result=""ok""} file.open(""" + filename + @""",""w"") udp_safe_sender(cjson.encode(rt) ) end";

						  JsonData jsonObj = new JsonData();
						  jsonObj["cmd"] = "eval";
						  jsonObj["code"] = strCode;

						  sendString(jsonObj.ToJson());

						  nFsm = 100;
						  m_rsPacketCallback = (jsonDat) =>
						  {

							  if (jsonDat["result"].Equals("ok"))
							  {
								  Debug.Log("open success");
								  nFsm = 1;
							  }
						  };

					  }
					  break;
				  case 1:
					  {
						  if (nStartIndex > strTemp.Length)
						  {
							  nFsm = 10;
						  }
						  else {

							  string strSub;
							  if (nStartIndex + div_size < strTemp.Length)
							  {
								  strSub = strTemp.Substring(nStartIndex, div_size);
							  }
							  else {
								  strSub = strTemp.Substring(nStartIndex);
							  }

							  nStartIndex += div_size;
							  nFsm = 100;

							  print("send : " + nStartIndex + ":" + strSub);

							  byte[] StrByte = Encoding.UTF8.GetBytes(strSub);

							  string strOut = "string.char(";

							  for (int i = 0; i < StrByte.Length; i++)
							  {
								  strOut += StrByte[i];

								  if (i < StrByte.Length - 1)
								  {
									  strOut += ",";
								  }
								  else {
									  strOut += ")";
								  }

							  }

								string strCode = @"current_index=" + nStartIndex;

								strCode += @" do print(current_index) file.write(" + strOut + @") local rt={type=""rs"",result=""ok""} udp_safe_sender(cjson.encode(rt) ) end prev_index=current_index ";

							  Debug.Log(strCode);

							  JsonData jsonObj = new JsonData();
							  jsonObj["cmd"] = "eval";
							  jsonObj["code"] = strCode;

							  sendString(jsonObj.ToJson());


							  IDisposable time_out = Observable.Interval(TimeSpan.FromMilliseconds(5000)).Subscribe((obj) =>
							  {
									Debug.Log("retry : " + nStartIndex);
									sendString(jsonObj.ToJson());
								  
							  });



							  m_rsPacketCallback = (jsonDat) =>
							{
								if (jsonDat["result"].Equals("ok"))
								{
									time_out.Dispose();
										

									nFsm = 1;
								}
							};

						  }

					  }
					  break;
				  case 10:
					  {
						  string strCode = @"do local rt={type=""rs"",result=""ok""} file.close() udp_safe_sender(cjson.encode(rt) ) end";

						  JsonData jsonObj = new JsonData();
						  jsonObj["cmd"] = "eval";
						  jsonObj["code"] = strCode;

						  sendString(jsonObj.ToJson());

						  nFsm = 100;
						  m_rsPacketCallback = (jsonDat) =>
						  {
							  Debug.Log("transfer complete");
							  //stop
							  nFsm = 999;

						  };

					  }

					  break;
				  case 100: //wait



					  break;
				  case 999:
					  m_rsPacketCallback = null;
					  update_stream.Dispose();
					  break;
			  }


		  });



		});


		transform.FindChild ("btn_group/download_script/Button").GetComponent<Button> ().OnClickAsObservable ()
			.Subscribe (_ => {

				int packet_id = "download_script".GetHashCode();

				Subject<JsonData> stream = new Subject<JsonData>();
				m_dicOnRsPacketReceive[packet_id] = stream;

				stream.Subscribe( (jsonDat)=> {
					inputfield_msgbox.text = jsonDat["d"].ToString();
					
				});

				string filename = transform.FindChild("btn_group/download_script/InputField").GetComponent<InputField>().text;
				string strCode ="file.open(\"" +filename  + "\",\"r\") local data = file.read() print(data) " +
					"local rt={type=\"rs\",result=\"ok\",d=data,id="+ packet_id  + "}";

				strCode += "udp_safe_sender("+ m_nBCPort +",\""+ Network.player.ipAddress + "\",cjson.encode(rt) )";

				JsonData jsonObj = new JsonData();
				jsonObj["cmd"] = "eval";
				//jsonObj["id"] = strCode.GetHashCode();
				jsonObj["code"] = strCode;
				remoteEndPoint = new IPEndPoint(IPAddress.Parse(m_strIp), m_nPort);
				sendString(jsonObj.ToJson());


					
				Debug.Log(jsonObj.ToJson());

				
		});


		transform.FindChild("target_ip/Button").GetComponent<Button>().OnClickAsObservable()
				 .Subscribe((obj) =>
				 {
					 transform.FindChild("Dlg_scanDevice").gameObject.SetActive(true);
				 });



	}

	// Update is called once per frame
	void Update()
	{

	}

	void OnDestroy()
	{
		Debug.Log("Script was destroyed");
	}

	void OnApplicationQuit()
	{
		if (mUdpClient != null)
		{
			mUdpClient.Client.Blocking = false;
		}
		//isAppQuitting = true;
	}
}
