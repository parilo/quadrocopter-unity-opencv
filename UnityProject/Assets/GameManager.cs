using UnityEngine;
using UnityEngine.UI;
using System;
using System.Collections;
using System.Runtime.InteropServices;

public class GameManager : MonoBehaviour {
	
	private Texture2D cam1Tex;
	private int frameIndex = 0;
	
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
			
			// Передаем управление в плагин
			// Передающийся int можно использовать например для
			// определения какое действие надо совершить в плагине.
			// Я его использую как счетчик кадров в debug целях
			GL.IssuePluginEvent(GetRenderEventFunc(), frameIndex++);
		}
	}

	private void PassCamerasTexturesToPlugin () {
		// Создаем текстуру
		cam1Tex = new Texture2D(256,256,TextureFormat.ARGB32,false);
		// Убираем фильтрацию
		cam1Tex.filterMode = FilterMode.Point;
		// Вызов Apply() загружает текстуру в GPU
		cam1Tex.Apply();
		// Помещаем вновь созданную текстуру на наш канвас.
		// Да, мы меняем текстуру, заданную в предидущем параграфе
		GameObject.Find ("/CamerasTextures/Camera1RawImage").GetComponent<RawImage>().texture = cam1Tex;
		// Функции, которые непосредственно передают идентификатор текстуры в плагин
		#if UNITY_GLES_RENDERER
		SetTextureOfCam1 (cam1Tex.GetNativeTexturePtr(), cam1Tex.width, cam1Tex.height);
		#else
		SetTextureOfCam1 (cam1Tex.GetNativeTexturePtr());
		#endif
	}

// Так выглядит объявление функции передачи идентификатора текстуры
// Определение ее будет внутри плагина
#if UNITY_IPHONE && !UNITY_EDITOR
[DllImport ("__Internal")]
#else
// Здесь указывается имя подгружаемой динамической библиотеки, в которой лежит плагин
[DllImport ("QuadrocopterBrain")]
#endif
#if UNITY_GLES_RENDERER
private static extern void SetTextureOfCam1(System.IntPtr texture, int w, int h);
#else
private static extern void SetTextureOfCam1(System.IntPtr texture);
#endif

	#if UNITY_IPHONE && !UNITY_EDITOR
	[DllImport ("__Internal")]
	#else
	[DllImport("QuadrocopterBrain")]
	#endif
	private static extern IntPtr GetRenderEventFunc();
	
}
