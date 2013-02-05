#ifndef Sound_H
#define Sound_H

#include <string>

namespace Aurora
{
	namespace Audio
	{
		class Sound
		{
		private:

			bool _stream;
			bool _looped;
			bool _paused;
            bool _loaded;

			std::string _name;
			std::string _filePath;

		public:

			Sound(std::string name,std::string filePath,bool streaming);

			bool IsPlaying();
			bool IsLooped() { return _looped; }
			bool IsStreaming() { return _stream; }
            bool IsLoaded() { return _loaded; }

			std::string GetName(){return _name; }
			std::string GetFilePath(){ return _filePath; }

			void SetLoop(bool state);

			void Play();
			void Stop();
			void Pause();
		};
	}
}

#endif
