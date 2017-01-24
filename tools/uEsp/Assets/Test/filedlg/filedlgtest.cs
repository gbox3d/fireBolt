using UnityEngine;

using System;
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

public class filedlgtest : MonoBehaviour {

	[SerializeField] GameObject m_btn1;
	[SerializeField] GameObject m_btn2;
	[SerializeField] GameObject m_btn3;

	[SerializeField] GameObject m_TestFileDlg;

	// Use this for initialization
	void Start () {

		int nCount = 0;
		m_btn1.GetComponent<Button> ().OnClickAsObservable ().Subscribe (_ => {

			m_TestFileDlg.GetComponent<com_gunpower_ui.FileDlg>().addItem("test" + nCount++,0);
			
		});


		m_btn2.GetComponent<Button>().OnClickAsObservable().Subscribe(_=> {

			//string strTests = "{\"init.lua\":989,\"stub.lua\":3765,\"readme.md\":2394,\"udp_safe_sender.lua\":1383,\"\":217,\"config.json\":218,\"tt\":989,\"app.lua\":3062,\"ext.lua\":92,\"three.js\":0,\"status.json\":18}";
			string strTests =  "{\"init.lua\":748,\"boot.lua\":3821,\"async_safe_sender.lua\":2836,\"test.json\":269,\"config.bak\":269,\"pu_team.lua\":388,\"boot.lc\":4392,\"pu_bat.lua\":392,\"pu_play.lua\":549,\"operate.lua\":376,\"async_safe_sender.lc\":1624,\"app.lc\":2628,\"pu_gun.lua\":330,\"pu_hit.lua\":288,\"uart.lua\":1530,\"app.lua\":2649,\"ext.lua\":363,\"status.json\":18,\"config.json\":270,\"test.lua\":990}";
			JsonData testDat = JsonMapper.ToObject(strTests);


			/*
			m_TestFileDlg.GetComponent<com_gunpower_ui.FileDlg>().m_OnSelectOK.Subscribe(__=> {

				Debug.Log("select file " + __);
				
			});
			m_TestFileDlg.GetComponent<com_gunpower_ui.FileDlg>().m_OnCancel.Subscribe(__=> {
				
				Debug.Log("cancel ");

			});
			*/

			Subject<string> OnSelect = new Subject<string>();
			Subject<string> OnCancel = new Subject<string>();

			OnSelect.Subscribe(__=> {
				Debug.Log(__);
			});
			OnCancel.Subscribe(__=> {
				Debug.Log(__);
			});

			m_TestFileDlg.GetComponent<com_gunpower_ui.FileDlg>().show(testDat,true,OnSelect,OnCancel);

		});


		
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
