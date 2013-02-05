#ifndef ZIPFILE_H 
#define ZIPFILE_H 

#include <Aurora/System/File.h>

#include <unzip.h>
#include <string>

namespace Aurora
{
	namespace System
	{
		class ZipPack;

		class ZipFile : public File
		{
		private:

			unzFile _zipFile;

		protected:

			ZipFile();
			ZipFile(std::string fileName,std::string loadFileName);

		public:

			bool Open();
			void Close();

			void Read(void *data,unsigned int size,int count);
			unsigned char* GetData(int &dataSize);

			friend class ZipPack;
		};
	}
}


#endif
