#ifndef AudioManager_H
#define AudioManager_H

#include <Aurora/Audio/Sound.h>

#include <string>

namespace Aurora
{
	namespace Audio
	{
		enum AudioFormat
		{
			WAV,
			OGG,
			MP3
		};

		class AudioManager
		{
		protected:

			static AudioManager *_audioManager;

		public:

			AudioManager();

			static AudioManager* Instance();

		public:

			virtual bool Init() = 0;
			virtual bool SupportedFormat(AudioFormat type) = 0;
		};
	}
}

#endif