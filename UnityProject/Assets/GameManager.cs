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
	private int cameraCalibSamplesCount = 250;

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

	void Awake() {
		Application.targetFrameRate = 1;
		GameObject.Find ("/Quadrocopter/Frame/Camera1").GetComponent<Camera> ().aspect = 1.0f;
		GameObject.Find ("/Quadrocopter/Frame/Camera2").GetComponent<Camera> ().aspect = 1.0f;
		enableScene (false);

		preFilterSizeSlider = GameObject.Find ("/CamerasTextures/PreFilterSize").GetComponent<Slider> ();
		preFilterCapSlider = GameObject.Find ("/CamerasTextures/PreFilterCap").GetComponent<Slider> ();
		blockSizeSlider = GameObject.Find ("/CamerasTextures/BlockSize").GetComponent<Slider> ();
		minDisparitySlider = GameObject.Find ("/CamerasTextures/MinDisparity").GetComponent<Slider> ();
		numDisparitiesSlider = GameObject.Find ("/CamerasTextures/NumDisparities").GetComponent<Slider> ();
		textureThresholdSlider = GameObject.Find ("/CamerasTextures/TextureThreshold").GetComponent<Slider> ();
		uniquenessRatioSlider = GameObject.Find ("/CamerasTextures/UniquenessRatio").GetComponent<Slider> ();
		speckleWindowSizeSlider = GameObject.Find ("/CamerasTextures/SpeckleWindowSize").GetComponent<Slider> ();
		speckleRangeSlider = GameObject.Find ("/CamerasTextures/SpeckleRange").GetComponent<Slider> ();
		disp12MaxDiffSlider = GameObject.Find ("/CamerasTextures/Disp12MaxDiff").GetComponent<Slider> ();
		p1Slider = GameObject.Find ("/CamerasTextures/P1").GetComponent<Slider> ();
		p2Slider = GameObject.Find ("/CamerasTextures/P2").GetComponent<Slider> ();

		quadrocopterPos = GameObject.Find ("/Quadrocopter").GetComponent<Transform> ();
		quadrocopterPosZSlider = GameObject.Find ("/CamerasTextures/QuadrocopterPosZ").GetComponent<Slider> ();

		preFilterSizeValue = GameObject.Find ("/CamerasTextures/PreFilterSizeValue").GetComponent<InputField> ();
		preFilterCapValue = GameObject.Find ("/CamerasTextures/PreFilterCapValue").GetComponent<InputField> ();
		blockSizeValue = GameObject.Find ("/CamerasTextures/BlockSizeValue").GetComponent<InputField> ();
		minDisparityValue = GameObject.Find ("/CamerasTextures/MinDisparityValue").GetComponent<InputField> ();
		numDisparitiesValue = GameObject.Find ("/CamerasTextures/NumDisparitiesValue").GetComponent<InputField> ();
		textureThresholdValue = GameObject.Find ("/CamerasTextures/TextureThresholdValue").GetComponent<InputField> ();
		uniquenessRatioValue = GameObject.Find ("/CamerasTextures/UniquenessRatioValue").GetComponent<InputField> ();
		speckleWindowSizeValue = GameObject.Find ("/CamerasTextures/SpeckleWindowSizeValue").GetComponent<InputField> ();
		speckleRangeValue = GameObject.Find ("/CamerasTextures/SpeckleRangeValue").GetComponent<InputField> ();
		disp12MaxDiffValue = GameObject.Find ("/CamerasTextures/Disp12MaxDiffValue").GetComponent<InputField> ();
		p1Value = GameObject.Find ("/CamerasTextures/P1Value").GetComponent<InputField> ();
		p2Value = GameObject.Find ("/CamerasTextures/P2Value").GetComponent<InputField> ();
		
		preFilterCapSlider.value = 5;
		blockSizeSlider.value = 8;
		numDisparitiesSlider.value = 7;
		disp12MaxDiffSlider.value = 15;

		onBMParametersChange ();
	}

	public void onBMParametersChange () {

		quadrocopterPos.position = new Vector3(0, 2, quadrocopterPosZSlider.value);

		int preFilterSize = (int)(preFilterSizeSlider.value * 2 + 1);
		int preFilterCap = (int)preFilterCapSlider.value;
		int blockSize = (int)(blockSizeSlider.value * 2 + 1);
		int minDisparity = (int)minDisparitySlider.value;
		int numDisparities = (int)(numDisparitiesSlider.value * 16);
		int textureThreshold = (int)textureThresholdSlider.value;
		int uniquenessRatio = (int)uniquenessRatioSlider.value;
		int speckleWindowSize = (int)speckleWindowSizeSlider.value;
		int speckleRange = (int)speckleRangeSlider.value;

		int disp12MaxDiff = (int)disp12MaxDiffSlider.value;
		int p1 = (int)p1Slider.value;
		int p2 = (int)p2Slider.value;

		preFilterSizeValue.text = "" + preFilterSize;
		preFilterCapValue.text = "" + preFilterCap;
		blockSizeValue.text = "" + blockSize;
		minDisparityValue.text = "" + minDisparity;
		numDisparitiesValue.text = "" + numDisparities;
		textureThresholdValue.text = "" + textureThreshold;
		uniquenessRatioValue.text = "" + uniquenessRatio;
		speckleWindowSizeValue.text = "" + speckleWindowSize;
		speckleRangeValue.text = "" + speckleRange;

		disp12MaxDiffValue.text = "" + disp12MaxDiff;
		p1Value.text = "" + p1;
		p2Value.text = "" + p2;

		setBMParameters (
			preFilterSize,
			preFilterCap, 
			blockSize, 
			minDisparity, 
			numDisparities, 
			textureThreshold, 
			uniquenessRatio,
			speckleWindowSize, 
			speckleRange,
			disp12MaxDiff
		);

		setSGBMParameters (
			preFilterCap,
			blockSize,
			minDisparity,
			numDisparities,
			uniquenessRatio,
			speckleWindowSize,
			speckleRange,
			disp12MaxDiff,
			p1,
			p2
		);
	}

	void Update () {
		if (frameIndex < cameraCalibSamplesCount) {
			Transform t = GameObject.Find ("/CameraCalibrationPattern").GetComponent<Transform> ();
			t.rotation = 
				Quaternion.Euler (
					UnityEngine.Random.Range (-60f, 60f),
					UnityEngine.Random.Range (-60f, 60f),
					UnityEngine.Random.Range (-60f, 60f)
			);

			t.position = new Vector3 (
				0 + 0.5f * UnityEngine.Random.Range (-1f, 1f),
				2 + 0.5f * UnityEngine.Random.Range (-1f, 1f),
				2.5f + 0.5f * UnityEngine.Random.Range (-1f, 4f)
			);
		} else if (frameIndex == cameraCalibSamplesCount) {

//			GameObject.Find ("/CameraCalibrationPattern").GetComponent<Transform> ().rotation = 
//				Quaternion.Euler (
//					0,
//					0,
//					0
//					);

			enableScene (true);
			Transform t = GameObject.Find ("/CameraCalibrationPattern").GetComponent<Transform> ();
			t.rotation = 
				Quaternion.Euler (
					UnityEngine.Random.Range (-60f, 60f),
					UnityEngine.Random.Range (-60f, 60f),
					UnityEngine.Random.Range (-60f, 60f)
					);
			
			t.position = new Vector3 (
				0 + 0.5f * UnityEngine.Random.Range (-1f, 1f),
				2 + 0.5f * UnityEngine.Random.Range (-1f, 1f),
				2.5f + 0.5f * UnityEngine.Random.Range (-1f, 4f)
				);
		} else {
			Application.targetFrameRate = 1;
		}
	}

	IEnumerator Start () {
		PassCamerasTexturesToPlugin ();
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
	
	private IEnumerator CallPluginAtEndOfFrames () {
		while (true) {
			// Подождать пока выполнится рендеринг кадра
			yield return new WaitForEndOfFrame();

			// Передаем управление в плагин
			// Передающийся int можно использовать например для
			// определения какое действие надо совершить в плагине.
			// 1 - снять калибровочный сэмпл
			// 2 - выполнить калибровку
			// 3 - 
			int action;
			if (frameIndex < cameraCalibSamplesCount) {
				action = 1;
				readCameras ();
			} else if (frameIndex == cameraCalibSamplesCount) {
				action = 2;
				readCameras ();
			} else {
				readCameras ();
				action = 3;
			}
			GL.IssuePluginEvent(GetRenderEventFunc(), action);
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

}
