#ifndef ZIPPACK_H 
#define ZIPPACK_H 

#include <Aurora/System/ZipFile.h>

#include <unzip.h>
#include <string>

namespace Aurora
{
	namespace System
	{
		class ZipPack
		{
		private:

			unzFile _zipFile;

			std::string _fileName;
			std::string _loadedFileName;

			bool _loaded;

		public:

			ZipPack(std::string filename);
			ZipPack(std::string filename,std::string loadedFileName);
			
			bool Open();
			void Close();

			void ListFiles();

			ZipFile* GetZipFile( std::string fileName, std::string loadName );
			bool FileExists(std::string fileName);
		};
	}
}

#endif