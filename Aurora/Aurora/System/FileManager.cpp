#include <Aurora/System/FileManager.h>

namespace Aurora
{
	namespace System
	{
		FileManager* FileManager::_instance = 0;

		FileManager::FileManager()
		{
			_fileManagerType = NormalType;
			_mainDirPath = "";

			_instance = this;
		}

		FileManager::FileManager(FileManagerType fileManagerType)
		{
			_fileManagerType = fileManagerType;
			_mainDirPath = "";

			_instance = this;
		}

		FileManager::FileManager(FileManagerType fileManagerType,std::string pathName)
		{
			_fileManagerType = fileManagerType;
			_mainDirPath = pathName;

			_instance = this;
		}

		void FileManager::SetMainDirPath(std::string pathName)
		{
			_mainDirPath = pathName;
		}

		void FileManager::SetMainFile(std::string fileName)
		{
			_mainFileName = fileName;

			switch(_fileManagerType)
			{
				case ZipType:
				{
					_zipPack = new ZipPack(_mainFileName);
					_zipPack->Open();
				}
				break;

				case PackType:
				{
					_vfsPack = new VFSPack();
					_vfsPack->LoadPack(_mainFileName);
				}
				break;
			}
		}

		void FileManager::SetMainFilePassword(std::string password)
		{
			_mainFilePassword = password;

			switch(_fileManagerType)
			{
				case ZipType:

				break;

				case PackType:

				_vfsPack->SetEncryptKey(_mainFilePassword);

				break;
			}
		}

		bool FileManager::FileExists(std::string fileName)
		{
			std::string loadName = _mainDirPath + fileName;

			switch(_fileManagerType)
			{
				case NormalType :
				{
					FILE * fp = fopen(loadName.c_str(),"r");

					if (fp != 0)
					{
						fclose(fp);
						return true;
					}

					return false;
				}
				break;

				case ZipType:
				{
					return _zipPack->FileExists(loadName);
				}
				break;

				case PackType:
				{
					return _vfsPack->FileExists(loadName);
				}
				break;
			}

			return false;
		}

		File* FileManager::GetFile(std::string fileName)
		{
			switch(_fileManagerType)
			{
				case NormalType :
					return _getDiskFile(fileName);
				break;

				case ZipType:
					return _getZipFile(fileName);
				break;

				case PackType:
					return _getPackFile(fileName);
				break;
			}

			return 0;
		}

		FileManager* FileManager::Instance()
		{
			if (_instance == 0)
			{
				_instance = new FileManager();
			}

			return _instance;
		}

		File* FileManager::_getDiskFile(std::string fileName)
		{
			std::string loadName = _mainDirPath + fileName;

			if (FileExists(fileName))
			{
				DiskFile* newFile = new DiskFile(fileName,loadName);
				return newFile;
			}

			return 0;
		}

		File* FileManager::_getZipFile(std::string fileName)
		{
			std::string loadName = _mainDirPath + fileName;

			if (FileExists(fileName))
			{
				ZipFile* newFile = _zipPack->GetZipFile(fileName,loadName);
				return newFile;
			}

			return 0;
		}

		File* FileManager::_getPackFile(std::string fileName)
		{
			std::string loadName = _mainDirPath + fileName;

			if (FileExists(fileName))
			{
				VFSFile* newFile = _vfsPack->GetFile(fileName,loadName);
				return newFile;
			}

			return 0;
		}
	}
}

