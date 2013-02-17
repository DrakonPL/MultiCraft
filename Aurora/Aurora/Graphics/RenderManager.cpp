#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/Utils/PlatformConfigurator.h>

#ifdef AURORA_PC
#include <Aurora/Graphics/opengl/LegacyOpenGLRenderManager.h>
#endif

#ifdef AURORA_UNIX_MAC
#include <Aurora/Graphics/opengl/LegacyOpenGLRenderManager.h>
#endif

#ifdef AURORA_PSP
#include <Aurora/Graphics/gu/GuRenderManager.h>
#endif

#ifdef AURORA_IOS
#include <Aurora/Graphics/opengl/OpenGLES1RenderManager.h>
#endif

#ifdef AURORA_ANDROID
#include <Aurora/Graphics/opengl/OpenGLES1RenderManager.h>
#endif

#ifdef AURORA_PS3
#include <Aurora/Graphics/opengl/PSGLRenderManager.h>
#endif

//Uncomment this to use the new OpenGL renderer where appropriate
//#define OPENGL_NEW

namespace Aurora
{
	namespace Graphics
	{
		RenderManager* RenderManager::_renderManager = 0;

		RenderManager* RenderManager::Instance()
		{
			if(_renderManager == 0)
			{
#ifdef AURORA_PC
					_renderManager = new LegacyOpengGLRenderManager();
#endif

#ifdef AURORA_UNIX_MAC
					_renderManager = new LegacyOpengGLRenderManager();
#endif

#ifdef AURORA_PSP
				_renderManager = new GuRenderManager();
#endif
                
#ifdef AURORA_IOS
				_renderManager = new OpenGLES1RenderManager();
#endif

#ifdef AURORA_ANDROID
				_renderManager = new OpenGLES1RenderManager();
#endif

#ifdef AURORA_PS3
				_renderManager = new PSGLRenderManager();
#endif

			}

			return _renderManager;
		}

		RenderManager::RenderManager()
		{
			_currentTexture = -1;
			_currentCam = 0;
		}

		void RenderManager::SetPov(float pov)
		{
			_pov = pov;
		}

		void RenderManager::SetZminMax(float zmin,float zmax)
		{
			_zMin = zmin;
			_zMax = zmax;
		}

		void RenderManager::SetOrthoZminMax(float zmin,float zmax)
		{
			_zOtrhoMin = zmin;
			_zOtrhoMax = zmax;
		}

		void RenderManager::SetSize(int width,int height)
		{
			_width = width;
			_height = height;

			SetVievport(0,0,_width,_height);
		}

		void RenderManager::SetFulscreen(bool state)
		{
			_fullScreen = state;
		}

		void RenderManager::SetVSync(bool state)
		{
			_vSync = state;
		}

		void RenderManager::SetVievport(int x,int y,int width,int height)
		{
			_viewportX = x;
			_viewportY = y;
			_viewportWidth = width;
			_viewportHeight = height;
		}

		void RenderManager::SetCurrentCam(Camera *cam)
		{
			_currentCam = cam;
		}

		Camera* RenderManager::GetCurrentCam()
		{
			return _currentCam;
		}
	}
}
