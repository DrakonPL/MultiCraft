#include <Aurora/System/VFSPack.h>
#include <Aurora/Utils/md5.h>
#include <Aurora/Utils/Crypto.h>
#include <Aurora/Utils/Compression.h>

namespace Aurora
{
	namespace System
	{

		VFSPack::VFSPack(std::string fileName)
		{
			archiveFilename = fileName;
			_encryptionKey = NULL;
		}

		VFSPack::~VFSPack()
		{

		}

		void VFSPack::CreateNewPack(std::string filename)
		{
			archiveFilename = filename;
		}

		void VFSPack::AddFile(std::string filePath,bool compressed,bool encryption)
		{
			//add file info only if file really exists
			FILE *file = fopen(filePath.c_str(),"rb");
			if (file == NULL) return;

			long fileSize;

			//get file size
			fseek (file , 0 , SEEK_END);
			fileSize = ftell (file);
			fclose(file);

			//add file info
			PackFileInfo fileInfo;
			fileInfo.hash = hashString((unsigned char *)filePath.c_str());
			fileInfo.filename = filePath;

			fileInfo.compressed = compressed;
			fileInfo.encrypted = encryption;

			fileInfo.fileInPackPosition = 0;
			fileInfo.originalSize = fileSize;
			fileInfo.compressedSize = 0;
			fileInfo.encryptedSize = 0;			

			//add to collection
			_packedFiles.insert(std::pair<long,PackFileInfo>(fileInfo.hash,fileInfo));
		}


		bool VFSPack::SavePack()
		{
			//create file and write header
			FILE* file = fopen(archiveFilename.c_str(),"wb");

			int fileInPackPosition = 0;
			int fileInPackHeaderSize = (sizeof(int) * 4) + (sizeof(bool) * 2) + sizeof(long);
			int fileInPackFinalSize = 0;

			//write pack version
			int packVersion = 1;
			fwrite(&packVersion,sizeof(int),1,file);

			//write some options flags
			int flags = 0;
			fwrite(&flags,sizeof(int),1,file);

			//write files count
			int filesCount = _packedFiles.size();
			fwrite(&filesCount,sizeof(int),1,file);

			//pack header size
			fileInPackPosition = sizeof(int) * 3;

			std::map<long,PackFileInfo>::iterator it;

			//1. iterate on all files info and load files into the memory with compression,encryption
			for ( it= _packedFiles.begin() ; it != _packedFiles.end(); it++ )
			{
				//open each file listed and add to pack file
				FILE *fileToPack = fopen(it->second.filename.c_str(),"rb");

				char* buffer = (char*) malloc (sizeof(char)*it->second.originalSize);
				fread (buffer,1,it->second.originalSize,fileToPack);

				fclose(fileToPack);

				//if not compressed and not encrypted
				if (!it->second.compressed && !it->second.encrypted)
				{
					it->second.buffer = (char*)malloc(sizeof(char)*it->second.originalSize);
					memset(it->second.buffer,0,it->second.originalSize);
					memcpy(it->second.buffer,buffer,it->second.originalSize);
					
					//clean buffer
					free(buffer);
				}
				else if (it->second.compressed && !it->second.encrypted)
				{
					//compress only
					int compressSize = 0;

					Utils::CompressString(buffer,it->second.originalSize,&it->second.buffer,&compressSize);

					it->second.compressedSize = compressSize;
					
					//clean buffer
					free(buffer);
				}
				else if (!it->second.compressed && it->second.encrypted)
				{
					//encrypt only
					int encryptSize = 0;

					Utils::aesEncrypt(_encryptionKey,buffer,it->second.originalSize,&it->second.buffer,&encryptSize);

					it->second.encryptedSize = encryptSize;

					//clean buffer
					free(buffer);
				}
				else if (it->second.compressed && it->second.encrypted)
				{
					//compress and encrypt
					int compressSize = 0;
					int encryptSize = 0;
					char* tempBuffer;

					Utils::CompressString(buffer,it->second.originalSize,&tempBuffer,&compressSize);
					Utils::aesEncrypt(_encryptionKey,tempBuffer,compressSize,&it->second.buffer,&encryptSize);

					it->second.compressedSize = compressSize;
					it->second.encryptedSize = encryptSize;

					//clean buffers
					free(buffer);
					free(tempBuffer);
				}
			}

			fileInPackPosition += (fileInPackHeaderSize * _packedFiles.size());

			//2. save all file info headers
			for ( it= _packedFiles.begin() ; it != _packedFiles.end(); it++ )
			{
				fwrite(&it->second.hash,sizeof(long),1,file);

				fwrite(&it->second.encrypted,sizeof(bool),1,file);
				fwrite(&it->second.compressed,sizeof(bool),1,file);

				fwrite(&fileInPackPosition,sizeof(int),1,file);
				fwrite(&it->second.originalSize,sizeof(int),1,file);
				fwrite(&it->second.compressedSize,sizeof(int),1,file);
				fwrite(&it->second.encryptedSize,sizeof(int),1,file);

				//correct position in file
				if (!it->second.compressed && !it->second.encrypted)
				{
					fileInPackPosition += it->second.originalSize;
				}
				else if (it->second.compressed && !it->second.encrypted)
				{
					fileInPackPosition += it->second.compressedSize;
				}
				else if (!it->second.compressed && it->second.encrypted)
				{
					fileInPackPosition += it->second.encryptedSize;
				}
				else if (it->second.compressed && it->second.encrypted)
				{
					fileInPackPosition += it->second.encryptedSize;
				}
			}

			//3. save all files data
			for ( it= _packedFiles.begin() ; it != _packedFiles.end(); it++ )
			{
				int bufferSize = 0;

				if (!it->second.compressed && !it->second.encrypted)
				{
					bufferSize = it->second.originalSize;
				}
				else if (it->second.compressed && !it->second.encrypted)
				{
					bufferSize = it->second.compressedSize;
				}
				else if (!it->second.compressed && it->second.encrypted)
				{
					bufferSize = it->second.encryptedSize;
				}
				else if (it->second.compressed && it->second.encrypted)
				{
					bufferSize = it->second.encryptedSize;
				}

				fwrite(it->second.buffer,sizeof(char),bufferSize,file);
				free(it->second.buffer);
			}


			fclose (file);
			return true;
		}

