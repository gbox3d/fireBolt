using UnityEngine;

using System;
using System.Text;
using System.Net;
using System.Net.Sockets;

using System.Threading;

using UniRx;
using UniRx.Triggers;

using UnityEngine.UI;

using LitJson;

public class mainObject : MonoBehaviour
{

	UdpClient mUdpClient;
	IPEndPoint m_remoteEndPoint;

	Dlg_scanDevice m_dlgScanDevice;

	delegate void dg_PacketCallback(JsonData jsonDat);

	dg_PacketCallback m_rsPacketCallback = null;


	string m_strIp = "192.168.9.1";
	int m_nBcPort = 1999;
	int m_nDataPort = 2012;

	public GameObject m_prefeb_dlgScanDevice;

	public GameObject m_GlobalInputDlg;


	// sendData
	private void sendString(string message)
	{
		try
		{
			//if (message != "")
			//{

			// Daten mit der UTF8-Kodierung in das Binärformat kodieren.
			byte[] data = Encoding.UTF8.GetBytes(message);
			m_remoteEndPoint = new IPEndPoint(IPAddress.Parse(m_strIp), m_nDataPort);
			// Den message zum Remote-Client senden.
			mUdpClient.Send(data, data.Length, m_remoteEndPoint);
			//}
		}
		catch (Exception err)
		{
			print(err.ToString());
		}
	}

	/*
	UdpClient reset_network(int nPort)
	{
		Debug.Log("listen port : " + nPort);
		UdpClient UdpClient = new UdpClient(nPort);

		Text Text_logbox = transform.FindChild("msg_log/Text").GetComponent<Text>();

		IObservable<JsonData> heavyMethod = Observable.Start(() =>
			{
				try
				{
					// Bytes empfangen.
					IPEndPoint anyIP = new IPEndPoint(IPAddress.Any, 0);
					byte[] data = UdpClient.Receive(ref anyIP);
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
			Text_logbox.text = xs.ToJson() + "\n" + Text_logbox.text;

		}).AddTo(this.gameObject);

		return UdpClient;
	}
	*/

	bool m_isAppQuitting;

	UdpClient listenThreadUdpData()
	{
		m_isAppQuitting = false;
		IObservable<JsonData> _udpSequence;

		Text Text_logbox = transform.FindChild("msg_log/Text").GetComponent<Text>();

		_udpSequence = Observable.Create<JsonData>(observer =>
	 {
		 Debug.Log(string.Format("_udpSequence thread : {0}", Thread.CurrentThread.ManagedThreadId));

		 IPEndPoint remoteEP = null;

		 //myClient.EnableBroadcast = true;
		 //myClient . Client . ReceiveTimeout  =  5000 ; 

		 while (!m_isAppQuitting)
		 {
			 try
			 {
				 remoteEP = null;
				 var receivedMsg = System.Text.Encoding.ASCII.GetString(mUdpClient.Receive(ref remoteEP));
				 if (receivedMsg != null)
				 {
					 JsonData jsnDat = new JsonData();
					 jsnDat["port"] = remoteEP.Port;
					 jsnDat["address"] = remoteEP.Address.ToString();
					 jsnDat["receivedMsg"] = receivedMsg;

					 observer.OnNext(jsnDat);
				 }
				 //observer.OnNext(new UdpState(remoteEP, receivedMsg));

			 }
			 catch (SocketException)
			 {
				 Debug.Log("UDP :: Receive timeout");
			 }
		 }
		 observer.OnCompleted();
		 return null;
	 })
			.SubscribeOn(Scheduler.ThreadPool)
			.Publish()
			.RefCount();

		_udpSequence
			.ObserveOnMainThread()
			.Subscribe(xs =>
			{
				Debug.Log(xs.ToJson());
				Text_logbox.text = xs.ToJson() + "\n" + Text_logbox.text;

				/*
				Debug.Log(x["receivedMsg"].ToString());
					Debug.Log("receive remote ip :" + x["Address"]);
					Debug.Log("receive remote port :" + x["Port"]);
					*/
			})
			.AddTo(this);

		return mUdpClient;
	}


