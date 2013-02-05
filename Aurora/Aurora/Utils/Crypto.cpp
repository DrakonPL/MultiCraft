#include <Aurora/Utils/Crypto.h>
#include <Aurora/Utils/AES.h>

#include <assert.h>
#include <string.h>  /* memset(), memcpy() */
#include <string>

namespace Aurora
{
	namespace Utils
	{

		void aesEncrypt(unsigned char *key, const char *inData, int inDataSize, char **outData, int *outDataSize)
		{
			// allocate output data buffer:
			*outDataSize = (inDataSize+15)&(~15); // round up
			*outData = new char[*outDataSize];

			// if there's no key, we don't need to decrypt anything, simply copy:
			if (key==NULL)
			{
				memset(*outData,0,*outDataSize); // zero out the memory
				memcpy(*outData,inData,inDataSize); // copy input data to output buffer
				return;
			}

			// put input data in temporary buffer:
			unsigned char *buf = new unsigned char[*outDataSize];
			memset(buf,0,*outDataSize); // zero out the memory
			memcpy(buf,inData,inDataSize); // copy input data to temp buffer

			// encrypt temporary buffer into output data:
			AES crypt;
			crypt.SetParameters(192);
			crypt.StartEncryption(key); // set encryption key
			crypt.Encrypt(
				reinterpret_cast<const unsigned char*>(inData),
				reinterpret_cast<unsigned char*>(*outData), 
				*outDataSize/16); // encrypt

			// deallocate temporary buffer:
			delete buf;
		}

		void aesDecrypt(unsigned char *key, const char *inData, int inDataSize, char **outData, int *outDataSize)
		{
			// allocate output data buffer:
			*outDataSize = (inDataSize+15)&(~15); // round up
			assert(*outDataSize==inDataSize);
			*outData = new char[*outDataSize];

			// if there's no key, we don't need to encrypt anything, simply copy:
			if (key==NULL)
			{
				memset(*outData,0,*outDataSize); // zero out the memory
				memcpy(*outData,inData,inDataSize); // copy input data to output buffer
				return;
			}

			// put input data in temporary buffer:
			unsigned char *buf = new unsigned char[*outDataSize];
			memset(buf,0,*outDataSize); // zero out the memory
			memcpy(buf,inData,inDataSize); // copy input data to temp buffer

			// encrypt temporary buffer into output data:
			AES crypt;
			crypt.SetParameters(192);
			crypt.StartDecryption(key); // set encryption key
			crypt.Decrypt(
				reinterpret_cast<const unsigned char*>(inData),
				reinterpret_cast<unsigned char*>(*outData), 
				*outDataSize/16); // encrypt

			// deallocate temporary buffer:
			delete buf;
		}
	}
}
