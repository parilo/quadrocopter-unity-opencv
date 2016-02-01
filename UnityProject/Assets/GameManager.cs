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
	private double[] quadrocopterCurrentState;
	private double[] quadrocopterPreviousState;
	private Vector3 quadrocopterTargetPos;
	private double reward;
	private Vector3 quadrocopterPrevPos;
	private Vector3 quadrocopterCurPos;

	void Awake() {

		quadrocopterAction = -1;
		quadrocopterCurrentState = new double [11];
		quadrocopterPreviousState = new double [11];
		quadrocopterTargetPos = new Vector3 (0, 4, 0);

		quadrocopter = GameObject.Find ("/Quadrocopter").GetComponent<quadrocopterScript> ();

//		Application.targetFrameRate = 1;
		GameObject.Find ("/Quadrocopter/Frame/Camera1").GetComponent<Camera> ().aspect = 1.0f;
		GameObject.Find ("/Quadrocopter/Frame/Camera2").GetComponent<Camera> ().aspect = 1.0f;
//		enableScene (false);
//
//		preFilterSizeSlider = GameObject.Find ("/CamerasTextures/PreFilterSize").GetComponent<Slider> ();
//		preFilterCapSlider = GameObject.Find ("/CamerasTextures/PreFilterCap").GetComponent<Slider> ();
//		blockSizeSlider = GameObject.Find ("/CamerasTextures/BlockSize").GetComponent<Slider> ();
//		minDisparitySlider = GameObject.Find ("/CamerasTextures/MinDisparity").GetComponent<Slider> ();
//		numDisparitiesSlider = GameObject.Find ("/CamerasTextures/NumDisparities").GetComponent<Slider> ();
//		textureThresholdSlider = GameObject.Find ("/CamerasTextures/TextureThreshold").GetComponent<Slider> ();
//		uniquenessRatioSlider = GameObject.Find ("/CamerasTextures/UniquenessRatio").GetComponent<Slider> ();
//		speckleWindowSizeSlider = GameObject.Find ("/CamerasTextures/SpeckleWindowSize").GetComponent<Slider> ();
//		speckleRangeSlider = GameObject.Find ("/CamerasTextures/SpeckleRange").GetComponent<Slider> ();
//		disp12MaxDiffSlider = GameObject.Find ("/CamerasTextures/Disp12MaxDiff").GetComponent<Slider> ();
//		p1Slider = GameObject.Find ("/CamerasTextures/P1").GetComponent<Slider> ();
//		p2Slider = GameObject.Find ("/CamerasTextures/P2").GetComponent<Slider> ();
//
//		quadrocopterPos = GameObject.Find ("/Quadrocopter").GetComponent<Transform> ();
//		quadrocopterPosZSlider = GameObject.Find ("/CamerasTextures/QuadrocopterPosZ").GetComponent<Slider> ();
//
//		preFilterSizeValue = GameObject.Find ("/CamerasTextures/PreFilterSizeValue").GetComponent<InputField> ();
//		preFilterCapValue = GameObject.Find ("/CamerasTextures/PreFilterCapValue").GetComponent<InputField> ();
//		blockSizeValue = GameObject.Find ("/CamerasTextures/BlockSizeValue").GetComponent<InputField> ();
//		minDisparityValue = GameObject.Find ("/CamerasTextures/MinDisparityValue").GetComponent<InputField> ();
//		numDisparitiesValue = GameObject.Find ("/CamerasTextures/NumDisparitiesValue").GetComponent<InputField> ();
//		textureThresholdValue = GameObject.Find ("/CamerasTextures/TextureThresholdValue").GetComponent<InputField> ();
//		uniquenessRatioValue = GameObject.Find ("/CamerasTextures/UniquenessRatioValue").GetComponent<InputField> ();
//		speckleWindowSizeValue = GameObject.Find ("/CamerasTextures/SpeckleWindowSizeValue").GetComponent<InputField> ();
//		speckleRangeValue = GameObject.Find ("/CamerasTextures/SpeckleRangeValue").GetComponent<InputField> ();
//		disp12MaxDiffValue = GameObject.Find ("/CamerasTextures/Disp12MaxDiffValue").GetComponent<InputField> ();
//		p1Value = GameObject.Find ("/CamerasTextures/P1Value").GetComponent<InputField> ();
//		p2Value = GameObject.Find ("/CamerasTextures/P2Value").GetComponent<InputField> ();
//		
//		preFilterCapSlider.value = 5;
//		blockSizeSlider.value = 8;
//		numDisparitiesSlider.value = 7;
//		disp12MaxDiffSlider.value = 15;
//
//		onBMParametersChange ();
	}

	public void onBMParametersChange () {

//		quadrocopterPos.position = new Vector3(0, 2, quadrocopterPosZSlider.value);
//
//		int preFilterSize = (int)(preFilterSizeSlider.value * 2 + 1);
//		int preFilterCap = (int)preFilterCapSlider.value;
//		int blockSize = (int)(blockSizeSlider.value * 2 + 1);
//		int minDisparity = (int)minDisparitySlider.value;
//		int numDisparities = (int)(numDisparitiesSlider.value * 16);
//		int textureThreshold = (int)textureThresholdSlider.value;
//		int uniquenessRatio = (int)uniquenessRatioSlider.value;
//		int speckleWindowSize = (int)speckleWindowSizeSlider.value;
//		int speckleRange = (int)speckleRangeSlider.value;
//
//		int disp12MaxDiff = (int)disp12MaxDiffSlider.value;
//		int p1 = (int)p1Slider.value;
//		int p2 = (int)p2Slider.value;
//
//		preFilterSizeValue.text = "" + preFilterSize;
//		preFilterCapValue.text = "" + preFilterCap;
//		blockSizeValue.text = "" + blockSize;
//		minDisparityValue.text = "" + minDisparity;
//		numDisparitiesValue.text = "" + numDisparities;
//		textureThresholdValue.text = "" + textureThreshold;
//		uniquenessRatioValue.text = "" + uniquenessRatio;
//		speckleWindowSizeValue.text = "" + speckleWindowSize;
//		speckleRangeValue.text = "" + speckleRange;
//
//		disp12MaxDiffValue.text = "" + disp12MaxDiff;
//		p1Value.text = "" + p1;
//		p2Value.text = "" + p2;
//
//		setBMParameters (
//			preFilterSize,
//			preFilterCap, 
//			blockSize, 
//			minDisparity, 
//			numDisparities, 
//			textureThreshold, 
//			uniquenessRatio,
//			speckleWindowSize, 
//			speckleRange,
//			disp12MaxDiff
//		);
//
//		setSGBMParameters (
//			preFilterCap,
//			blockSize,
//			minDisparity,
//			numDisparities,
//			uniquenessRatio,
//			speckleWindowSize,
//			speckleRange,
//			disp12MaxDiff,
//			p1,
//			p2
//		);
	}

	//	void FixedUpdate () {
	//		readQuadrocopterState ();
	//	}

	void Update () {
		//		if (frameIndex < cameraCalibSamplesCount) {
		//			Transform t = GameObject.Find ("/CameraCalibrationPattern").GetComponent<Transform> ();
		//			t.rotation = 
		//				Quaternion.Euler (
		//					UnityEngine.Random.Range (-60f, 60f),
		//					UnityEngine.Random.Range (-60f, 60f),
		//					UnityEngine.Random.Range (-60f, 60f)
		//			);
		//
		//			t.position = new Vector3 (
		//				0 + 0.5f * UnityEngine.Random.Range (-1f, 1f),
		//				2 + 0.5f * UnityEngine.Random.Range (-1f, 1f),
		//				2.5f + 0.5f * UnityEngine.Random.Range (-1f, 4f)
		//			);
		//		} else if (frameIndex == cameraCalibSamplesCount) {
		//
		////			GameObject.Find ("/CameraCalibrationPattern").GetComponent<Transform> ().rotation = 
		////				Quaternion.Euler (
		////					0,
		////					0,
		////					0
		////					);
		//
		//			enableScene (true);
		//			Transform t = GameObject.Find ("/CameraCalibrationPattern").GetComponent<Transform> ();
		//			t.rotation = 
		//				Quaternion.Euler (
		//					UnityEngine.Random.Range (-60f, 60f),
		//					UnityEngine.Random.Range (-60f, 60f),
		//					UnityEngine.Random.Range (-60f, 60f)
		//					);
		//			
		//			t.position = new Vector3 (
		//				0 + 0.5f * UnityEngine.Random.Range (-1f, 1f),
		//				2 + 0.5f * UnityEngine.Random.Range (-1f, 1f),
		//				2.5f + 0.5f * UnityEngine.Random.Range (-1f, 4f)
		//				);
		//		} else {
		//			Application.targetFrameRate = 1;
		//		}
	}

	IEnumerator Start () {
		//		PassCamerasTexturesToPlugin ();
		yield return StartCoroutine ("CallPluginAtEndOfFrames");
	}

	private void readCameras () {

		RenderTexture cam1RT = GameObject.Find ("/Quadrocopter/Frame/Camera1").GetComponent<Camera>().targetTexture;
		// Активный Render Texture - это тот, с которого будут прочитаны пиксели
		RenderTexture.active = cam1RT;
		cam1Tex.ReadPixels(new Rect(0, 0, cam1RT.width, cam1RT.height), 0, 0);
		// Помещаем текстуру в GPU
		cam1Tex.Apply ();
		RenderTexture.active = null;

		RenderTexture cam2RT = GameObject.Find ("/Quadrocopter/Frame/Camera2").GetComponent<Camera>().targetTexture;
		// Активный Render Texture - это тот, с которого будут прочитаны пиксели
		RenderTexture.active = cam2RT;
		cam2Tex.ReadPixels(new Rect(0, 0, cam2RT.width, cam2RT.height), 0, 0);
		// Помещаем текстуру в GPU
		cam2Tex.Apply ();
		RenderTexture.active = null;

	}

	private void enableScene (bool enabled) {
		GameObject.Find ("/CameraCalibrationPattern").GetComponent<SpriteRenderer>().enabled = !enabled;

		Renderer[] renderers = GameObject.Find ("/CudilleroBakeFinal").GetComponentsInChildren<MeshRenderer>();

		foreach (Renderer r in renderers)
		{
			r.enabled = enabled;
		}
	}

	private void readQuadrocopterState (double[] state) {
		Quaternion rot = quadrocopter.getRotation ();
		quadrocopterCurPos = quadrocopter.getPosition ();
		Vector4 motorsPower = quadrocopter.getMotorsPower ();

		//нормируем вектор задающий направление к требуемой позиции
		Vector3 rvec = quadrocopterCurPos - quadrocopterTargetPos;
		if (rvec.magnitude > 5f) {
			rvec = rvec.normalized;
		} else {
			rvec /= 5f;
		}

		//нормируем данные о мощности двигателей
		motorsPower /= 40.0f;

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

	private void sendQuadrocopterState (double[] state) {
		SetQuadrocopterState (
			state [0],
			state [1],
			state [2],
			state [3],
			0,
			0,
			0,
			state [4],
			state [5],
			state [6],
			state [7],
			state [8],
			state [9],
			state [10]
		);
	}

	private void performQuadrocopterAction (long action) {
		switch (action) {
		case 0:
			quadrocopter.incMotorPower (0, 0.1);
			break;
		case 1:
			quadrocopter.incMotorPower (0, -0.1);
			break;
		case 2:
			//не делать ничего первым мотором
			break;
		case 3:
			quadrocopter.incMotorPower (1, 0.1);
			break;
		case 4:
			quadrocopter.incMotorPower (1, -0.1);
			break;
		case 5:
			//не делать ничего вторым мотором
			break;
		case 6:
			quadrocopter.incMotorPower (2, 0.1);
			break;
		case 7:
			quadrocopter.incMotorPower (2, -0.1);
			break;
		case 8:
			//не делать ничего третьим мотором
			break;
		case 9:
			quadrocopter.incMotorPower (3, 0.1);
			break;
		case 10:
			quadrocopter.incMotorPower (3, -0.1);
			break;
		case 11:
			//не делать ничего четвертым мотором
			break;
		}
	}

	private void calcReward () {
		if (quadrocopterAction < 0)
			return;
		
		//предидущее действие было

		long prevQuadrocopterAction = quadrocopterAction;

		Vector3 rPrev = quadrocopterPrevPos - quadrocopterTargetPos;
		Vector3 rCur = quadrocopterCurPos - quadrocopterTargetPos;

		Vector3 move = rCur - rPrev;
		reward = 10 * Vector3.Dot (move, - rPrev.normalized);

//		print ("prev dist " + rPrev.x + " " + rPrev.y + " " + rPrev.z + " " );
//		print ("cur dist " + rCur.x + " " + rCur.y + " " + rCur.z + " " );
//		print ("move " + move.x + " " + move.y + " " + move.z + " " );
//		print ("reward " + reward);
	}

	private void storeExperience () {
		if (quadrocopterAction < 0)
			return;
		
		//предидущее действие было
		if (reward > 5) return;

		StoreQuadrocopterExperience (
			quadrocopterPreviousState [0],
			quadrocopterPreviousState [1],
			quadrocopterPreviousState [2],
			quadrocopterPreviousState [3],
			quadrocopterPreviousState [4],
			quadrocopterPreviousState [5],
			quadrocopterPreviousState [6],
			quadrocopterPreviousState [7],
			quadrocopterPreviousState [8],
			quadrocopterPreviousState [9],
			quadrocopterPreviousState [10],

			quadrocopterCurrentState [0],
			quadrocopterCurrentState [1],
			quadrocopterCurrentState [2],
			quadrocopterCurrentState [3],
			quadrocopterCurrentState [4],
			quadrocopterCurrentState [5],
			quadrocopterCurrentState [6],
			quadrocopterCurrentState [7],
			quadrocopterCurrentState [8],
			quadrocopterCurrentState [9],
			quadrocopterCurrentState [10],

			reward,
			quadrocopterAction
		);
	}

	public void resetPrevAction () {
		quadrocopterAction = -1;
	}
	
	private IEnumerator CallPluginAtEndOfFrames () {
		while (true) {
			// Подождать пока выполнится рендеринг кадра
			yield return new WaitForEndOfFrame();

			// Передаем управление в плагин
			// Передающийся int можно использовать например для
			// определения какое действие надо совершить в плагине.
			// 1 - снять калибровочный сэмпл
			// 2 - выполнить калибровку
			// 3 - вычисление disparity map
			// 4 - q-learning
			int action;
//			if (frameIndex < cameraCalibSamplesCount) {
//				action = 1;
//				readCameras ();
//			} else if (frameIndex == cameraCalibSamplesCount) {
//				action = 2;
//				readCameras ();
//			} else {
//				readCameras ();
//				action = 3;
//			} 

			if (frameIndex % 4  == 0) {

				action = 4;

				readQuadrocopterState (quadrocopterCurrentState);
	//			collect reward
				calcReward ();
	//			store experience
				storeExperience ();

				sendQuadrocopterState (quadrocopterCurrentState);

				GL.IssuePluginEvent(GetRenderEventFunc(), action);

				quadrocopterAction = GetQuadrocopterAction ();
				performQuadrocopterAction (quadrocopterAction);

				quadrocopterCurrentState.CopyTo (quadrocopterPreviousState, 0);
				quadrocopterPrevPos.Set (
					quadrocopterCurPos.x,
					quadrocopterCurPos.y,
					quadrocopterCurPos.z
				);
			
			}

			frameIndex++;
		}
	}

	private void PassCamerasTexturesToPlugin () {
		// Создаем текстуру

		int texSize = 512;

		cam1Tex = new Texture2D(texSize, texSize,TextureFormat.ARGB32,false);
		cam2Tex = new Texture2D(texSize, texSize,TextureFormat.ARGB32,false);
		out1Tex = new Texture2D(texSize, texSize,TextureFormat.ARGB32,false);

		// Убираем фильтрацию
		cam1Tex.filterMode = FilterMode.Point;
		cam2Tex.filterMode = FilterMode.Point;
		out1Tex.filterMode = FilterMode.Point;

		// Вызов Apply() загружает текстуру в GPU
		cam1Tex.Apply();
		cam2Tex.Apply();
		out1Tex.Apply();

		// Помещаем вновь созданную текстуру на наш канвас.
		// Да, мы меняем текстуру, заданную в предидущем параграфе
		GameObject.Find ("/CamerasTextures/Camera1RawImage").GetComponent<RawImage>().texture = cam1Tex;
		GameObject.Find ("/CamerasTextures/Camera2RawImage").GetComponent<RawImage>().texture = cam2Tex;
		GameObject.Find ("/CamerasTextures/DisparityMap").GetComponent<RawImage>().texture = out1Tex;

		// Функции, которые непосредственно передают идентификатор текстуры в плагин
		SetTextureOfCam1 (cam1Tex.GetNativeTexturePtr());
		SetTextureOfCam2 (cam2Tex.GetNativeTexturePtr());
		SetTextureOut1 (out1Tex.GetNativeTexturePtr ());
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
	double currentRotW,
	double currentRotX,
	double currentRotY,
	double currentRotZ,

	double currentXVal,
	double currentYVal,
	double currentZVal,

	double targetXVal,
	double targetYVal,
	double targetZVal,

	double motor1powerVal,
	double motor2powerVal,
	double motor3powerVal,
	double motor4powerVal
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
private static extern void StoreQuadrocopterExperience (
	double currentRotWPrev,
	double currentRotXPrev,
	double currentRotYPrev,
	double currentRotZPrev,

	double targetXValPrev,
	double targetYValPrev,
	double targetZValPrev,

	double motor1powerValPrev,
	double motor2powerValPrev,
	double motor3powerValPrev,
	double motor4powerValPrev,

	double currentRotWNext,
	double currentRotXNext,
	double currentRotYNext,
	double currentRotZNext,

	double targetXValNext,
	double targetYValNext,
	double targetZValNext,

	double motor1powerValNext,
	double motor2powerValNext,
	double motor3powerValNext,
	double motor4powerValNext,

	double reward,
	long action
);


}
