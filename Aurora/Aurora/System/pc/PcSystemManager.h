#ifndef PCSYSTEMManager_H
#define PCSYSTEMManager_H

#include <Aurora/System/SystemManager.h>
#include <SFML/Window.hpp>
#include <SFML/Window/Input.hpp>

using namespace Key;

namespace Aurora
{
	namespace System
	{
		class PcSystemManager : public SystemManager
		{
		private:

			bool _keyStates[Key::Count];
			bool _mouseButtonsStates[Mouse::ButtonCount];

		public:

			sf::Input* _input;
			sf::Window _window;

			PcSystemManager();

			void SetInput();

			void Update();

			bool keyPressed(Key::Code keyCode);
			bool keyHold(Key::Code keyCode);

			bool mouseButtonDown(Mouse::Button buttonNumber);
			bool mouseButtonPressed(Mouse::Button buttonNumber);

			int getMouseX();
			int getMouseY();

			void SetMousePosition(int x,int y);

			float getAnalogX();
			float getAlanogY();

			//Graphicy stuff
			void SetWindowSize(int width,int height);
		};

	}
}

#endif
