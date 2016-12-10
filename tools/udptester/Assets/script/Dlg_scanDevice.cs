using UnityEngine;

using System;
using System.Text;
using System.Net;
using System.Net.Sockets;

using UniRx;
using UniRx.Triggers;

using UnityEngine.UI;

using LitJson;

public class Dlg_scanDevice : MonoBehaviour
{

	GameObject m_prefeb_ListItem;
	GameObject m_listObj;

	public delegate void MyDelegateType(device_item item);

	public MyDelegateType m_callback;

	public void processPaket(JsonData jsnDat)
	{

		if (gameObject.activeSelf)
		{
			//RectTransform[] items = m_listObj.GetComponentsInChildren<RectTransform>();

			string strName = "item_" + jsnDat["ip"].ToJson();
			if (m_listObj.transform.FindChild(strName) == null)
			{
				GameObject itemObj = UnityEngine.Object.Instantiate(m_prefeb_ListItem) as GameObject;
				itemObj.name = strName;
				itemObj.transform.FindChild("Text").GetComponent<Text>().text = jsnDat["ip"].ToString();
				itemObj.transform.SetParent(m_listObj.transform);
				itemObj.transform.localPosition = new Vector3((568 / 2), 0 - m_listObj.transform.childCount * 36, 0);
				itemObj.GetComponent<device_item>().m_strIP = jsnDat["ip"].ToString();
				Debug.Log(jsnDat.ToJson());
			}


		}
	}


	// Use this for initialization
	void Start()
	{

		m_prefeb_ListItem = Resources.Load("prefeb/device_item", typeof(GameObject)) as GameObject;
		m_listObj = transform.FindChild("device_list/Viewport/Content").gameObject;

		this.OnEnableAsObservable()
			.Subscribe((obj) =>
			{

				Transform container = transform.FindChild("device_list/Viewport/Content");
				while (container.transform.childCount > 0)
				{
					Transform child = container.transform.GetChild(0);
					child.SetParent(null);
					Destroy(child.gameObject);

				}
				//transform.FindChild("device_list/Viewport/Content").DetachChildren();

			}).AddTo(this);

		transform.FindChild("Button_cancel").GetComponent<Button>().OnClickAsObservable()
				 .Subscribe((obj) =>
				 {
					 gameObject.SetActive(false);
				 }).AddTo(this);

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
							  m_callback(rect_trn.GetComponent<device_item>());
							  gameObject.SetActive(false);

						  }

					  }
				  }).AddTo(this);


	}

	// Update is called once per frame
	void Update()
	{

	}
}
