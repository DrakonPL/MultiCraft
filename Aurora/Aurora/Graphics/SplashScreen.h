#ifndef SPLASHSCREEN_H_
#define SPLASHSCREEN_H_

#include <Aurora/Graphics/Sprite.h>
//#include <Aurora/System/Clock.h>

namespace Aurora
{
	namespace Graphics
	{
		class SplashScreen
		{
		private:

			float _finishTime;
			float _timer;
			float _dt;
			Sprite* _sprite;
			//System::Clock _mTimer;

		public:

			SplashScreen(const char* filename,int startW,int startH,int endW,int endH,float splashTime);
			~SplashScreen();

			void ShowSplash();
		};
	}
}

#endif