		void VFSPack::SetEncryptKey(std::string key)
		{
			if (_encryptionKey != NULL)
			{
				free(_encryptionKey);
			}

			int test = key.length();
			char *test2 = (char *)key.c_str();

			_encryptionKey = new unsigned char[test];//(unsigned char *)malloc(test);

			memset(_encryptionKey,0,test);
			memcpy(_encryptionKey,test2, test); 

		}

		unsigned long VFSPack::hashString(unsigned char *str)
		{
			unsigned long hash = 0;
			int c;

			while (c = *str++)
				hash = c + (hash << 6) + (hash << 16) - hash;

			return hash;
		}

		bool VFSPack::LoadPack(std::string filename)
		{
			archiveFilename = filename;
			_packedFiles.clear();

			FILE *file = fopen(filename.c_str(),"rb");
			if (file == NULL) return false;
						
			int packVersion = 0;
			fread(&packVersion,sizeof(int),1,file);

			//write some options flags
			int flags = 0;
			fread(&flags,sizeof(int),1,file);

			//write files count
			int filesCount = 0;
			fread(&filesCount,sizeof(int),1,file);

			//load info of every file
			for(int i = 0;i < filesCount;i++)
			{
				PackFileInfo fileInfo;

				fread(&fileInfo.hash,sizeof(long),1,file);

				fread(&fileInfo.encrypted,sizeof(bool),1,file);
				fread(&fileInfo.compressed,sizeof(bool),1,file);

				fread(&fileInfo.fileInPackPosition,sizeof(int),1,file);
				fread(&fileInfo.originalSize,sizeof(int),1,file);
				fread(&fileInfo.compressedSize,sizeof(int),1,file);
				fread(&fileInfo.encryptedSize,sizeof(int),1,file);

				_packedFiles.insert(std::pair<long,PackFileInfo>(fileInfo.hash,fileInfo));
			}

			fclose(file);

			return true;
		}

