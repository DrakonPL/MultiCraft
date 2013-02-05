#ifndef PSPAUDIOMANAGER_H_
#define PSPAUDIOMANAGER_H_

#include <Aurora/Audio/AudioManager.h>

namespace Aurora
{
	namespace Audio
	{
		class PspAudioManager: public AudioManager
		{
		public:
			PspAudioManager();

			bool Init();
			bool SupportedFormat(AudioFormat type);
		};
	}
}

#endif