	// Use this for initialization
	void Start()
	{

		string str_config = myFileUtils.readStringFromFile("../config/config.udptester.json");

		JsonData config_data = JsonMapper.ToObject(str_config);



		m_nBcPort = int.Parse(config_data["port"]["BroadCast"].ToJson());
		m_nDataPort = int.Parse(config_data["port"]["Data"].ToJson());


		//remoteEndPoint = new IPEndPoint(IPAddress.Parse("192.168.9.15"), 2012);

		transform.FindChild("target_ip/InputField").GetComponent<InputField>().text = m_strIp;
		transform.FindChild("input_port/InputField_bc").GetComponent<InputField>().text = m_nBcPort.ToString();
		transform.FindChild("input_port/InputField_data").GetComponent<InputField>().text = m_nDataPort.ToString();

		try
		{
			mUdpClient = new UdpClient(m_nDataPort);
			listenThreadUdpData();
		}
		catch (SocketException ex)
		{
			Debug.Log(ex.ToString());
		}



		//mUdpClient = reset_network(m_nDataPort);
		//mUdpClient = resetNetWork(m_nDataPort);

		m_rsPacketCallback = (jsonDat) => { Debug.Log(jsonDat.ToJson()); };


		//m_dlgScanDevice = transform.FindChild("Dlg_scanDevice").GetComponent<Dlg_scanDevice>();
		//m_dlgScanDevice.gameObject.SetActive(false);

		//m_dlgScanDevice.m_callback = new Dlg_scanDevice.MyDelegateType((item) =>
		//{
		//	m_strIp = transform.FindChild("target_ip/InputField").GetComponent<InputField>().text = item.m_strIP;

		//});

		//m_dlgScanDevice.m_OnSelectStream.Subscribe(_ =>
		//{
		//	m_strIp = transform.FindChild("target_ip/InputField").GetComponent<InputField>().text = _.m_strIP;

		//});




		//InputField inpf_ipaddress = transform.FindChild("ip_address").GetComponent<InputField>();
		//inpf_ipaddress.text = m_strIp;
		InputField inputfield_msgbox = transform.FindChild("InputField_msg").GetComponent<InputField>();
		transform.FindChild("Button_send").GetComponent<Button>().onClick.AsObservable()
				 .Subscribe((obj) =>
				 {
					 //inputfield_msgbox.text = "test";
					 JsonData jsonObj = new JsonData();
					 jsonObj["cmd"] = "eval";
					 jsonObj["code"] = inputfield_msgbox.text;

					 sendString(jsonObj.ToJson());

				 });

		transform.FindChild("btn_group/Button_1").GetComponent<Button>().onClick.AsObservable()
				 .Subscribe((obj) =>
				 {
					 inputfield_msgbox.text = config_data["macro"][0].ToString();
					 //inputfield_msgbox.text = "local rt = {type=\"rs\",result=\"echo back test\"} udp_safe_sender(cjson.encode(rt))";

				 });
		transform.FindChild("btn_group/Button_2").GetComponent<Button>().onClick.AsObservable()
				 .Subscribe((obj) =>
				 {
					 inputfield_msgbox.text = config_data["macro"][1].ToString();//"local rt = {type=\"rs\",id=0} udp_safe_sender(cjson.encode(rt))";

					 //inputfield_msgbox.text = @"lf_mt(0)udp_safe_sender(cjson.encode({type='rs',result='ok'}))";
				 });
		transform.FindChild("btn_group/loadscript/Button").GetComponent<Button>().onClick.AsObservable()
				 .Subscribe((obj) =>
				 {
					 string filename = transform.FindChild("btn_group/loadscript/InputField").GetComponent<InputField>().text;
					 inputfield_msgbox.text = myFileUtils.readStringFromFile("../lua_script/" + filename);

				 });
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
			//remoteEndPoint = new IPEndPoint(IPAddress.Parse(m_strIp), m_nBcPort);


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

		transform.FindChild ("btn_group/download_script/Button").GetComponent<Button> ().OnClickAsObservable ().Subscribe ((_) => {

			m_GlobalInputDlg.GetComponent<com_gunpower_ui.input_dlg>().show("input file name","",(evt)=> {
				
				if(  (bool) evt["ok"] == true ) {
					Debug.Log(evt["val"].ToString());
				}
				else { //cancel
				}

				//Debug.Log(evt.ToJson());
			});
			
		});

		transform.FindChild("Button_clearlog").GetComponent<Button>().OnClickAsObservable().Subscribe(_ =>
		{
			transform.FindChild("msg_log/Text").GetComponent<Text>().text = "";
		});



		transform.FindChild("target_ip/Button").GetComponent<Button>().OnClickAsObservable()
				 .Subscribe((obj) =>
				 {
					 //transform.FindChild("Dlg_scanDevice").gameObject.SetActive(true);
					 GameObject dlgobj = Instantiate(m_prefeb_dlgScanDevice, transform);
					 dlgobj.GetComponent<Dlg_scanDevice>().init(int.Parse(transform.FindChild("input_port/InputField_bc").GetComponent<InputField>().text));
					 dlgobj.GetComponent<Dlg_scanDevice>().m_OnSelectStream.Subscribe(_ =>
					 {
						 Debug.Log(_.m_strIP);
						 m_strIp = transform.FindChild("target_ip/InputField").GetComponent<InputField>().text = _.m_strIP;
						 m_remoteEndPoint = new IPEndPoint(IPAddress.Parse(m_strIp), m_nDataPort);

					 });

				 });

		//port change
		transform.FindChild("input_port/Button").GetComponent<Button>().OnClickAsObservable()
				 .Subscribe((obj) =>
				 {
					 m_nDataPort = int.Parse(transform.FindChild("input_port/InputField_data").GetComponent<InputField>().text);
					 m_nBcPort = int.Parse(transform.FindChild("input_port/InputField_bc").GetComponent<InputField>().text);

					 try
					 {
						 mUdpClient.Close();
						 mUdpClient = new UdpClient(m_nDataPort);
					 }
					 catch (SocketException ex)
					 {
						 Debug.Log(ex.ToString());
					 }
					 //m_isAppQuitting = true;


					 //resetNetWork(m_nDataPort);
					 //m_nBcPort = int.Parse(transform.FindChild("input_port/InputField").GetComponent<InputField>().text);
					 //Debug.Log("open socket " + m_nBcPort);
					 //mUdpClient = reset_network(m_nBcPort);
					 //transform.FindChild("Dlg_scanDevice").gameObject.SetActive(true);
				 });

		this.OnDestroyAsObservable().Subscribe((obj) =>
		{
			Debug.Log("Script was destroyed");
			if (mUdpClient != null)
			{
				mUdpClient.Client.Blocking = false;
				mUdpClient.Close();
				//mUdpClient.Client.Blocking = false;
			}
		});




	}

	// Update is called once per frame
	void Update()
	{

	}

	void OnDestroy()
	{

	}

	void OnApplicationQuit()
	{
		m_isAppQuitting = true;

		//isAppQuitting = true;
	}
}
