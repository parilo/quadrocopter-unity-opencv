using UnityEngine;
using UnityEngine.UI;
using System;
using System.Collections;
using System.Runtime.InteropServices;

public class GameManager : MonoBehaviour {
	
	private Texture2D cam1Tex;
	private Texture2D cam2Tex;
	private Texture2D out1Tex;
	private int frameIndex = 0;
	private int cameraCalibSamplesCount = 0;//250;

	private Slider preFilterSizeSlider;
	private Slider preFilterCapSlider;
	private Slider blockSizeSlider;
	private Slider minDisparitySlider;
	private Slider numDisparitiesSlider;
	private Slider textureThresholdSlider;
	private Slider uniquenessRatioSlider;
	private Slider speckleWindowSizeSlider;
	private Slider speckleRangeSlider;

	private Slider disp12MaxDiffSlider;
	private Slider p1Slider;
	private Slider p2Slider;

	private Transform quadrocopterPos;
	private Slider quadrocopterPosZSlider;

	private InputField preFilterSizeValue;
	private InputField preFilterCapValue;
	private InputField blockSizeValue;
	private InputField minDisparityValue;
	private InputField numDisparitiesValue;
	private InputField textureThresholdValue;
	private InputField uniquenessRatioValue;
	private InputField speckleWindowSizeValue;
	private InputField speckleRangeValue;

	private InputField disp12MaxDiffValue;
	private InputField p1Value;
	private InputField p2Value;

	private quadrocopterScript quadrocopter;
	private long quadrocopterAction;
	private float[] quadrocopterCurrentState;
	private float[] quadrocopterPreviousState;
	private Vector3 quadrocopterTargetPos;
	private double reward;
	private Vector3 quadrocopterPrevPos;
	private Vector3 quadrocopterCurPos;

	//continous
	private float[] actionsScores;

	void Awake() {

//		Time.timeScale = 2f;
//		Time.fixedDeltaTime = 0.02f * Time.timeScale;

		actionsScores = new float[8];

		quadrocopterAction = -1;
		quadrocopterCurrentState = new float [11];
		quadrocopterPreviousState = new float [11];
		quadrocopterTargetPos = new Vector3 (1, 2, 0);

		quadrocopter = GameObject.Find ("/Quadrocopter").GetComponent<quadrocopterScript> ();

//		Application.targetFrameRate = 1;
		GameObject.Find ("/Quadrocopter/Frame/Camera1").GetComponent<Camera> ().aspect = 1.0f;
		GameObject.Find ("/Quadrocopter/Frame/Camera2").GetComponent<Camera> ().aspect = 1.0f;

	}

	private int fixedUpdateCount = 0;

	void FixedUpdate () {
//		quadrocopter.setRotation (Quaternion.Euler (0f, 0f, 0f));
//		fixedUpdateCount++;
//		readQuadrocopterState ();
	}

	IEnumerator Start () {
		//		PassCamerasTexturesToPlugin ();
		yield return StartCoroutine ("CallPluginAtEndOfFrames");
	}

	private void readQuadrocopterState (float[] state) {
		Quaternion rot = quadrocopter.getRotation ();
		quadrocopterCurPos = quadrocopter.getPosition ();
		Vector4 motorsPower = quadrocopter.getMotorsPower ();

		//нормируем вектор задающий направление к требуемой позиции
		Vector3 rvec = quadrocopterCurPos - quadrocopterTargetPos;
//		if (rvec.magnitude > 5f) {
//			rvec = rvec.normalized;
//		} else {
//			rvec /= 5f;
//		}

		//нормируем данные о мощности двигателей
//		motorsPower /= 30.0f;

		state [0] = rot.w;
		state [1] = rot.x;
		state [2] = rot.y;
		state [3] = rot.z;
		state [4] = rvec.x;
		state [5] = rvec.y;
		state [6] = rvec.z;
		state [7] = motorsPower [0];
		state [8] = motorsPower [1];
		state [9] = motorsPower [2];
		state [10] = motorsPower [3];
	}

	private void sendQuadrocopterState (float[] state) {
		SetQuadrocopterState (state);
	}

