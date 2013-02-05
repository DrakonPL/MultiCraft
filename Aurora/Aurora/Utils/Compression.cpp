#include <Aurora/Utils/Compression.h>

#include <zlib.h>

namespace Aurora
{
	namespace Utils
	{
		int CompressString(char* src,int srcLen,char** destination, int* destLength)
		{
			//Define the source, destination, source length, and destination length
			int destLen=compressBound(srcLen);
			char *dest=new char[destLen];
			//Decompress the string in src and place it in dest
			int result=compress((unsigned char *)dest,(uLongf*)&destLen,(const unsigned char *)src,srcLen);
			//Return the results of the compression
			*destination = dest;
			*destLength = destLen;
			return(result);
		}

		int DecompressString(char* src, int srcLen, char** destination, int* destLen,int dupa)
		{
			//Define the source, destination, source length, and destination length
			char *dest=new char[dupa];
			//Decompress the string in src and place it in dest
			int result=uncompress((unsigned char *)dest,(uLongf*)destLen,(const unsigned char *)src,srcLen);
			//Return the results of the decompression
			*destination = dest;
			return(result);
		}
	}
}

