//
//  Main.cpp
//  QuadrocopterBrain
//
//  Created by anton on 18.10.15.
//  Copyright © 2015 anton. All rights reserved.
//

#include <math.h>
#include <stdio.h>
#include <vector>
#include <string>

#include "Unity/IUnityGraphics.h"

#include "Main.h"
#include "CameraCalibrator.hpp"
#include "StereoCalibrator.hpp"
#include "CameraUndistortion.hpp"
#include "DisparityMapCalculator.hpp"
#include "OdometryLibviso2.hpp"

// --------------------------------------------------------------------------
// Include headers for the graphics APIs we support

#if SUPPORT_D3D9
	#include <d3d9.h>
	#include "Unity/IUnityGraphicsD3D9.h"
#endif
#if SUPPORT_D3D11
	#include <d3d11.h>
	#include "Unity/IUnityGraphicsD3D11.h"
#endif
#if SUPPORT_D3D12
	#include <d3d12.h>
	#include "Unity/IUnityGraphicsD3D12.h"
#endif

#if SUPPORT_OPENGLES
	#if UNITY_IPHONE
		#include <OpenGLES/ES2/gl.h>
	#elif UNITY_ANDROID
		#include <GLES2/gl2.h>
	#endif
#elif SUPPORT_OPENGL
	#if UNITY_WIN || UNITY_LINUX
		#include <GL/gl.h>
	#else
		#include <OpenGL/gl.h>
	#endif
#endif

// Prints a string
void DebugLog (std::string str)
{
#if UNITY_WIN
    OutputDebugStringA (str.c_str ());
#else
    fprintf(stderr, "%s\n", str.c_str ());
#endif
}


	
//переменные для хранения идентификатора текстуры
static void* g_Cam1TexturePointer = NULL;

//Определение функции передачи идентификатора текстуры.
//Объявление находится в C# скрипте
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetTextureOfCam1(void* texturePtr)
{
    g_Cam1TexturePointer = texturePtr;
}

static void* g_Cam2TexturePointer = NULL;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetTextureOfCam2(void* texturePtr)
{
    g_Cam2TexturePointer = texturePtr;
}

static void* g_Out1TexturePointer = NULL;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetTextureOut1(void* texturePtr)
{
    g_Out1TexturePointer = texturePtr;
}


CameraCalibrator camera1Calibrator;
CameraCalibrator camera2Calibrator;
StereoCalibrator stereoCalibrator;
//CameraUndistortion camera1Undistortion;
//CameraUndistortion camera2Undistortion;
DisparityMapCalculator disparityMapCalculator;
OdometryLibviso2 odometryLibviso2;

void getMatFromTexture (void* texturePointer, cv::Mat& mat) {
//если вы посмотрите в пример RenderingPluginExampleXX.zip
//то увидите здесь большее количество кода для разных платформ, я оставил здесь только OpenGL.
//Передача текстур на разных платформах осуществляется по разному, если у вас другая платформа
//обратитесь к коду примера
#if SUPPORT_OPENGL
    if (texturePointer) {
	
        GLuint gltex = (GLuint)(size_t)(texturePointer);
        glBindTexture (GL_TEXTURE_2D, gltex);
        int texWidth, texHeight;
        glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
        glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);
		
        // Считывание текстуры в матрицу
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, mat.data);
		
    }
#endif
}

void setTextureFromMat (const cv::Mat& mat, void* texturePointer) {
//если вы посмотрите в пример RenderingPluginExampleXX.zip
//то увидите здесь большее количество кода для разных платформ, я оставил здесь только OpenGL.
//Передача текстур на разных платформах осуществляется по разному, если у вас другая платформа
//обратитесь к коду примера
#if SUPPORT_OPENGL
    if (texturePointer) {
	
        GLuint gltex = (GLuint)(size_t)(texturePointer);
        glBindTexture (GL_TEXTURE_2D, gltex);
		
        // Загружаем назад в память текстуру, чтобы отобразить ее в Unity.
        // Unity не позволяет обращаться к GUI функциям OpenCV, так что никакие imshow работать не будут
        // Поэтому весь вывод графической информации надо осуществлять передачей ее в Unity
        glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, mat.rows, mat.cols, GL_RGBA, GL_UNSIGNED_BYTE, mat.data);
    }
#endif
}

int foundSamples = 0;

cv::Mat image1;
cv::Mat image2;
cv::Mat image1r, image2r, disparityMap;
cv::Mat l1, l2, r1, r2;

