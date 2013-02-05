#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/System/SystemManager.h>
#include <Aurora/Utils/PlatformConfigurator.h>

#ifdef AURORA_PC
#include <Aurora/System/pc/PcSystemManager.h>
#endif

#ifdef AURORA_PSP
#include <Aurora/System/psp/PspSystemManager.h>
#endif

#ifdef AURORA_IOS
#include <Aurora/System/touch/TouchSystemManager.h>
#endif

#ifdef AURORA_ANDROID
#include <Aurora/System/touch/TouchSystemManager.h>
#endif

#ifdef AURORA_PS3
#include <Aurora/System/ps3/PS3SystemManager.h>
#endif

namespace Aurora
{
	namespace System
	{
		SystemManager* SystemManager::_systemManager = 0;

		SystemManager* SystemManager::Instance()
		{
			if(_systemManager == 0)
			{
#ifdef AURORA_PC
				_systemManager = new PcSystemManager();
#endif

#ifdef AURORA_PSP
				_systemManager = new PspSystemManager();
#endif
                
#ifdef AURORA_IOS
				_systemManager = new TouchSystemManager();
#endif

#ifdef AURORA_ANDROID
				_systemManager = new TouchSystemManager();
#endif

#ifdef AURORA_PS3
				_systemManager = new PS3SystemManager();
#endif
			}

			return _systemManager;
		}

		SystemManager::SystemManager()
		{
			_platformType = PCType;
		}

		bool SystemManager::platformUseKeyboard()
		{
			return _platformType == PCType;
		}

		bool SystemManager::platformUseMouse()
		{
			return _platformType == PCType;
		}
		
		bool SystemManager::platformUseJoypad()
		{
			return (_platformType == PSPType || _platformType == PS3Type);
		}

		PlatformType SystemManager::getPlatformType()
		{
			return _platformType;
		}

		void SystemManager::setPlatformType(PlatformType type)
		{
			_platformType = type;
		}

		void SystemManager::SetWindowSize(int width,int height)
		{
			Aurora::Graphics::RenderManager::Instance()->SetSize(width, height);
		}
	}
}
