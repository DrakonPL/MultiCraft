#include <Aurora/System/psp/PspSystemManager.h>

using namespace Key;

namespace Aurora
{
	namespace System
	{
			PspSystemManager::PspSystemManager() : SystemManager()
			{
				setPlatformType(PSPType);
			}
			
			PspCtrlButtons PspSystemManager::keyToPspButton(Key::Code keyCode)
			{
				PspCtrlButtons button;
				
				switch(keyCode)
				{
					case W:
					button = PSP_CTRL_UP;
					break;
					case S:
					button = PSP_CTRL_DOWN;
					break;
					case A:
					button = PSP_CTRL_LEFT;
					break;
					case D:
					button = PSP_CTRL_RIGHT;
					break;
					
					case Space:
					button = PSP_CTRL_SELECT;
					break;
					case Return:
					button = PSP_CTRL_START;
					break;
					
					case Up:
					button = PSP_CTRL_TRIANGLE;
					break;
					case Down:
					button = PSP_CTRL_CROSS;
					break;
					case Left:
					button = PSP_CTRL_SQUARE;
					break;
					case Right:
					button = PSP_CTRL_CIRCLE;
					break;
					
					case Q:
					button = PSP_CTRL_LTRIGGER ;
					break;
					case E:
					button = PSP_CTRL_RTRIGGER ;
					break;
					
					default:
					break;
				}
				
				return button;
			}

			void PspSystemManager::Update()
			{
				//new to old
				oldPadData = newPadData;
				//get input data
				sceCtrlReadBufferPositive(&newPadData, 1);
				//sceCtrlPeekBufferPositive(&newPadData, 1);
			}

			bool PspSystemManager::keyPressed(Key::Code keyCode)
			{
				PspCtrlButtons button = keyToPspButton(keyCode);
				
				if (newPadData.Buttons != oldPadData.Buttons)
				{
					if (newPadData.Buttons & button)
					{
						return true;
					}
				}
				return false;
			}

			bool PspSystemManager::keyHold(Key::Code keyCode)
			{
				PspCtrlButtons button = keyToPspButton(keyCode);
				
				if (newPadData.Buttons & button)
				{
					return true;
				}

				return false;
			}

			bool PspSystemManager::mouseButtonDown(Mouse::Button buttonNumber)
			{
				return false;
			}
			
			bool PspSystemManager::mouseButtonPressed(Mouse::Button buttonNumber)
			{
				return false;
			}

			int PspSystemManager::getMouseX()
			{
				return 0.0f;
			}

			int PspSystemManager::getMouseY()
			{
				return 0.0f;
			}
			
			void PspSystemManager::SetMousePosition(int x,int y)
			{
				
			}

			float PspSystemManager::getAnalogX()
			{
				return (((float)newPadData.Lx - 122.5f)/122.5f);
			}

			float PspSystemManager::getAlanogY()
			{
				return (((float)newPadData.Ly - 122.5f)/122.5f);
			}
	}
}