void processRenderEvent (int eventID) {
	
	using namespace std;
	using namespace cv;
	
	stringstream outs;
	
	// Матрица OpenCV, в которую будет считана текстура
	
	getMatFromTexture(g_Cam1TexturePointer, l1);
	getMatFromTexture(g_Cam2TexturePointer, r1);
	
	// при считывании изображений из OpenGL получается так, что они
	// оказываются зеркально отображены,
	// сказываются особенности хранения текстур в OpenGL и OpenCV
	flip(l1, l2, 1);
	flip(r1, r2, 1);
	flip(l2, image1, 0);
	flip(r2, image2, 0);

	switch (eventID) {
		case 1:
		
			camera1Calibrator.findSample(image1);
			camera2Calibrator.findSample(image2);
			if (camera1Calibrator.isSampleFound() && camera2Calibrator.isSampleFound()) {
				camera1Calibrator.acceptSample();
				camera2Calibrator.acceptSample();

				outs.clear();
				outs << "--- foundSamples: " << ++foundSamples;
				DebugLog(outs.str());
			
			}
			
			flip(image1, l1, 1);
			flip(image2, r1, 1);
			flip(l1, image1, 0);
			flip(r1, image2, 0);
			setTextureFromMat(image1, g_Cam1TexturePointer);
			setTextureFromMat(image2, g_Cam2TexturePointer);
			
		break;
		
		case 2:{
		
//			camera1Calibrator.makeCalibration();
//			camera2Calibrator.makeCalibration();
//			
//			Mat cam1 = camera1Calibrator.getCameraMatrix();
//			Mat dist1 = camera1Calibrator.getDistCoeffs();
//			Mat cam2 = camera2Calibrator.getCameraMatrix();
//			Mat dist2 = camera2Calibrator.getDistCoeffs();

//			//512, 70 deg, -0.25, 0.25
//			Mat cam1 = (Mat_<double>(3, 3) <<
//			355.3383989449604, 0, 258.0008490063121,
//			 0, 354.5068750418187, 255.7252273330564,
//			 0, 0, 1);
//
//			Mat dist1 = (Mat_<double>(5, 1) <<
//			-0.02781875153957544,
//			 0.05084431574408409,
//			 0.0003262438299225566,
//			 0.0005420218184546293,
//			 -0.06711413339515834);
//
//			Mat cam2 = (Mat_<double>(3, 3) <<
//			354.8366825622115, 0, 255.7668702403205,
//			 0, 353.9950515096826, 254.3218524455621,
//			 0, 0, 1);
//
//			Mat dist2 = (Mat_<double>(12, 1) <<
//			-0.03429254591232522,
//			 0.04304840389703278,
//			 -0.0005799461588668822,
//			 0.0005396568753307817,
//			 -0.01867317550268149);

//			Mat R = (Mat_<double>(3, 3) <<
//			0.9999698145104303, 3.974878365893637e-06, 0.007769816740176146,
//			 -3.390471048492443e-05, 0.9999925806915616, 0.003851936175643478,
//			 -0.00776974378253147, -0.003852083336451321, 0.9999623955607145);
//
//			Mat T = (Mat_<double>(3, 1) <<
//			498.2890078004688,
//			 0.3317087752736566,
//			 -6.137837861924672);



			//512, 70 deg, -0.05, 0.05
			Mat cam1 = (Mat_<double>(3, 3) <<
			355.632787696046, 0, 254.8861648370192,
			0, 355.2269749905191, 258.102966900518,
			0, 0, 1);

			Mat dist1 = (Mat_<double>(5, 1) <<
			0.02711950895400501,
			-0.227837274174492,
			0.0006652596614750893,
			-0.0005721827370480087,
			0.307580072157251);

			Mat cam2 = (Mat_<double>(3, 3) <<
			355.3297273311275, 0, 254.3225838301501,
			0, 354.8360415105653, 258.4734268638256,
			0, 0, 1);

			Mat dist2 = (Mat_<double>(5, 1) <<
			0.038442541655977,
			-0.2972611233578806,
			0.0009819212628353676,
			-0.0006713304761550705,
			0.4203027526362795);
			
			Mat R = (Mat_<double>(3, 3) <<
			0.9999985973387854, 0.0001540282654807415, 0.001667811666592735,
			-0.0001522206102392302, 0.9999994009716373, -0.001083921239050295,
			-0.001667977622034611, 0.00108366584336636, 0.9999980217575394);
			 
			Mat T = (Mat_<double>(3, 1) <<
			100.1986525660281,
			0.03164248138029097,
			-1.316448461007167);

//			stereoCalibrator.makeCalibration(
//				camera1Calibrator.getSamplesPoints(),
//				camera2Calibrator.getSamplesPoints(),
//				cam1,
//				dist1,
//				cam2,
//				dist2
//			);
//			
//			Mat R = stereoCalibrator.getRotationMatrix();
//			Mat T = stereoCalibrator.getTranslationVector();
			
			outs.clear();
			outs << "--- stereo calibration: " <<
			"C1: " << cam1 << endl <<
			"D1: " << dist1 << endl <<
			"C2: " << cam2 << endl <<
			"D2: " << dist2 << endl <<
			"R: " << R << endl <<
			"T: " << T << endl;
			DebugLog(outs.str());
			
			disparityMapCalculator.set(
				cam1,
				cam2,
				dist1,
				dist2,
				R,
				T,
				cv::Size (image1.rows, image1.cols)
			);
			
			camera1Calibrator.clear();
			camera2Calibrator.clear();
			foundSamples = 0;
			
		}
		break;
		
		case 3:
		{
		
			disparityMapCalculator.compute(image1, image2, image1r, image2r, disparityMap);
			
			flip(image1r, l1, 1);
			flip(image2r, r1, 1);
			flip(l1, image1, 0);
			flip(r1, image2, 0);
			flip(disparityMap, l1, 1);
			flip(l1, disparityMap, 0);
			setTextureFromMat(image1, g_Cam1TexturePointer);
			setTextureFromMat(image2, g_Cam2TexturePointer);
			setTextureFromMat(disparityMap, g_Out1TexturePointer);
		}
		break;
		
		case 4: {
			//odometry
//			calcOdometry();
		}
		break;
		
		default:
		break;
	}
	
}

