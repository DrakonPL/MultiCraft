#ifndef CRYPTO_H
#define CRYPTO_H

namespace Aurora
{
	namespace Utils
	{
		void aesEncrypt(unsigned char *key, 
			const char *inData, int inDataSize, 
			char **outData, int *outDataSize);

		void aesDecrypt(unsigned char *key, 
			const char *inData, int inDataSize, 
			char **outData, int *outDataSize);
	}

}


#endif
