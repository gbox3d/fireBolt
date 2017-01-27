using UnityEngine;
using System;
using System.Collections;

using UniRx;
using UniRx.Triggers;

using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

using LitJson;

using System.Collections;
using System.Collections.Generic;

using UnityEngine.UI;

//http://qiita.com/Napier_271828_/items/0d489c7d833047241079

public class UdpState : System.IEquatable<UdpState>
{
	//UDP通信の情報を収める。送受信ともに使える
	public IPEndPoint EndPoint { get; set; }
	public string UdpMsg { get; set; }

	public UdpState(IPEndPoint ep, string udpMsg)
	{
		this.EndPoint = ep;
		this.UdpMsg = udpMsg;
	}
	public override int GetHashCode()
	{
		return EndPoint.Address.GetHashCode();
	}

	public bool Equals(UdpState s)
	{
		if (s == null)
		{
			return false;
		}
		return EndPoint.Address.Equals(s.EndPoint.Address);
	}
}


public class mainApp : MonoBehaviour
{


	private int m_nBCPort = 1999;
	private int m_nDataPort = 2012;
	private static UdpClient myClient;
	private bool isAppQuitting;
	public IObservable<UdpState> _udpSequence;


	[SerializeField] GameObject m_prefebDevice;
	public Dictionary<string,GameObject> m_deviceObjList; 

	GameObject m_Slider_left;
	GameObject m_Slider_right;

	IPEndPoint m_remoteEndPoint = null;
	public void sendMsg(string message)
	{
		try
		{
			//if (message != "")
			//{

			// Daten mit der UTF8-Kodierung in das Binärformat kodieren.
			byte[] data = Encoding.UTF8.GetBytes(message);

			// Den message zum Remote-Client senden.
			myClient.Send(data, data.Length, m_remoteEndPoint);
			//}
		}
		catch (Exception err)
		{
			print(err.ToString());
		}
	}

	// Use this for initialization
	void Start()
	{
		m_deviceObjList = new  Dictionary<string,GameObject> ();
		isAppQuitting = false;

		m_Slider_left = transform.FindChild ("left_slide_panel").gameObject;
		m_Slider_right = transform.FindChild ("right_slide_panel").gameObject;

		_udpSequence = Observable.Create<UdpState>(observer =>
		{
			Debug.Log(string.Format("_udpSequence thread : {0}", Thread.CurrentThread.ManagedThreadId));
			try
			{
				myClient = new UdpClient(m_nBCPort);
			}
			catch (SocketException ex)
			{
				observer.OnError(ex);
			}
			IPEndPoint remoteEP = null;
			myClient.EnableBroadcast = true;
			//myClient . Client . ReceiveTimeout  =  5000 ; 
			while (!isAppQuitting)
			{
				try
				{
					remoteEP = null;
					var receivedMsg = System.Text.Encoding.ASCII.GetString(myClient.Receive(ref remoteEP));
					observer.OnNext(new UdpState(remoteEP, receivedMsg)); // UdpState 객체를 를 Subscribe 로 내보낸다.
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


		//myUdpSequence  =  _udpReceiverRx . _udpSequence ; 

		Transform device_list_content = transform.FindChild ("device_list/Viewport/Content");

		_udpSequence
			.ObserveOnMainThread()
			.Subscribe(x =>
			{
					//Debug.Log(x.EndPoint.Address);
					//Debug.Log(x.UdpMsg);					
					try {
						var jsonObj = JsonMapper.ToObject(x.UdpMsg);
						string str_ip = x.EndPoint.Address.ToString();

						if (jsonObj["type"].Equals("bc"))
						{
							//디바이스 추가하기 
							GameObject devobj;
							if(m_deviceObjList.TryGetValue(str_ip,out devobj) ){
							}
							else {
								devobj = Instantiate(m_prefebDevice,device_list_content);
								devobj.transform.GetComponent<RectTransform>().localPosition = new Vector2(90,-100);
								devobj.transform.FindChild("Text").GetComponent<Text>().text = str_ip;
								m_deviceObjList[ str_ip] = devobj;
							}
							
						}
						else if (jsonObj["type"].Equals("res"))
						{

						}
						else {
						}
					}
					catch(Exception e)
					{
						Debug.Log(e.ToString());
					}
					

			
				//print(x.UdpMsg);
			})
			.AddTo(this);

		//device click event 처리 
		this.UpdateAsObservable().Subscribe( _ =>			
			{
				int cnt = Input.touchCount;
				for (int i = 0; i < cnt; ++i)
				{
					Touch touch = Input.GetTouch(i);
					Vector2 pos = touch.position;
					if (touch.phase == TouchPhase.Began)
					{

						foreach(string key in m_deviceObjList.Keys) {

							GameObject item = m_deviceObjList[key];

							if (RectTransformUtility.RectangleContainsScreenPoint(
								item.GetComponent<RectTransform>(), pos) == true)
							{
								Debug.Log("hit " + item.transform.FindChild("Text").GetComponent<Text>().text);
								item.transform.FindChild("Panel").GetComponent<Image>().color = Color.red;
								m_remoteEndPoint = new IPEndPoint(IPAddress.Parse(key), m_nDataPort);
								//sendMsg("lf_mt(256)");
								break;
							}
							
						}
						
						

					}
				}
				
			});
		
		this.UpdateAsObservable ().Subscribe (_ => {

			//Debug.Log(m_Slider_left.GetComponent<slideControl>().m_fGraderValue + "," + m_Slider_right.GetComponent<slideControl>().m_fGraderValue);
			int left_val = (int) Mathf.Clamp( Mathf.Round(m_Slider_left.GetComponent<slideControl>().m_fGraderValue) * 4,-900, 900);
			int right_val = 0-(int)Mathf.Clamp(Mathf.Round(m_Slider_right.GetComponent<slideControl>().m_fGraderValue) * 4,-900, 900);
			//int right_val =0;
			if(m_remoteEndPoint != null) {
				string strCode = "lf_mt("+  left_val + ")";
				strCode += " rt_mt(" + right_val + ")";
				//Debug.Log(strCode);
				JsonData jsonObj = new JsonData();
				jsonObj["cmd"] = "eval";
				jsonObj["code"] = strCode;
				sendMsg(jsonObj.ToJson());
				//lf_mt(0)
				//sendMsg("lf_mt("+m_Slider_left.GetComponent<slideControl>().m_fGraderValue+")");
			}
			else {
					Debug.Log(left_val + "," + right_val);
			}
			
		});





	}

	// Update is called once per frame
	void Update()
	{

	}
	void OnApplicationQuit()
	{
		isAppQuitting = true;
		myClient.Client.Blocking = false;
	}
}
