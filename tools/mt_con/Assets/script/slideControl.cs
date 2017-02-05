using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

using UniRx;
using UniRx.Triggers;

public class slideControl : MonoBehaviour
{
	public float m_fGraderValue;
	// Use this for initialization
	void Start ()
	{
		
		m_fGraderValue = 0;

#if UNITY_ANDROID
		//touch
		{
			int nFsm = 0;
			int nSelect_touch_id = -1;
			Vector2 startPos = new Vector2 ();

			GameObject grader = transform.FindChild ("grader").gameObject;
			this.UpdateAsObservable ().Subscribe ((obj) => {

				switch (nFsm) {
				case 0:
					{
						// 현재 터치되어 있는 카운트 가져오기
						int cnt = Input.touchCount;
						for (int i = 0; i < cnt; ++i) {
							Touch touch = Input.GetTouch (i);
							Vector2 pos = touch.position;

							if (touch.phase == TouchPhase.Began) {
								if (RectTransformUtility.RectangleContainsScreenPoint (
									   transform.GetComponent<RectTransform> (), pos) == true) {
									//Debug.Log ("hit" + gameObject.name + " " + i);
									nSelect_touch_id = i;
									startPos = pos;
									nFsm = 10;
									break;
								}


							}

						}

					}
					break;
				case 10:
					{
						int cnt = Input.touchCount;

						//for (int i = 0; i < cnt; ++i)
						if (nSelect_touch_id <= (cnt - 1)) {
							Touch touch = Input.GetTouch (nSelect_touch_id);
							Vector2 pos = touch.position;
							if (touch.phase == TouchPhase.Moved) {
								m_fGraderValue = (startPos - pos).y;
								//Debug.Log("move : " + (startPos - pos).y);
								//grader.GetComponent<RectTransform>().anchoredPosition = new Vector2(0,(startPos - pos).y);

							} else if ((touch.phase == TouchPhase.Ended)) {
								//Debug.Log ("end");
								nFsm = 0;
								m_fGraderValue = 0;
								nSelect_touch_id = -1;
								//grader.GetComponent<RectTransform>().anchoredPosition = new Vector2(0, 0);
							}
						} else {
							//Debug.Log ("force end");
							nFsm = 0;
							m_fGraderValue = 0;
							nSelect_touch_id = -1;
						}
						break;
					}
				}

				grader.GetComponent<RectTransform> ().anchoredPosition = new Vector2 (0, -m_fGraderValue);
			});
		}

				#else

		//mouse
		{
			int nFsm = 0;

			Vector2 startPos = new Vector2 ();

			GameObject grader = transform.FindChild ("grader").gameObject;
			this.UpdateAsObservable ().Subscribe ((obj) => {
				Vector2 pos = Input.mousePosition;
				switch (nFsm) {
				case 0:
					{
						if(Input.GetMouseButton(0) == true && RectTransformUtility.RectangleContainsScreenPoint (
							transform.GetComponent<RectTransform> (), pos) == true
						) {
							startPos = pos;
							nFsm = 10;
						}
					}
					break;
				case 10:
					{

						if(Input.GetMouseButton(0) == false ) {
							nFsm = 0;
							m_fGraderValue = 0;
						}
						else {
							
							
							m_fGraderValue = (startPos - pos).y;
						}

						break;
					}
				}
				grader.GetComponent<RectTransform> ().anchoredPosition = new Vector2 (0, -m_fGraderValue);
			});
		}


		#endif
		
	}




	// Update is called once per frame
	void Update ()
	{

	}
}
