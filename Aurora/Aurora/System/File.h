#ifndef FILE_H
#define FILE_H

#include <string>

namespace Aurora
{
	namespace System
	{
		class FileManager;

		class File
		{
		protected:

			std::string _fileName;
			std::string _loadFileName;

			bool _loaded;

		protected:

			File();
			File(std::string fileName,std::string loadFileName);

		public:

			bool IsLoaded() { return _loaded; }

		public:

			virtual bool Open() = 0;
			virtual void Close() = 0;

			virtual void Read(void *data,unsigned int size,int count) = 0;
			virtual unsigned char* GetData(int &dataSize) = 0;
			
			friend class FileManager;
		};
	}
}

#endif
