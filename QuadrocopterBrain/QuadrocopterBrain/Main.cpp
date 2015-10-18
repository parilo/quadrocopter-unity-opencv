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

#include <opencv2/opencv.hpp>

#include "Main.h"

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
static void DebugLog (const char* str)
{
#if UNITY_WIN
    OutputDebugStringA (str);
#else
    fprintf(stderr, "%s", str);
#endif
}



//переменные для хранения идентификатора текстуры
static void* g_Cam1TexturePointer = NULL;
#ifdef SUPPORT_OPENGLES
static int   g_TexWidth  = 0;
static int   g_TexHeight = 0;
#endif

//Определение функции передачи идентификатора текстуры.
//Объявление находится в C# скрипте
#ifdef SUPPORT_OPENGLES
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetTextureOfCam1(void* texturePtr, int w, int h)
#else
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetTextureOfCam1(void* texturePtr)
#endif
{
    g_Cam1TexturePointer = texturePtr;
#ifdef SUPPORT_OPENGLES
    g_TexWidth = w;
    g_TexHeight = h;
#endif
}

static void UNITY_INTERFACE_API OnRenderEvent(int eventID) {
//если вы посмотрите в пример RenderingPluginExampleXX.zip
//то увидите здесь большее количество кода для разных платформ, я оставил здесь только OpenGL.
//Передача текстур на разных платформах осуществляется по разному, если у вас другая платформа
//обратитесь к коду примера
#if SUPPORT_OPENGL
    if (g_Cam1TexturePointer) {
	
        GLuint gltex = (GLuint)(size_t)(g_Cam1TexturePointer);
        glBindTexture (GL_TEXTURE_2D, gltex);
        int texWidth, texHeight;
        glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
        glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);
		
        // Матрица OpenCV, в которую будет считана текстура
        cv::Mat img (texHeight, texWidth, CV_8UC4);
		
        // Считывание текстуры в матрицу
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
		
        // В качестве тестовой обработки просто напишем на текстуре Cam1
        cv::putText(img, "Cam1", cv::Point2f(10,50), cv::FONT_HERSHEY_SIMPLEX, 2,  cv::Scalar(0, 0, 0, 255), 3);

        // Загружаем назад в память текстуру, чтобы отобразить ее в Unity.
        // Unity не позволяет обращаться к GUI функциям OpenCV, так что никакие imshow работать не будут
        // Поэтому весь вывод графической информации надо осуществлять передачей ее в Unity
        glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
    }
#endif
}



// --------------------------------------------------------------------------
// UnitySetInterfaces

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);

static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;
static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;

extern "C" void	UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
    DebugLog("--- UnityPluginLoad");
    s_UnityInterfaces = unityInterfaces;
    s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
    s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
    
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
