using UnityEngine;
using UnityEngine.UI;
using System;
using System.Collections;
using System.Runtime.InteropServices;

public class GameController : MonoBehaviour {

	public GameObject quadrocopter;
	private GameObject currentQuadrocopter;

	// Use this for initialization
	void Start () {
		
		//		Application.targetFrameRate = 10;
		
		Destroy(GameObject.Find ("/Quadrocopter"));
		
		currentQuadrocopter = Instantiate(quadrocopter);
		currentQuadrocopter.name = "Quadrocopter";
		
	}

	public void onResetClicked () {
		
		Destroy (currentQuadrocopter);
		currentQuadrocopter = Instantiate(quadrocopter);
		currentQuadrocopter.name = "Quadrocopter";
		
	}
}