#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <Aurora/System/VFSPack.h>
#include <Aurora/System/ZipPack.h>

#include <Aurora/System/File.h>
#include <Aurora/System/ZipFile.h>
#include <Aurora/System/DiskFile.h>
#include <Aurora/System/VFSFile.h>


#include <string>

namespace Aurora
{
	namespace System
	{
		enum FileManagerType
		{
			NormalType,
			ZipType,
			PackType
		};

		class FileManager
		{
		private:

			FileManagerType _fileManagerType;

			std::string _mainDirPath;

			std::string _mainFileName;
			std::string _mainFilePassword;

			ZipPack* _zipPack;
			VFSPack* _vfsPack;

			static FileManager* _instance;

		private:

			File *_getDiskFile(std::string fileName);
			File *_getZipFile(std::string fileName);
			File *_getPackFile(std::string fileName);

		public:

			FileManager();
			FileManager(FileManagerType fileManagerType);
			FileManager(FileManagerType fileManagerType,std::string pathName);

			void SetMainDirPath(std::string pathName);
			void SetMainFile(std::string fileName);
			void SetMainFilePassword(std::string password);

			bool FileExists(std::string fileName);
			File *GetFile(std::string fileName);

			std::string GetMainDirPath() {return _mainDirPath; }

			static FileManager* Instance();
		};
	}
}

#endif
