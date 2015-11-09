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
		
		mat.create(texHeight, texWidth, CV_8UC4);
		
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

static void UNITY_INTERFACE_API OnRenderEvent(int eventID) {
	
	using namespace std;
	using namespace cv;
	
	stringstream outs;
	
	// Матрица OpenCV, в которую будет считана текстура
	cv::Mat image1;
	cv::Mat image2;
	
	cv::Mat* resultImage1;
	cv::Mat* resultImage2;
	
	getMatFromTexture(g_Cam1TexturePointer, image1);
	getMatFromTexture(g_Cam2TexturePointer, image2);

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
			
			resultImage1 = &image1;
			resultImage2 = &image2;
			
		break;
		
		case 2:{
		
			camera1Calibrator.makeCalibration();
			camera2Calibrator.makeCalibration();
			stereoCalibrator.makeCalibration(
				camera1Calibrator.getSamplesPoints(),
				camera2Calibrator.getSamplesPoints(),
				camera1Calibrator.getCameraMatrix(),
				camera1Calibrator.getDistCoeffs(),
				camera2Calibrator.getCameraMatrix(),
				camera2Calibrator.getDistCoeffs()
			);
			
			outs.clear();
			outs << "--- stereo calibration: R: " << stereoCalibrator.getRotationMatrix() << endl <<
			"T: " << stereoCalibrator.getTranslationVector() << endl <<
			"E: " << stereoCalibrator.getEssentialMatrix() << endl <<
			"F: " << stereoCalibrator.getFundamentalMatrix() << endl;
			DebugLog(outs.str());
			
			disparityMapCalculator.set(
				camera1Calibrator.getCameraMatrix(),
				camera2Calibrator.getCameraMatrix(),
				camera1Calibrator.getDistCoeffs(),
				camera2Calibrator.getDistCoeffs(),
				stereoCalibrator.getRotationMatrix(),
				stereoCalibrator.getTranslationVector(),
				cv::Size (image1.rows, image1.cols)
			);
			
			camera1Calibrator.clear();
			camera2Calibrator.clear();
			
			Mat image1r, image2r, disparityMap;
			
			disparityMapCalculator.compute(image1, image2, image1r, image2r, disparityMap);
			
			resultImage1 = &image1r;
			resultImage2 = &image2r;
			
			setTextureFromMat(disparityMap, g_Out1TexturePointer);
			
		}
		break;
		
		case 3:
		{
		}
		break;
		
		default:
		break;
	}
	
	setTextureFromMat(image1, g_Cam1TexturePointer);
	setTextureFromMat(image2, g_Cam2TexturePointer);
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
	Size imageSize (1024, 1024); //in pixels
	
	camera1Calibrator.set(boardSize, squareSize, imageSize);
	camera2Calibrator.set(boardSize, squareSize, imageSize);
	stereoCalibrator.set(boardSize, squareSize, imageSize);
	
//	Mat cameraMatrix = (Mat_<double>(3, 3) << 1539.913182592396, 0, 871.6435203036719,
//              0, 1539.913182592396, 704.4739699217903,
//              0, 0, 1);
//	
//	Mat distCoeffs = (Mat_<double>(5, 1) <<
//		0,
//		0,
//		0,
//		0,
//		0
//	);
//	Size imageSize (1024, 1024);
//	
//	Mat baseline = (Mat_<double>(3, 1) << 0.1, 0, 0);
//	
////	camera1Undistortion.set(cameraMatrix, distCoeffs, imageSize);
////	camera2Undistortion.set(cameraMatrix, distCoeffs, imageSize);
//	disparityMapCalculator.set(cameraMatrix, cameraMatrix, distCoeffs, distCoeffs, Mat::eye(3, 3, CV_64F), baseline, imageSize);


	
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
