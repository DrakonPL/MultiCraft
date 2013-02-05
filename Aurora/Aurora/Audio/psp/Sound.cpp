#include <Aurora/Audio/Sound.h>

#include <Aurora/Audio/psp/Audio.h>
#include <Aurora/Audio/psp/AudioMPEG.h>

namespace Aurora
{
	namespace Audio
	{
		PAudio _sound;

		Sound::Sound(std::string name,std::string filePath,bool streaming)
		{
			_name = name;
			_filePath = filePath;
			_stream = streaming;

			_looped = false;
			_paused = false;

			int stream = streaming == true ? 1 : 0;

			_sound = loadMp3((char*)filePath.c_str(),stream);
		}

		bool Sound::IsPlaying()
		{
			if (_sound)
			{
				int stat = getPlayAudio(_sound);
				if(stat == 1 )
					return true;
			}

			return false;
		}

		void Sound::SetLoop(bool state)
		{
			_looped = state;
			loopAudio(_sound);
		}

		void Sound::Play()
		{
			if (_sound)
			{
				playAudio(_sound);
			}
		}

		void Sound::Stop()
		{
			if (_sound)
			{
				stopAudio(_sound);
			}
		}

		void Sound::Pause()
		{
			if (_sound)
			{
				pauseAudio(_sound);
			}
		}
	}
}

