using UnityEngine;

using System;
using System.IO;
using System.Text;
using System.Net;
using System.Net.Sockets;

using System.Collections;
using System.Collections.Generic;


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

	//delegate void dg_PacketCallback (JsonData jsonDat);
	//dg_PacketCallback m_rsPacketCallback = null;

	Subject<JsonData> m_OnPacketReceive;
	Dictionary<int,Subject<JsonData>> m_dicOnRsPacketReceive = new Dictionary<int,Subject<JsonData>> ();

	static string m_strVersion = "0.0.3";

	#if UNITY_STANDALONE_OSX
	static string m_basePath = "../";
	#else
	static string m_basePath = "./";
	#endif

	//static string m_configFileName = "config.uesp.json";

	static string m_configDirPath = m_basePath + "config";
	static string m_configFilePath = m_configDirPath + "/config.uesp.json";
	static string m_scriptDirPath = m_basePath + "lua_script";

	string m_strIp = "192.168.9.1";
	int m_nBcPort = 1999;
	int m_nDataPort = 2012;

	public GameObject m_prefeb_dlgScanDevice;
	public GameObject m_GlobalInputDlg;
	public GameObject m_GlobalAlertDlg;
	public GameObject m_GlobalFileDlg;

	[SerializeField] Text m_TextInfo;
	[SerializeField] Text m_TextLog;

	public InputField m_inputfieldCode;

	[SerializeField] Button m_btnPortSetup;
	[SerializeField] Button m_btnSelectIp;

	[SerializeField] Button m_btnUpload;
	[SerializeField] Button m_btnDownload;
	[SerializeField] Button m_btnLoad;
	[SerializeField] Button m_btnSave;

	[SerializeField] Button m_btnRun;
	[SerializeField] Button m_btnAdd_ResposeCode;
	[SerializeField] Button m_btnClearLog;
	[SerializeField] Button[] m_btnMacros;

	bool m_isAppQuitting;

	void output_log (string msg)
	{

		m_TextLog.text += msg + "\n";
	}

	// sendData
	private void sendString (string message)
	{
		try {
			//if (message != "")
			//{

			// Daten mit der UTF8-Kodierung in das Binärformat kodieren.
			byte[] data = Encoding.UTF8.GetBytes (message);
			m_remoteEndPoint = new IPEndPoint (IPAddress.Parse (m_strIp), m_nDataPort);
			// Den message zum Remote-Client senden.
			mUdpClient.Send (data, data.Length, m_remoteEndPoint);
			//}
		} catch (Exception err) {
			print (err.ToString ());
		}
	}

	Subject<JsonData> listenThreadUdpData (UdpClient udp_client)
	{
		m_isAppQuitting = false;
		IObservable<JsonData> _udpSequence;

		//Text Text_logbox = transform.FindChild ("msg_log/Text").GetComponent<Text> ();

		Subject<JsonData> stream = new Subject<JsonData> ();

		_udpSequence = Observable.Create<JsonData> (observer => {
			Debug.Log (string.Format ("_udpSequence thread : {0}", Thread.CurrentThread.ManagedThreadId));

			IPEndPoint remoteEP = null;

			//myClient.EnableBroadcast = true;
			//myClient . Client . ReceiveTimeout  =  5000 ; 

			while (!m_isAppQuitting) {
				try {
					remoteEP = null;
					var receivedMsg = System.Text.Encoding.ASCII.GetString (udp_client.Receive (ref remoteEP));
					if (receivedMsg != null) {
						JsonData jsnDat = new JsonData ();
						jsnDat ["port"] = remoteEP.Port;
						jsnDat ["address"] = remoteEP.Address.ToString ();
						jsnDat ["receivedMsg"] = receivedMsg;

						observer.OnNext (jsnDat);
					}
					//observer.OnNext(new UdpState(remoteEP, receivedMsg));

				} catch (SocketException) {
					Debug.Log ("UDP :: socket broken");
					stream.OnError (new Exception ("socket broken"));
				}
			}
			observer.OnCompleted ();
			return null;
		})
			.SubscribeOn (Scheduler.ThreadPool)
			.Publish ()
			.RefCount ();

		_udpSequence
			.ObserveOnMainThread ()
			.Subscribe (xs => {
			Debug.Log (xs.ToJson ());

			stream.OnNext (xs);
			//Text_logbox.text = xs.ToJson () + "\n" + Text_logbox.text;
			/*
				Debug.Log(x["receivedMsg"].ToString());
					Debug.Log("receive remote ip :" + x["Address"]);
					Debug.Log("receive remote port :" + x["Port"]);
					*/
		})
			.AddTo (this);

		//return mUdpClient;
		return stream;
	}

	void upload_file (string filename, string upload_data, Subject<JsonData> _Observer)
	{
		int nFsm = 0;
		int nStartIndex = 0;
		int div_size = 16;

		string strTemp = upload_data;
		m_strIp = transform.FindChild ("target_ip/InputField").GetComponent<InputField> ().text;

		IDisposable update_stream = null;
		update_stream = this.UpdateAsObservable ().Subscribe ((__) => {
			switch (nFsm) {
			case 0:
				{
					_Observer.OnNext (JsonMapper.ToObject ("{\"status\":\"start\"}"));
					nFsm = 100;
					Debug.Log ("transfer start");

					int packet_id = "upload script start".GetHashCode ();
					string strCode =
						@"do local rt={type=""rs"",result=""ok"",id=" + packet_id + "}" +
						@"file.open(""" + filename + @""",""w"")" +
						@"udp_safe_sender(cjson.encode(rt)," + m_nDataPort + @",""" + Network.player.ipAddress +
						@""")" +
						@"end";

					JsonData jsonObj = new JsonData ();
					jsonObj ["cmd"] = "eval";
					jsonObj ["code"] = strCode;

					int nRetry_count = 0;
					IDisposable time_out = null;
					time_out = Observable.Interval (TimeSpan.FromMilliseconds (5000)).Subscribe ((obj) => {
						Debug.Log ("retry : " + nStartIndex);
						nRetry_count++;
						if (nRetry_count < 3) {
							sendString (jsonObj.ToJson ());
						} else {
							time_out.Dispose ();
							_Observer.OnError (new Exception ("time out"));
						}
					});

					Subject<JsonData> stream = new Subject<JsonData> ();
					m_dicOnRsPacketReceive [packet_id] = stream;
					stream.Subscribe ((jsonDat) => {
						if (jsonDat ["result"].Equals ("ok")) {
							Debug.Log ("open success");
							time_out.Dispose ();
							nFsm = 1;
						}
					});



					sendString (jsonObj.ToJson ());
				}
				break;
			case 1:
				{
					if (nStartIndex > strTemp.Length) {
						nFsm = 10;
					} else {
						
						string strSub;
						if (nStartIndex + div_size < strTemp.Length) {
							strSub = strTemp.Substring (nStartIndex, div_size);
						} else {
							strSub = strTemp.Substring (nStartIndex);
						}

						nStartIndex += div_size;
						nFsm = 100;

						print ("send : " + nStartIndex + ":" + strSub);

						byte[] StrByte = Encoding.UTF8.GetBytes (strSub);

						string strOut = "string.char(";

						for (int i = 0; i < StrByte.Length; i++) {
							strOut += StrByte [i];

							if (i < StrByte.Length - 1) {
								strOut += ",";
							} else {
								strOut += ")";
							}

						}

						int packet_id = "uploading script".GetHashCode ();
						string strCode = @"current_index=" + nStartIndex;

						strCode += @" do print(current_index) file.write(" + strOut + @") " +
						@" local rt={type=""rs"",result=""ok"",id=" + packet_id + "} " +
							//@"udp_safe_sender(cjson.encode(rt) ) " + 
						@"udp_safe_sender(cjson.encode(rt)," + m_nDataPort + @",""" + Network.player.ipAddress + @""") " +
						@" prev_index=current_index  end ";
						Debug.Log (strCode);

						JsonData jsonObj = new JsonData ();
						jsonObj ["cmd"] = "eval";
						jsonObj ["code"] = strCode;

						int nRetry_count = 0;
						IDisposable time_out = null;
						time_out = Observable.Interval (TimeSpan.FromMilliseconds (5000)).Subscribe ((obj) => {
							Debug.Log ("retry : " + nStartIndex);
							nRetry_count++;
							if (nRetry_count < 3) {
								sendString (jsonObj.ToJson ());
							} else {
								time_out.Dispose ();
								_Observer.OnError (new Exception ("time out"));
							}
						});

						Subject<JsonData> stream = new Subject<JsonData> ();
						m_dicOnRsPacketReceive [packet_id] = stream;
						stream.Subscribe ((jsonDat) => {
							if (jsonDat ["result"].Equals ("ok")) {
								jsonDat ["status"] = "upload";
								jsonDat ["pos"] = nStartIndex;
								_Observer.OnNext (jsonDat);
								time_out.Dispose ();
								nFsm = 1;
							}
						});
						sendString (jsonObj.ToJson ());
						//							m_rsPacketCallback = (jsonDat) => {
						//								if (jsonDat ["result"].Equals ("ok")) {
						//									time_out.Dispose ();
						//									nFsm = 1;
						//								}
						//							};

					}

				}
				break;
			case 10:
				{
					int packet_id = "upload final script".GetHashCode ();
					string strCode = 
						@"do local rt={type=""rs"",result=""ok"",id=" + packet_id + "} " +
						@"file.close() " +
						@"udp_safe_sender(cjson.encode(rt)," + m_nDataPort + @",""" + Network.player.ipAddress + @""") " +
						@"end ";

					JsonData jsonObj = new JsonData ();
					jsonObj ["cmd"] = "eval";
					jsonObj ["code"] = strCode;

					IDisposable time_out = Observable.Interval (TimeSpan.FromMilliseconds (5000)).Subscribe ((obj) => {
						Debug.Log ("retry : " + nStartIndex);
						sendString (jsonObj.ToJson ());
					});

					Subject<JsonData> stream = new Subject<JsonData> ();
					m_dicOnRsPacketReceive [packet_id] = stream;
					stream.Subscribe ((jsonDat) => {
						if (jsonDat ["result"].Equals ("ok")) {
							Debug.Log ("transfer complete");
							jsonDat ["status"] = "complete";
							_Observer.OnNext (jsonDat);
							time_out.Dispose ();
							nFsm = 999;
						}
					});

					sendString (jsonObj.ToJson ());

					nFsm = 100;
					//						m_rsPacketCallback = (jsonDat) => {
					//							Debug.Log ("transfer complete");
					//							//stop
					//							nFsm = 999;
					//
					//						};

				}

				break;
			case 100: //wait
				break;
			case 999:
				//m_rsPacketCallback = null;
				update_stream.Dispose ();
				//_Observer.OnCompleted ();
				break;
			}
		});



	}

	void download_file (string filename, Subject<JsonData> _Observer)
	{

		int nFsm = 0;
		int nCurrentPos = 0;
		int div_size = 128;

		string strTemp = "";

		IDisposable update_stream = null;
		update_stream = this.UpdateAsObservable ().Subscribe ((__) => {
			switch (nFsm) {
			case 0:
				{
					
					_Observer.OnNext (JsonMapper.ToObject ("{\"status\":\"start\"}"));


					nFsm = 100;
					Debug.Log ("transfer start");
					int packet_id = "download file start".GetHashCode ();
					string strCode =
						@"do local rt={type=""rs"",result=""ok"",id=" + packet_id + "}" +
						@"file.open(""" + filename + @""",""r"")" +
						@"udp_safe_sender(cjson.encode(rt)," + m_nDataPort + @",""" + Network.player.ipAddress +
						@""")" +
						@"end";

					JsonData jsonObj = new JsonData ();
					jsonObj ["cmd"] = "eval";
					jsonObj ["code"] = strCode;

					int nRetry_count = 0;
					IDisposable time_out = null;
					time_out = Observable.Interval (TimeSpan.FromMilliseconds (5000)).Subscribe ((obj) => {
						Debug.Log ("retry : " + nCurrentPos);
						if (nRetry_count < 3) {
							sendString (jsonObj.ToJson ());
						} else {
							time_out.Dispose ();
							_Observer.OnError (new Exception ("time out"));
						}

					});

					Subject<JsonData> stream = new Subject<JsonData> ();
					m_dicOnRsPacketReceive [packet_id] = stream;
					stream.Subscribe ((jsonDat) => {
						if (jsonDat ["result"].Equals ("ok")) {
							Debug.Log ("open success");
							time_out.Dispose ();
							nFsm = 1;
						}
					});

					sendString (jsonObj.ToJson ());
				}
				break;
			case 1:
				{
					nFsm = 100;

					int packet_id = "uploading script".GetHashCode ();
					//string strCode = @"current_index=" + nStartIndex;

					string strCode = @" do local buf = file.read(" + div_size + ") " +
					                 @"local rt={type=""rs"",result=""ok"",id=" + packet_id + @",d=buf} if(buf == nil) then rt.result=""eof"" end " +
					                 @"udp_safe_sender(cjson.encode(rt)," + m_nDataPort + @",""" + Network.player.ipAddress + @""") " +
					                 @"end";

					Debug.Log (strCode);

					JsonData jsonObj = new JsonData ();
					jsonObj ["cmd"] = "eval";
					jsonObj ["code"] = strCode;

					int nRetry_count = 0;
					IDisposable time_out = null;
					time_out = Observable.Interval (TimeSpan.FromMilliseconds (5000)).Subscribe ((obj) => {
						//Debug.Log ("retry : " + nStartIndex);
						nRetry_count++;
						Debug.Log ("re try : " + nRetry_count);
						if (nRetry_count < 3) {
							sendString (jsonObj.ToJson ());
						} else {
							_Observer.OnError (new Exception ("time out"));
							time_out.Dispose ();
						}

					});

					Subject<JsonData> stream = new Subject<JsonData> ();
					m_dicOnRsPacketReceive [packet_id] = stream;
					stream.Subscribe ((jsonDat) => {
						time_out.Dispose ();
						if (jsonDat ["result"].Equals ("ok")) {
							strTemp += jsonDat ["d"].ToString ();

							jsonDat ["status"] = "download";
							jsonDat ["pos"] = nCurrentPos;

							_Observer.OnNext (jsonDat);
							//m_GlobalAlertDlg.GetComponent<com_gunpower_ui.AlertDlgBox>().m_textMsg.text = "uploading .." + nCurrentPos;

							nCurrentPos += div_size;

//							time_out.Dispose ();
							nFsm = 1;
						} else if (jsonDat ["result"].Equals ("eof")) {
//							time_out.Dispose ();
							nFsm = 10; //final 
						}
					});
					sendString (jsonObj.ToJson ());
					
				}
				break;
			case 10:
				{
					int packet_id = "download final".GetHashCode ();
					string strCode = 
						@"do local rt={type=""rs"",result=""ok"",id=" + packet_id + "} " +
						@"file.close() " +
						@"udp_safe_sender(cjson.encode(rt)," + m_nDataPort + @",""" + Network.player.ipAddress + @""") " +
						@"end ";

					JsonData jsonObj = new JsonData ();
					jsonObj ["cmd"] = "eval";
					jsonObj ["code"] = strCode;

					int nRetry_count = 0;
					IDisposable time_out = null;
					time_out = Observable.Interval (TimeSpan.FromMilliseconds (5000)).Subscribe ((obj) => {
						Debug.Log ("retry : " + nCurrentPos);
						if (nRetry_count < 3) {
							sendString (jsonObj.ToJson ());
						} else {
							time_out.Dispose ();
							_Observer.OnError (new Exception ("time out"));
						}

					});

					Subject<JsonData> stream = new Subject<JsonData> ();
					m_dicOnRsPacketReceive [packet_id] = stream;
					stream.Subscribe ((jsonDat) => {
						if (jsonDat ["result"].Equals ("ok")) {
							Debug.Log ("transfer complete");
							jsonDat ["status"] = "complete";
							jsonDat ["d"] = strTemp;
							_Observer.OnNext (jsonDat);

							//_Observer.OnNext(JsonMapper.ToObject("{\"status\":\"complete\"}"));
							time_out.Dispose ();
							nFsm = 999;
						}
					});
					sendString (jsonObj.ToJson ());
					nFsm = 100;
				}

				break;
			case 100: //wait
				break;
			case 999:
				//m_rsPacketCallback = null;
				//m_GlobalAlertDlg.GetComponent<com_gunpower_ui.AlertDlgBox>().m_textMsg.text = "download complete";
				update_stream.Dispose ();
				//_Observer.OnCompleted ();
				break;
			}
		});


	}


	void resetNewWrok ()
	{
		try {
			mUdpClient = new UdpClient (m_nDataPort);
			m_OnPacketReceive = listenThreadUdpData (mUdpClient);

			m_OnPacketReceive.Subscribe (_ => {

				try {
					output_log (_.ToJson ());

					JsonData jsonObj = JsonMapper.ToObject (_ ["receivedMsg"].ToString ());
					Subject<JsonData> stream;

					if (jsonObj ["type"].Equals ("rs")) {
						if (m_dicOnRsPacketReceive.TryGetValue ((int)jsonObj ["id"], out stream)) {
							stream.OnNext (jsonObj);
							m_dicOnRsPacketReceive.Remove ((int)jsonObj ["id"]);
						}
					}
				} catch (JsonException e) {
					Debug.Log (e.ToString ());
					output_log (e.ToString ());

				}

				

			});
			//m_TextLog.text += "set port  bc :" + m_nBcPort + ", data :" + m_nDataPort + "\n"; 
			output_log ("set port  bc :" + m_nBcPort + ", data :" + m_nDataPort);

		} catch (SocketException ex) {
			Debug.Log (ex.ToString ());
		}
	}

	void save_configdata (JsonData config_data)
	{
		config_data ["remote_ip"] = m_strIp;
		config_data ["port"] ["BroadCast"] = m_nBcPort;
		config_data ["port"] ["Data"] = m_nDataPort;

		m_remoteEndPoint = new IPEndPoint (IPAddress.Parse (m_strIp), m_nDataPort);

		string strDat = config_data.ToJson ();
		myFileUtils.writeStringToFile (strDat, m_configFilePath);
	}

	// Use this for initialization
	void Start ()
	{

		output_log ("start up");

		JsonData config_data = null;
		if (System.IO.File.Exists (myFileUtils.pathForDocumentsFile (m_configFilePath))) {
			//if (true) {
			Debug.Log ("find file");
			output_log ("load config success");
			//do stuff
			string str_config = myFileUtils.readStringFromFile (m_configFilePath);
			try {

				config_data = JsonMapper.ToObject (str_config);
			} catch (JsonException e) {
				Debug.Log (e.ToString ());
				output_log (e.ToString ());
			}

		} 
		else {

			output_log ("can not found config file");
			
			config_data = new JsonData ();
			config_data ["port"] = new JsonData ();
			config_data ["port"] ["BroadCast"] = 1999;
			config_data ["port"] ["Data"] = 2012;
			config_data ["remote_ip"] = "192.168.9.9";
			config_data ["macro"] = new JsonData ();
			config_data ["macro"] = JsonMapper.ToObject (
				@"[ ""udp_safe_sender(cjson.encode(rt),port,local_ip)"" " +
				@",""""]");

			m_GlobalAlertDlg.GetComponent<com_gunpower_ui.AlertDlgBox> ().show ("info", "file not found", "ok", () => {
			}, true);


			try {
				if (!Directory.Exists (myFileUtils.pathForDocumentsFile (m_configDirPath))) {
					Directory.CreateDirectory (myFileUtils.pathForDocumentsFile (m_configDirPath));
				}

				if (!Directory.Exists (myFileUtils.pathForDocumentsFile (m_scriptDirPath))) {
					Directory.CreateDirectory (myFileUtils.pathForDocumentsFile (m_scriptDirPath));
				}
				m_GlobalAlertDlg.GetComponent<com_gunpower_ui.AlertDlgBox> ().show ("info", "file not found", "ok", () => {
				}, true);

			} catch (IOException ex) {
				m_GlobalAlertDlg.GetComponent<com_gunpower_ui.AlertDlgBox> ().show ("info", ex.ToString (), "ok", () => {
				}, true);
				
			}
		}

		m_nBcPort = int.Parse (config_data ["port"] ["BroadCast"].ToJson ());
		m_nDataPort = int.Parse (config_data ["port"] ["Data"].ToJson ());
		m_strIp = config_data ["remote_ip"].ToString ();

		m_TextInfo.text = m_strVersion;

		m_TextLog.text += "strart system \n";
		m_TextLog.text += "local ip : " + Network.player.ipAddress + "\n";
		m_TextLog.text += myFileUtils.pathForDocumentsFile ("./") + "\n";

		transform.FindChild ("target_ip/InputField").GetComponent<InputField> ().text = m_strIp;
		transform.FindChild ("target_ip/InputField").GetComponent<InputField> ().OnValueChangedAsObservable ().Subscribe (_ => {
			m_strIp = _;
		});

		transform.FindChild ("input_port/InputField_bc").GetComponent<InputField> ().text = m_nBcPort.ToString ();
		transform.FindChild ("input_port/InputField_data").GetComponent<InputField> ().text = m_nDataPort.ToString ();



		//macro buttons
		for (int i = 0; i < m_btnMacros.Length; i++) {

			string strCode = config_data ["macro"] [i].ToString ();
			m_btnMacros [i].OnClickAsObservable ().Subscribe (_ => {
				m_inputfieldCode.text = strCode;
			});
		}

		//start udp 
		resetNewWrok ();

		// = transform.FindChild ("InputField_msg").GetComponent<InputField> ();
		m_btnRun.GetComponent<Button> ().onClick.AsObservable ().Subscribe ((obj) => {
			//inputfield_msgbox.text = "test";
			JsonData jsonObj = new JsonData ();
			jsonObj ["cmd"] = "eval";
			jsonObj ["code"] = m_inputfieldCode.text;
			sendString (jsonObj.ToJson ());

		});

		m_btnAdd_ResposeCode.GetComponent<Button> ().OnClickAsObservable ().Subscribe (_ => {

			m_inputfieldCode.text += "\n rt ={type=\"rs\",id=0} " +
			"udp_safe_sender(cjson.encode(rt)," + m_nDataPort + ",\"" + Network.player.ipAddress + "\") \n";
		});

		m_btnLoad.onClick.AsObservable ().Subscribe ((obj) => {
				
			DirectoryInfo dir = new DirectoryInfo (myFileUtils.pathForDocumentsFile (m_scriptDirPath));
			FileInfo[] info = dir.GetFiles ("*.*");

			JsonData file_list = new JsonData ();

			foreach (FileInfo f in info) {
				Debug.Log (f.Name + "," + f.Length);

				file_list [f.Name] = f.Length;
			}

			m_GlobalFileDlg.GetComponent<com_gunpower_ui.FileDlg> ().m_OnSelectOK = new Subject<string> ();
			m_GlobalFileDlg.GetComponent<com_gunpower_ui.FileDlg> ().m_OnCancel = new Subject<string> ();

			m_GlobalFileDlg.GetComponent<com_gunpower_ui.FileDlg> ().m_OnSelectOK.Subscribe (__ => {
				Debug.Log (__);
				m_inputfieldCode.text = myFileUtils.readStringFromFile (m_scriptDirPath + "/" + __);
			});

			m_GlobalFileDlg.GetComponent<com_gunpower_ui.FileDlg> ().show (file_list);
			
		});


		m_btnSave.onClick.AsObservable ().Subscribe ((obj) => {

			DirectoryInfo dir = new DirectoryInfo (myFileUtils.pathForDocumentsFile (m_scriptDirPath));
			FileInfo[] info = dir.GetFiles ("*.*");

			JsonData file_list = new JsonData ();

			foreach (FileInfo f in info) {
				Debug.Log (f.Name + "," + f.Length);

				file_list [f.Name] = f.Length;
			}

			m_GlobalFileDlg.GetComponent<com_gunpower_ui.FileDlg> ().m_OnSelectOK = new Subject<string> ();
			m_GlobalFileDlg.GetComponent<com_gunpower_ui.FileDlg> ().m_OnCancel = new Subject<string> ();

			m_GlobalFileDlg.GetComponent<com_gunpower_ui.FileDlg> ().m_OnSelectOK.Subscribe (__ => {
				Debug.Log (__);
				//m_inputfieldCode.text = myFileUtils.readStringFromFile( m_scriptDirPath + "/" + __);
				myFileUtils.writeStringToFile (m_inputfieldCode.text, m_scriptDirPath + "/" + __);
			});

			m_GlobalFileDlg.GetComponent<com_gunpower_ui.FileDlg> ().show (file_list);

		});

		//editor -> esp , upload
		m_btnUpload.OnClickAsObservable ().Subscribe (_ => {

			int packet_id = "get_file_list".GetHashCode ();

			Subject<JsonData> OnGetListFilestream = new Subject<JsonData> ();
			m_dicOnRsPacketReceive [packet_id] = OnGetListFilestream;

			OnGetListFilestream.Subscribe ((jsonDat) => {

				//get file list
				JsonData file_list = JsonMapper.ToObject (jsonDat ["l"].ToJson ());

				Subject<string> OnSelectOk = new Subject<string> ();
				Subject<string> OnCancel = new Subject<string> ();

				OnSelectOk.Subscribe ((__) => {
					//select file 
					Debug.Log ("select file : " + __);
					string strTemp = m_inputfieldCode.text; 
					Subject<JsonData> stream = new Subject<JsonData> ();
					stream.Subscribe (
						(___) => { //next

							if (___ ["status"].Equals ("complete")) {
								m_GlobalAlertDlg.GetComponent<com_gunpower_ui.AlertDlgBox> ().m_textMsg.text = "upload complete";
								//m_inputfieldCode.text = ___ ["d"].ToString ();
								stream.Dispose ();
							} else if (___ ["status"].Equals ("start")) {
								m_GlobalAlertDlg.GetComponent<com_gunpower_ui.AlertDlgBox> ().show ("info",	"upload start",	"close", null, true);
								
							} else if (___ ["status"].Equals ("upload")) {
								m_GlobalAlertDlg.GetComponent<com_gunpower_ui.AlertDlgBox> ().m_textMsg.text = "upload : " + ___ ["pos"];
							}

						},
						(err) => { //err
							m_GlobalAlertDlg.GetComponent<com_gunpower_ui.AlertDlgBox> ().m_textMsg.text = err.ToString ();

						},
						() => { //complete
							Debug.Log ("comolete");
						}
					);
					upload_file (__, strTemp, stream);

				});

				m_GlobalFileDlg.GetComponent<com_gunpower_ui.FileDlg> ().show (file_list, true, OnSelectOk, OnCancel);

			});

			string strCode = 
				"do " +
				"local rt={type=\"rs\",result=\"ok\",d=data,id=" + packet_id + "}" +
				"local l=file.list() " +
				"rt.l=l " +
				"udp_safe_sender(cjson.encode(rt)," + m_nDataPort + ",\"" + Network.player.ipAddress + "\") " +
				"end ";

			JsonData jsonObj = new JsonData ();
			jsonObj ["cmd"] = "eval";
			jsonObj ["code"] = strCode;
			sendString (jsonObj.ToJson ());

			
			//string filename = transform.FindChild ("btn_group/upload_script/InputField").GetComponent<InputField> ().text;


		});

		//esp -> editor, download 
		m_btnDownload.OnClickAsObservable ().Subscribe ((_) => {


			int packet_id = "get_file_list".GetHashCode ();

			Subject<JsonData> stream = new Subject<JsonData> ();
			m_dicOnRsPacketReceive [packet_id] = stream;

			stream.Subscribe ((jsonDat) => {

				Debug.Log (jsonDat ["l"].ToJson ());
				JsonData file_list = JsonMapper.ToObject (jsonDat ["l"].ToJson ());

				Subject<string> OnSelectOk = new Subject<string> ();
				Subject<string> OnCancel = new Subject<string> ();

				//select file and start download
				OnSelectOk.Subscribe ((__) => {
					Debug.Log (__);
					Subject<JsonData> OnReceiveFile = new Subject<JsonData> ();
					OnReceiveFile.Subscribe (
						___ => {
							if (___ ["status"].Equals ("complete")) {
								m_GlobalAlertDlg.GetComponent<com_gunpower_ui.AlertDlgBox> ().m_textMsg.text = "download complete";
								m_inputfieldCode.text = ___ ["d"].ToString ();
								OnReceiveFile.Dispose ();
							} else if (___ ["status"].Equals ("start")) {
								m_GlobalAlertDlg.GetComponent<com_gunpower_ui.AlertDlgBox> ().show ("info", "start download", "close", null, true);
							} else if (___ ["status"].Equals ("download")) {
								m_GlobalAlertDlg.GetComponent<com_gunpower_ui.AlertDlgBox> ().m_textMsg.text = "download : " + ___ ["pos"];
								
							}
						},
						err => {
							m_GlobalAlertDlg.GetComponent<com_gunpower_ui.AlertDlgBox> ().m_textMsg.text = err.ToString ();
							Debug.Log (err.ToString ());
						});
					download_file (__, OnReceiveFile);
				});

				//show file dlg
				m_GlobalFileDlg.GetComponent<com_gunpower_ui.FileDlg> ().show (file_list, true, OnSelectOk, OnCancel);

			});

			string strCode = 
				"do " +
				"local rt={type=\"rs\",result=\"ok\",d=data,id=" + packet_id + "}" +
				"local l=file.list() " +
				"rt.l=l " +
				"udp_safe_sender(cjson.encode(rt)," + m_nDataPort + ",\"" + Network.player.ipAddress + "\") " +
				"end ";

			JsonData jsonObj = new JsonData ();
			jsonObj ["cmd"] = "eval";
			jsonObj ["code"] = strCode;
			sendString (jsonObj.ToJson ());

		});

		m_btnClearLog.OnClickAsObservable ().Subscribe (_ => {
			m_TextLog.text = "";
		});

		//ip selected 
		m_btnSelectIp.OnClickAsObservable ()
				 .Subscribe ((obj) => {
				
			GameObject dlgobj = Instantiate (m_prefeb_dlgScanDevice, transform);			
			dlgobj.GetComponent<Dlg_scanDevice> ().init (m_nBcPort);
			dlgobj.GetComponent<Dlg_scanDevice> ().m_OnSelectStream.Subscribe (_ => {
				//Debug.Log (_.m_strIP);
				transform.FindChild ("target_ip/InputField").GetComponent<InputField> ().text = _.m_strIP;
				save_configdata (config_data);

			});

		});

		//port change
		//transform.FindChild ("input_port/Button").GetComponent<Button> ().OnClickAsObservable ()
		m_btnPortSetup.OnClickAsObservable ()
				 .Subscribe ((obj) => {
			m_nDataPort = int.Parse (transform.FindChild ("input_port/InputField_data").GetComponent<InputField> ().text);
			m_nBcPort = int.Parse (transform.FindChild ("input_port/InputField_bc").GetComponent<InputField> ().text);
			save_configdata (config_data);

			try {
				mUdpClient.Close ();				
				resetNewWrok ();
			} catch (SocketException ex) {
				Debug.Log (ex.ToString ());
			}
			
		});

		this.OnDestroyAsObservable ().Subscribe ((obj) => {
			Debug.Log ("Script was destroyed");
			if (mUdpClient != null) {
				mUdpClient.Client.Blocking = false;
				mUdpClient.Close ();
				//mUdpClient.Client.Blocking = false;
			}
		});




	}

	// Update is called once per frame
	void Update ()
	{

	}

	void OnDestroy ()
	{

	}

	void OnApplicationQuit ()
	{
		m_isAppQuitting = true;

		//isAppQuitting = true;
	}
}
