#include <Aurora/Audio/psp/PspAudioManager.h>

#include <Aurora/Audio/psp/Audio.h>

namespace Aurora
{
	namespace Audio
	{

		PspAudioManager::PspAudioManager()
		{

		}

		bool PspAudioManager::Init()
		{
			audioInit(4);
		}

		bool PspAudioManager::SupportedFormat(AudioFormat type)
		{
			switch(type)
			{
				case MP3:
					return true;
				break;

				case WAV:
					return true;
				break;

				case OGG:
					return false;
				break;
			}

			return false;
		}
	}
}

