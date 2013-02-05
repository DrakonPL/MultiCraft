#include <Aurora/Audio/AudioManager.h>
#include <Aurora/Utils/PlatformConfigurator.h>

#ifdef AURORA_PC
#include <Aurora/Audio/sfml/sfmlAudioManager.h>
#endif

#ifdef AURORA_PSP
#include <Aurora/Audio/psp/PspAudioManager.h>
#endif

#ifdef AURORA_IOS
#include <Aurora/Audio/caudio/cAudioManager.h>
#endif

namespace Aurora
{
	namespace Audio
	{
		AudioManager* AudioManager::_audioManager = 0;

		AudioManager::AudioManager()
		{


		}

		AudioManager* AudioManager::Instance()
		{
			if(_audioManager == 0)
			{
#ifdef AURORA_PC
				_audioManager = new sfmlAudioManager();
#endif

#ifdef AURORA_PSP
				_audioManager = new PspAudioManager();
#endif

#ifdef AURORA_IOS
				_audioManager = new cAudioManager();
#endif                

			}

			return _audioManager;
		}

	}
}

