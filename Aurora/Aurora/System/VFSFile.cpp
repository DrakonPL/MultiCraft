#include <Aurora/System/VFSFile.h>

#include <stdio.h>
#include <string.h>

namespace Aurora
{
	namespace System
	{
		VFSFile::VFSFile() : File()
		{
			_data = 0;
			_dataSize = 0;
			_offset = 0;
		}

		VFSFile::VFSFile(std::string fileName,std::string loadFileName) : File(fileName,loadFileName)
		{
			_data = 0;
			_dataSize = 0;
			_offset = 0;
		}

		VFSFile::VFSFile(std::string fileName,std::string loadFileName,void* data, int dataSize) : File(fileName,loadFileName)
		{
			_data = data;
			_dataSize = dataSize;
			_offset = 0;
		}

		VFSFile::~VFSFile()
		{
			if (_data != 0)
			{
				delete[] _data;
			}			
		}

		bool VFSFile::Open()
		{
			if (_data != 0)
			{
				_loaded = true;
			}

			return _loaded;
		}

		void VFSFile::Close()
		{

		}

		void VFSFile::Read(void *data,unsigned int size,int count)
		{
			int readSize = 0, realSize = size * count;

			readSize = CVFMin(realSize, _dataSize - _offset);

			if (readSize > 0)
			{
				memcpy(data, (char*)_dataSize + _offset, readSize);
				_offset += readSize;
			}
		}

		unsigned char* VFSFile::GetData(int &dataSize)
		{
			dataSize = _dataSize;
			return (unsigned char*)_data;
		}
	

		/*void VFSFile::SaveToDisk(const char* filename)
		{
			FILE* file = fopen(filename,"wb");

			fwrite(mData,1,mLength,file);

			fclose(file);
		}*/

	}
}

