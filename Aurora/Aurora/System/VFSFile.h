#ifndef VFSFILE_H_
#define VFSFILE_H_

#include <Aurora/System/File.h>

#define CVFMin(x, y)				(((x)<(y))?(x):(y))

namespace Aurora
{
	namespace System
	{
		class VFSPack;

		class VFSFile : public File
		{
		private:
			
			void* _data;

			int _dataSize;
			int _offset;

		protected:

			VFSFile();
			VFSFile(std::string fileName,std::string loadFileName);
			VFSFile(std::string fileName,std::string loadFileName,void* data, int dataSize);

		public:
			virtual ~VFSFile();

			bool Open();
			void Close();

			void Read(void *data,unsigned int size,int count);
			unsigned char* GetData(int &dataSize);

			void Rewind() { _offset = 0; }

			//void SaveToDisk(const char* filename);
			friend class VFSPack;
		};
	}
}

#endif

