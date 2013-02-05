#include <Aurora/System/File.h>

namespace Aurora
{
	namespace System
	{
		File::File()
		{
			_fileName = "";
			_loadFileName = "";

			_loaded = false;
		}

		File::File(std::string fileName,std::string loadFileName)
		{
			_fileName = fileName;
			_loadFileName = loadFileName;

			_loaded = false;
		}
	}
}
