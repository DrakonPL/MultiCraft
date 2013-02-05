#include <Aurora/Graphics/SplashScreen.h>
#include <Aurora/Graphics/RenderManager.h>

namespace Aurora
{
	namespace Graphics
	{
		SplashScreen::SplashScreen(const char* filename,int startW,int startH,int endW,int endH,float splashTime)
		{
			// load up the images
			_sprite = new Sprite(filename,startW,startH,endW,endH);
			_sprite->SetPosition(240,136);

			_finishTime = splashTime;
			_timer = 0.0f;
			_dt = 0.0f;
			//_mTimer.Reset();
		}

		SplashScreen::~SplashScreen()
		{
			_sprite->RemoveImage();
			delete _sprite;
		}

		void SplashScreen::ShowSplash()
		{
			_dt = 0.0f;//mTimer.getTime();
			while(_timer < _finishTime)
			{
				//_dt = _mTimer.getTime();

				RenderManager::Instance()->StartFrame();

				//implemet this

				RenderManager::Instance()->EndFrame();

				_timer+=_dt;
			}
			return;
		}

	}
}