	private void performQuadrocopterAction (long action) {
		double strength = 0.1;
		switch (action) {
		case 0:
			quadrocopter.incMotorPower (1, strength);
			break;
		case 1:
			quadrocopter.incMotorPower (1, -strength);
			break;
		case 2:
			quadrocopter.incMotorPower (1, strength);
			break;
		case 3:
			quadrocopter.incMotorPower (1, -strength);
			break;
		case 4:
			quadrocopter.incMotorPower (3, strength);
			break;
		case 5:
			quadrocopter.incMotorPower (3, -strength);
			break;
		case 6:
			quadrocopter.incMotorPower (3, strength);
			break;
		case 7:
			quadrocopter.incMotorPower (3, -strength);
			break;
		}
	}

	private void performQuadrocopterActionContinous () {
		double strength = 0.1;
		quadrocopter.incMotorPower (0, strength * (actionsScores [0] - actionsScores [1]));
		quadrocopter.incMotorPower (1, strength * (actionsScores [2] - actionsScores [3]));
		quadrocopter.incMotorPower (2, strength * (actionsScores [4] - actionsScores [5]));
		quadrocopter.incMotorPower (3, strength * (actionsScores [6] - actionsScores [7]));
	}

	private void calcReward () {
		if (frameIndex == 0)
			return;
		
		//предидущее действие было

		long prevQuadrocopterAction = quadrocopterAction;

		//награда приближение к позиции
		double nextDist = Vector3.Magnitude (quadrocopterTargetPos - quadrocopterCurPos);
		double prevDist = Vector3.Magnitude (quadrocopterTargetPos - quadrocopterPrevPos);

		reward = 50 * (prevDist - nextDist);
		print ("cur pos " + quadrocopterCurPos + " dist " + nextDist + " reward " + reward);
//		if (prevDist < nextDist) {
//			reward = -5;
//		} else {
//			reward = 5;
//		}
		if (Math.Abs (prevDist - nextDist) > 1) {
			reward = 0;
		}
//		print ("prevDist: " + prevDist + " next dist: " + nextDist + "reward: " + reward);

		//награда за вертикальное положение
//		double cos = Quaternion.Dot (quadrocopter.getRotation (), Quaternion.Euler (0, 0, 0));
//		reward = - 5 * Math.Sqrt ((1 - cos)*0.5) + 0.2;
//		if (reward > 0)
//			reward *= 20;
//		reward = - 5 * Math.Sqrt (1 - cos * cos) + 1.5;

//		//награда за приближение к указаной точке
//		Vector3 rPrev = quadrocopterPrevPos - quadrocopterTargetPos;
//		Vector3 rCur = quadrocopterCurPos - quadrocopterTargetPos;
//
//		Vector3 move = rCur - rPrev;
//		reward = 10 * Vector3.Dot (move, - rPrev.normalized);

//		print ("prev dist " + rPrev.x + " " + rPrev.y + " " + rPrev.z + " " );
//		print ("cur dist " + rCur.x + " " + rCur.y + " " + rCur.z + " " );
//		print ("move " + move.x + " " + move.y + " " + move.z + " " );
//		print ("reward " + reward);
	}

	private void storeExperience () {
		if (frameIndex == 0)
			return;

		//предидущее действие было

		if (reward > 5) return;

		StoreQuadrocopterExperience (
			quadrocopterPreviousState,
			quadrocopterCurrentState,
			reward,
			quadrocopterAction
		);

	}

	private void storeExperienceContious () {
		if (frameIndex == 0)
			return;

		//предидущее действие было

		if (reward > 5) return;

		StoreQuadrocopterExperienceContinous (
			quadrocopterPreviousState,
			quadrocopterCurrentState,
			reward,
			actionsScores
		);
	}

	public void resetPrevAction () {
		quadrocopterAction = -1;
	}
	
