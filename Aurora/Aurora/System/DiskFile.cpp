#include <Aurora/System/DiskFile.h>

namespace Aurora
{
	namespace System
	{
		DiskFile::DiskFile() : File()
		{
			_filBuf = 0;
		}

		DiskFile::DiskFile(std::string fileName,std::string loadFileName) : File(fileName,loadFileName)
		{
			_filBuf = 0;
		}

		bool DiskFile::Open()
		{
			_filBuf = fopen(_loadFileName.c_str(),"rb");

			if (_filBuf != 0)
			{
				return true;
			}

			return false;
		}

		void DiskFile::Close()
		{
			if (_filBuf != 0)
				fclose(_filBuf);
		}

		void DiskFile::Read(void *data,unsigned int size,int count)
		{
			fread(data,size,count,_filBuf);
		}

		unsigned char* DiskFile::GetData(int &dataSize)
		{
			if (_filBuf != 0)
			{
				fseek(_filBuf, 0, SEEK_END);
				dataSize = ftell(_filBuf);
				fseek(_filBuf, 0, SEEK_SET);

				unsigned char* buffer = new unsigned char[dataSize]; 

				fread(buffer, 1, dataSize, _filBuf);

				return buffer;
			}

			return 0;
		}
	}
}