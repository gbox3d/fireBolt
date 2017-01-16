﻿using UnityEngine;

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

using UniRx;
using UniRx.Triggers;

using UnityEngine.UI;

using LitJson;

using System.Linq;

public class AlertDlgBox : MonoBehaviour {

	public Button m_close_Btn;
	public delegate void dg_CallBack();

	dg_CallBack m_CallbackBtn;


	public void show(string title, string msg, string btn_text,dg_CallBack callback)
	{
		transform.localPosition = new Vector3(0, 0, 0);
		transform.FindChild("header/Text").GetComponent<Text>().text = title;
		transform.FindChild("body/Text").GetComponent<Text>().text = msg;
		transform.FindChild("body/Button/Text").GetComponent<Text>().text = btn_text;

		m_CallbackBtn = callback;

		gameObject.SetActive(true);

	}

	public void close()
	{
		
		m_CallbackBtn();
		gameObject.SetActive(false);
		Destroy(gameObject, 0.5f);
	}

	// Use this for initialization
	void Start () {

		//gameObject.SetActive(false);
		m_close_Btn = transform.FindChild("body/Button").GetComponent<Button>();

		m_close_Btn.OnClickAsObservable()
		           .Subscribe((obj) => {
			
					   close();
			
		}).AddTo(this);

		
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
