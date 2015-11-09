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
	private int cameraCalibSamplesCount = 50;

	void Awake() {
		Application.targetFrameRate = 1;
		GameObject.Find ("/Quadrocopter/Frame/Camera1").GetComponent<Camera> ().aspect = 1.0f;
		GameObject.Find ("/Quadrocopter/Frame/Camera2").GetComponent<Camera> ().aspect = 1.0f;
	}

	void Update () {
		if (frameIndex < cameraCalibSamplesCount) {
			Transform t = GameObject.Find ("/CameraCalibrationPattern").GetComponent<Transform> ();
			t.rotation = 
				Quaternion.Euler (
					UnityEngine.Random.Range (-40f, 40f),
					UnityEngine.Random.Range (-40f, 40f),
					UnityEngine.Random.Range (-40f, 40f)
				);

			t.position = new Vector3 (
				0 + 0.5f * UnityEngine.Random.Range (-1f, 1f),
				2 + 0.5f * UnityEngine.Random.Range (-1f, 1f),
				3 + 0.5f * UnityEngine.Random.Range (-1f, 3f)
			);
		}
//		else if (frameIndex == cameraCalibSamplesCount) {
//			GameObject.Find ("/CameraCalibrationPattern").GetComponent<Transform> ().rotation = 
//				Quaternion.Euler (
//					0,
//					0,
//					0
//					);
//		}
	}

	IEnumerator Start () {
		PassCamerasTexturesToPlugin ();
		yield return StartCoroutine ("CallPluginAtEndOfFrames");
	}
	
	private IEnumerator CallPluginAtEndOfFrames () {
		while (true) {
			// Подождать пока выполнится рендеринг кадра
			yield return new WaitForEndOfFrame();

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

			// Передаем управление в плагин
			// Передающийся int можно использовать например для
			// определения какое действие надо совершить в плагине.
			// 1 - снять калибровочный сэмпл
			// 2 - выполнить калибровку
			// 3 - 
			int action;
			if (frameIndex < cameraCalibSamplesCount) {
				action = 1;
			} else if (frameIndex == cameraCalibSamplesCount) {
				action = 2;
			} else {
				action = 3;
			}
			GL.IssuePluginEvent(GetRenderEventFunc(), action);
			frameIndex++;
		}
	}

	private void PassCamerasTexturesToPlugin () {
		// Создаем текстуру
		cam1Tex = new Texture2D(1024, 1024,TextureFormat.ARGB32,false);
		cam2Tex = new Texture2D(1024, 1024,TextureFormat.ARGB32,false);
		out1Tex = new Texture2D(1024, 1024,TextureFormat.ARGB32,false);

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

}
