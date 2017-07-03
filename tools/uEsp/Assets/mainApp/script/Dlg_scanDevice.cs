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

public class Dlg_scanDevice : MonoBehaviour
{

	GameObject m_prefeb_ListItem;
	GameObject m_listObj;
	UdpClient mUdpClient = null;


	public Subject<device_item> m_OnSelectStream = new Subject<device_item>();

	//public delegate void MyDelegateType(device_item item);
	//public MyDelegateType m_callback;

	public void processPaket(JsonData jsnDat)
	{

		if (gameObject.activeSelf)
		{
			//RectTransform[] items = m_listObj.GetComponentsInChildren<RectTransform>();

			string strName = "item_" + jsnDat["ip"].ToJson();
			if (m_listObj.transform.Find(strName) == null)
			{
				GameObject itemObj = UnityEngine.Object.Instantiate(m_prefeb_ListItem) as GameObject;
				itemObj.name = strName;
				itemObj.transform.Find("Text").GetComponent<Text>().text = jsnDat["ip"].ToString();
				itemObj.transform.SetParent(m_listObj.transform);
				itemObj.transform.localPosition = new Vector3((568 / 2), 0 - m_listObj.transform.childCount * 36, 0);
				itemObj.GetComponent<device_item>().m_strIP = jsnDat["ip"].ToString();
				Debug.Log(jsnDat.ToJson());
			}


		}
	}

	/*
	UdpClient reset_network(int nPort)
	{

		UdpClient UdpClient = new UdpClient(nPort);

		//Text Text_logbox = transform.FindChild("msg_log/Text").GetComponent<Text>();

		IObservable<JsonData> heavyMethod = Observable.Start(() =>
			{
				try
				{
					if (mUdpClient.Client.Blocking)
					
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

				}
				catch (Exception err)
				{
					print(err.ToString());
				}

				return null;
			});

		// Join and await two other thread values
		Observable.ObserveOnMainThread(heavyMethod) // return to main thread
		          .Where(_ => { return _!= null; })
			.Repeat().TakeUntilDestroy(this)

			.Subscribe(xs =>
		{
			//Text_logbox.text = xs.ToJson() + "\n" + Text_logbox.text;
			if (xs["type"].Equals("bc"))
			{
				processPaket(xs);
			}


		}).AddTo(this.gameObject);

		return UdpClient;
	}

*/

	bool m_isAppQuitting;

	UdpClient resetNetWork(int nPort)
	{
		m_isAppQuitting = false;
		IObservable<JsonData> _udpSequence;

		//Text Text_logbox = transform.FindChild("msg_log/Text").GetComponent<Text>();

		_udpSequence = Observable.Create<JsonData>(observer =>
	 {
		 Debug.Log(string.Format("_udpSequence thread : {0}", Thread.CurrentThread.ManagedThreadId));
		 try
		 {
			 mUdpClient = new UdpClient(nPort);
		 }
		 catch (SocketException ex)
		 {
			 observer.OnError(ex);
		 }
		 IPEndPoint remoteEP = null;

		 mUdpClient.Client.Blocking = true;
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
						/*
					 JsonData jsnDat = new JsonData();
					 jsnDat["port"] = remoteEP.Port;
					 jsnDat["address"] = remoteEP.Address.ToString();
					 jsnDat["receivedMsg"] = receivedMsg;
					 */

					 JsonData jsonObj = JsonMapper.ToObject(receivedMsg);
					 //get remote ip,port 
					 //http://stackoverflow.com/questions/1314671/how-do-i-find-the-port-number-assigned-to-a-udp-client-in-net-c
					 jsonObj["ip"] = remoteEP.Address.ToString();
					 jsonObj["port"] = remoteEP.Port;


					 observer.OnNext(jsonObj);
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
				//Debug.Log(xs.ToJson());
				//Text_logbox.text = xs.ToJson() + "\n" + Text_logbox.text;

			processPaket(xs);

				/*
				Debug.Log(x["receivedMsg"].ToString());
					Debug.Log("receive remote ip :" + x["Address"]);
					Debug.Log("receive remote port :" + x["Port"]);
					*/
			})
			.AddTo(this);

		return mUdpClient;
	}


	public void init(int nPort)
	{
		transform.localPosition = new Vector2(0, 0);
		//m_nBcPort = int.Parse(transform.FindChild("input_port/InputField_bc").GetComponent<InputField>().text);
		mUdpClient = resetNetWork(nPort);
	}


	// Use this for initialization
	void Start()
	{

		m_prefeb_ListItem = Resources.Load("prefeb/device_item", typeof(GameObject)) as GameObject;
		m_listObj = transform.Find("device_list/Viewport/Content").gameObject;

		/*
		Transform container = transform.FindChild("device_list/Viewport/Content");
		while (container.transform.childCount > 0)
		{
			Transform child = container.transform.GetChild(0);
			child.SetParent(null);
			Destroy(child.gameObject);
		}
		*/


		transform.Find("Button_cancel").GetComponent<Button>().OnClickAsObservable()
				 .Subscribe((obj) =>
				 {
					 //gameObject.SetActive(false);
					 Destroy(this.gameObject);
				 }).AddTo(this);

		//select list
		gameObject.UpdateAsObservable()
				  .Where(_ => Input.GetMouseButtonDown(0))
				  .Subscribe((obj) =>
				  {
					  //Debug.Log("click");

					  Vector3 cur_mpos = Input.mousePosition;

					  for (int i = 0; i < m_listObj.transform.childCount; i++)
					  {
						  RectTransform rect_trn = m_listObj.transform.GetChild(i).GetComponent<RectTransform>();
						  if (RectTransformUtility.RectangleContainsScreenPoint(
										rect_trn,
										cur_mpos))
						  {
							  Debug.Log("click " + rect_trn.gameObject.name);
							  m_OnSelectStream.OnNext(rect_trn.GetComponent<device_item>());
							  //m_callback(rect_trn.GetComponent<device_item>());
							  gameObject.SetActive(false);

						  }

					  }
					  Destroy(this.gameObject);
				  }).AddTo(this);

		this.OnDestroyAsObservable().Subscribe((obj) => {
			Debug.Log("Script was destroyed");
			if (mUdpClient != null)
			{
				mUdpClient.Client.Blocking = false;
				//mUdpClient.Client.Blocking = false;
				//mUdpClient.Close();
				m_isAppQuitting = true;

				if (mUdpClient != null)
				{
					mUdpClient.Close();
				}

			}

		});


	}

	// Update is called once per frame
	void Update()
	{

	}
}