static void UNITY_INTERFACE_API OnRenderEvent(int eventID) {
	processRenderEvent(eventID);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API setBMParameters (
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
) {
	disparityMapCalculator.setBMParameters(preFilterSize, preFilterCap, blockSize, minDisparity, numDisparities, textureThreshold, uniquenessRatio, speckleWindowSize, speckleRange, disp12maxDiff);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API setSGBMParameters (
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
) {
	disparityMapCalculator.setSGBMParameters(preFilterCap, blockSize, minDisparity, numDisparities, uniquenessRatio, speckleWindowSize, speckleRange, disp12maxDiff, p1, p2);
}


// --------------------------------------------------------------------------
// UnitySetInterfaces

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);

static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;
static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;

extern "C" void	UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
	using namespace cv;

    DebugLog("--- UnityPluginLoad");
    s_UnityInterfaces = unityInterfaces;
    s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
    s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

	Size boardSize (9, 6);
	float squareSize = 167; //in pixels
	int texSize = 512;
	Size imageSize (texSize, texSize); //in pixels

	image1.create(imageSize, CV_8UC4);
	image2.create(imageSize, CV_8UC4);
	image1r.create(imageSize, CV_8UC4);
	image2r.create(imageSize, CV_8UC4);
	l1.create(imageSize, CV_8UC4);
	l2.create(imageSize, CV_8UC4);
	r1.create(imageSize, CV_8UC4);
	r2.create(imageSize, CV_8UC4);
	
	camera1Calibrator.set(boardSize, squareSize, imageSize);
	camera2Calibrator.set(boardSize, squareSize, imageSize);
	stereoCalibrator.set(boardSize, squareSize, imageSize);
	
    // Run OnGraphicsDeviceEvent(initialize) manually on plugin load
    OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
    s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

// --------------------------------------------------------------------------
// GraphicsDeviceEvent

// Actual setup/teardown functions defined below
#if SUPPORT_D3D9
static void DoEventGraphicsDeviceD3D9(UnityGfxDeviceEventType eventType);
#endif
#if SUPPORT_D3D11
static void DoEventGraphicsDeviceD3D11(UnityGfxDeviceEventType eventType);
#endif
#if SUPPORT_D3D12
static void DoEventGraphicsDeviceD3D12(UnityGfxDeviceEventType eventType);
#endif
#if SUPPORT_OPENGLES
static void DoEventGraphicsDeviceGLES(UnityGfxDeviceEventType eventType);
#endif

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
    UnityGfxRenderer currentDeviceType = s_DeviceType;
    
    switch (eventType)
    {
        case kUnityGfxDeviceEventInitialize:
        {
            DebugLog("OnGraphicsDeviceEvent(Initialize).\n");
            s_DeviceType = s_Graphics->GetRenderer();
            currentDeviceType = s_DeviceType;
            break;
        }
            
        case kUnityGfxDeviceEventShutdown:
        {
            DebugLog("OnGraphicsDeviceEvent(Shutdown).\n");
            s_DeviceType = kUnityGfxRendererNull;
            g_Cam1TexturePointer = NULL;
            break;
        }
            
        case kUnityGfxDeviceEventBeforeReset:
        {
            DebugLog("OnGraphicsDeviceEvent(BeforeReset).\n");
            break;
        }
            
        case kUnityGfxDeviceEventAfterReset:
        {
            DebugLog("OnGraphicsDeviceEvent(AfterReset).\n");
            break;
        }
    };
    
#if SUPPORT_D3D9
    if (currentDeviceType == kUnityGfxRendererD3D9)
        DoEventGraphicsDeviceD3D9(eventType);
#endif
    
#if SUPPORT_D3D11
    if (currentDeviceType == kUnityGfxRendererD3D11)
        DoEventGraphicsDeviceD3D11(eventType);
#endif
    
#if SUPPORT_D3D12
    if (currentDeviceType == kUnityGfxRendererD3D12)
        DoEventGraphicsDeviceD3D12(eventType);
#endif
    
#if SUPPORT_OPENGLES
    if (currentDeviceType == kUnityGfxRendererOpenGLES20 ||
        currentDeviceType == kUnityGfxRendererOpenGLES30)
        DoEventGraphicsDeviceGLES(eventType);
#endif
}

// --------------------------------------------------------------------------
// GetRenderEventFunc, an example function we export which is used to get a rendering event callback function.
extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc()
{
    return OnRenderEvent;
}