		bool VFSPack::FileExists(std::string fileName)
		{
			long hash = hashString((unsigned char *)fileName.c_str());

			if(_packedFiles.find(hash) != _packedFiles.end())
			{
				return true;
			}

			return false;
		}

		VFSFile* VFSPack::GetFile(std::string filename,std::string loadFileName)
		{
			long hash = hashString((unsigned char *)loadFileName.c_str());

			if(_packedFiles.find(hash) != _packedFiles.end())
			{
				//file exist
				int bufferSize = 0;
				char *buffer;

				if (!_packedFiles[hash].compressed && !_packedFiles[hash].encrypted)
				{
					bufferSize = _packedFiles[hash].originalSize;
				}
				else if (_packedFiles[hash].compressed && !_packedFiles[hash].encrypted)
				{
					bufferSize = _packedFiles[hash].compressedSize;
				}
				else if (!_packedFiles[hash].compressed && _packedFiles[hash].encrypted)
				{
					bufferSize = _packedFiles[hash].encryptedSize;
				}
				else if (_packedFiles[hash].compressed && _packedFiles[hash].encrypted)
				{
					bufferSize = _packedFiles[hash].encryptedSize;
				}

				buffer = (char *)malloc(bufferSize);

				//load buffer from pack
				FILE *file = fopen(archiveFilename.c_str(),"rb");
			
				fseek(file, _packedFiles[hash].fileInPackPosition, 0);
				fread(buffer,1,bufferSize,file);

				fclose(file);

				//we have our file
				if (!_packedFiles[hash].compressed && !_packedFiles[hash].encrypted)
				{
					 return new VFSFile(filename,loadFileName,buffer, bufferSize);
				}
				else if (_packedFiles[hash].compressed && !_packedFiles[hash].encrypted)
				{
					char* decompressedBuffer;
					Utils::DecompressString(buffer,bufferSize,&decompressedBuffer,&_packedFiles[hash].originalSize,_packedFiles[hash].originalSize);

					free(buffer);

					return new VFSFile(filename,loadFileName,decompressedBuffer, _packedFiles[hash].originalSize);
				}
				else if (!_packedFiles[hash].compressed && _packedFiles[hash].encrypted)
				{
					char* decryptedBuffer;
					int decryptedSize;

					Utils::aesDecrypt(_encryptionKey,buffer,bufferSize,&decryptedBuffer,&decryptedSize);

					free(buffer);

					return new VFSFile(filename,loadFileName,decryptedBuffer,decryptedSize);
				}
				else if (_packedFiles[hash].compressed && _packedFiles[hash].encrypted)
				{
					char* decompressedBuffer;
					char* decryptedBuffer;
					int decryptedSize;

					Utils::aesDecrypt(_encryptionKey,buffer,bufferSize,&decryptedBuffer,&decryptedSize);
					Utils::DecompressString(decryptedBuffer,decryptedSize,&decompressedBuffer,&_packedFiles[hash].originalSize,_packedFiles[hash].originalSize);

					free(buffer);
					free(decryptedBuffer);

					return new VFSFile(filename,loadFileName,decompressedBuffer, _packedFiles[hash].originalSize);
				}
			}

			return new VFSFile(filename,loadFileName, 0 , 0);
		}

		void VFSPack::CalcMD5(std::string text, unsigned char *out)
		{
			md5_state_t state;
			md5_init(&state);
			md5_append(&state, (const md5_byte_t*)text.c_str(), (int)strlen(text.c_str()));
			md5_finish(&state, out);
		}
	}
}

