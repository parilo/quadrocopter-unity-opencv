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
#include "QuadrocopterBrain.hpp"

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

using namespace std;

// Prints a string
void DebugLog (std::string str)
{
#if UNITY_WIN
    OutputDebugStringA (str.c_str ());
#else
    fprintf(stderr, "%s\n", str.c_str ());
#endif
}


QuadrocopterBrain quadrocopterBrain;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetQuadrocopterState(
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
) {

	Observation state (
		currentRotW,
		currentRotX,
		currentRotY,
		currentRotZ,
		
//		currentXVal,
//		currentYVal,
//		currentZVal,

		targetXVal,
		targetYVal,
		targetZVal,

		motor1powerVal,
		motor2powerVal,
		motor3powerVal,
		motor4powerVal
	);

	quadrocopterBrain.setState(state);
}

extern "C" long UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetQuadrocopterAction() {
	long action = quadrocopterBrain.getAction();
	cerr << "--- get action: " << action << endl;
	return action;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API QuadrocopterBrainAct() {
	quadrocopterBrain.act();
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API StoreQuadrocopterExperience (
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
) {

	Observation prevState (
		currentRotWPrev,
		currentRotXPrev,
		currentRotYPrev,
		currentRotZPrev,
		
//		0,0,0,

		targetXValPrev,
		targetYValPrev,
		targetZValPrev,

		motor1powerValPrev,
		motor2powerValPrev,
		motor3powerValPrev,
		motor4powerValPrev
	);
	
	Observation nextState (
		currentRotWNext,
		currentRotXNext,
		currentRotYNext,
		currentRotZNext,

//		0,0,0,

		targetXValNext,
		targetYValNext,
		targetZValNext,

		motor1powerValNext,
		motor2powerValNext,
		motor3powerValNext,
		motor4powerValNext
	);
	
	ExperienceItem expItem (
		prevState,
		nextState,
		reward,
		action
	);

	quadrocopterBrain.storeExperience(expItem);
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

void processRenderEvent (int eventID) {
	
	using namespace std;
	using namespace cv;
	
	stringstream outs;
	
	// Матрица OpenCV, в которую будет считана текстура
	
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
			
			setTextureFromMat(image1, g_Cam1TexturePointer);
			setTextureFromMat(image2, g_Cam2TexturePointer);
			
		break;
		
		case 2:{
		
			camera1Calibrator.makeCalibration();
			camera2Calibrator.makeCalibration();
			
			Mat cam1 = camera1Calibrator.getCameraMatrix();
			Mat dist1 = camera1Calibrator.getDistCoeffs();
			Mat cam2 = camera2Calibrator.getCameraMatrix();
			Mat dist2 = camera2Calibrator.getDistCoeffs();

//			//512, 80 deg
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
//
//			Mat R = (Mat_<double>(3, 3) <<
//			0.9999698145104303, 3.974878365893637e-06, 0.007769816740176146,
//			 -3.390471048492443e-05, 0.9999925806915616, 0.003851936175643478,
//			 -0.00776974378253147, -0.003852083336451321, 0.9999623955607145);
//
//			Mat T = (Mat_<double>(3, 1) <<
//			498.2890078004688,
//			 0.3317087752736566,
//			 -6.137837861924672);

			stereoCalibrator.makeCalibration(
				camera1Calibrator.getSamplesPoints(),
				camera2Calibrator.getSamplesPoints(),
				cam1,
				dist1,
				cam2,
				dist2
			);
			
			Mat R = stereoCalibrator.getRotationMatrix();
			Mat T = stereoCalibrator.getTranslationVector();
			
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
			
			setTextureFromMat(image1r, g_Cam1TexturePointer);
			setTextureFromMat(image2r, g_Cam2TexturePointer);
			setTextureFromMat(disparityMap, g_Out1TexturePointer);
		}
		break;
		
		default:
		break;
	}
	
}

static void UNITY_INTERFACE_API OnRenderEvent(int eventID) {
	cerr << "--- OnRenderEvent" << endl;
//	processRenderEvent(eventID);
	quadrocopterBrain.act();
//	cerr << "--- act action: " << quadrocopterBrain.getAction() << endl;
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