	private IEnumerator CallPluginAtEndOfFrames () {
		while (true) {
			yield return new WaitForFixedUpdate ();


			//discrete
			readQuadrocopterState (quadrocopterCurrentState);
//			collect reward
			calcReward ();
//			store experience
			storeExperience ();

			sendQuadrocopterState (quadrocopterCurrentState);

			QuadrocopterBrainAct ();

			quadrocopterAction = GetQuadrocopterAction ();
			print ("action: " + quadrocopterAction);
			performQuadrocopterAction (quadrocopterAction);



//			GetQuadrocopterActionsScores (actionsScores);
//			string str = "";
//			foreach (float item in actionsScores) {
//				str += item + " ";
//			}
//			print ("actions scores: " + str);
//			performQuadrocopterActionContinous ();



			quadrocopterCurrentState.CopyTo (quadrocopterPreviousState, 0);
			quadrocopterPrevPos.Set (
				quadrocopterCurPos.x,
				quadrocopterCurPos.y,
				quadrocopterCurPos.z
			);

			frameIndex++;
		}
	}

// Так выглядит объявление функции передачи идентификатора текстуры
// Определение ее будет внутри плагина
#if UNITY_IPHONE && !UNITY_EDITOR
[DllImport ("__Internal")]
#else
// Здесь указывается имя подгружаемой динамической библиотеки, в которой лежит плагин
[DllImport ("QuadrocopterBrain")]
#endif
private static extern void SetTextureOfCam1(System.IntPtr texture);

#if UNITY_IPHONE && !UNITY_EDITOR
[DllImport ("__Internal")]
#else
// Здесь указывается имя подгружаемой динамической библиотеки, в которой лежит плагин
[DllImport ("QuadrocopterBrain")]
#endif
private static extern void SetTextureOfCam2(System.IntPtr texture);
	
#if UNITY_IPHONE && !UNITY_EDITOR
[DllImport ("__Internal")]
#else
// Здесь указывается имя подгружаемой динамической библиотеки, в которой лежит плагин
[DllImport ("QuadrocopterBrain")]
#endif
private static extern void SetTextureOut1(System.IntPtr texture);

// Объявление функции, которая будет возвращать функцию,
// куда будет передано управление в C++
#if UNITY_IPHONE && !UNITY_EDITOR
[DllImport ("__Internal")]
#else
[DllImport("QuadrocopterBrain")]
#endif
private static extern IntPtr GetRenderEventFunc();


#if UNITY_IPHONE && !UNITY_EDITOR
[DllImport ("__Internal")]
#else
[DllImport("QuadrocopterBrain")]
#endif
private static extern void setBMParameters (
	int preFilterSize,
	int preFilterCap,
	int blockSize,
	int minDisparity,
	int numDisparities,
	int textureThreshold,
	int uniquenessRatio,
	int speckleWindowSize,
	int speckleRange,
	int disp12maxDiff
);

#if UNITY_IPHONE && !UNITY_EDITOR
[DllImport ("__Internal")]
#else
[DllImport("QuadrocopterBrain")]
#endif
private static extern void setSGBMParameters (
	int preFilterCap,
	int blockSize,
	int minDisparity,
	int numDisparities,
	int uniquenessRatio,
	int speckleWindowSize,
	int speckleRange,
	int disp12maxDiff,
	int p1,
	int p2
);

#if UNITY_IPHONE && !UNITY_EDITOR
[DllImport ("__Internal")]
#else
[DllImport("QuadrocopterBrain")]
#endif
private static extern void SetQuadrocopterState (
	[In, Out] float[] state
);

#if UNITY_IPHONE && !UNITY_EDITOR
[DllImport ("__Internal")]
#else
[DllImport("QuadrocopterBrain")]
#endif
private static extern long GetQuadrocopterAction();

#if UNITY_IPHONE && !UNITY_EDITOR
[DllImport ("__Internal")]
#else
[DllImport("QuadrocopterBrain")]
#endif
private static extern long GetQuadrocopterActionsScores([In, Out] float[] actionsScores);

#if UNITY_IPHONE && !UNITY_EDITOR
[DllImport ("__Internal")]
#else
[DllImport("QuadrocopterBrain")]
#endif
private static extern void StoreQuadrocopterExperience (
	[In, Out] float[] prevState,
	[In, Out] float[] nextState,
	double reward,
	long action
);

#if UNITY_IPHONE && !UNITY_EDITOR
[DllImport ("__Internal")]
#else
[DllImport("QuadrocopterBrain")]
#endif
private static extern void StoreQuadrocopterExperienceContinous (
	[In, Out] float[] prevState,
	[In, Out] float[] nextState,
	double reward,
	[In, Out] float[] actionsScores
);
		
#if UNITY_IPHONE && !UNITY_EDITOR
[DllImport ("__Internal")]
#else
[DllImport("QuadrocopterBrain")]
#endif
private static extern void QuadrocopterBrainAct();


}
