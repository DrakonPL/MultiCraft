#ifndef DISKFILE_H
#define DISKFILE_H

#include <Aurora/System/File.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>

namespace Aurora
{
	namespace System
	{
		class FileManager;

		class DiskFile : public File
		{
		private:

			FILE* _filBuf;

		protected:

			DiskFile();
			DiskFile(std::string fileName,std::string loadFileName);

		public:			
			
			bool Open();
			void Close();

			void Read(void *data,unsigned int size,int count);
			unsigned char* GetData(int &dataSize);

			friend class FileManager;
		};
	}
}

#endif