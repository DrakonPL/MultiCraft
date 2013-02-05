#ifndef VFSPACK_H_
#define VFSPACK_H_

#include <Aurora/System/VFSFile.h>

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <map>

namespace Aurora
{
	namespace System
	{
		class PackFileInfo
		{
		public:

			long hash;
			std::string filename;

			bool encrypted;
			bool compressed;

			char* buffer;

			int fileInPackPosition;

			int originalSize;
			int compressedSize;
			int encryptedSize;
		};

		class VFSPack
		{
		private:
			//all files in pack
			std::map<long,PackFileInfo> _packedFiles;
			
			unsigned char *_encryptionKey;
			std::string archiveFilename;

			void CalcMD5(std::string text, unsigned char *out);
			unsigned long hashString(unsigned char *str);

		public:
			VFSPack(){ _encryptionKey = NULL; }
			VFSPack(std::string fileName);
			virtual ~VFSPack();

			VFSFile* GetFile(std::string filename,std::string loadFileName);
			
			void CreateNewPack(std::string filename);
			void AddFile(std::string filePath,bool compressed,bool encryption);
			bool SavePack();
			bool LoadPack(std::string filename);

			void SetEncryptKey(std::string key);
			bool FileExists(std::string fileName);
		};

	}
}

#endif

