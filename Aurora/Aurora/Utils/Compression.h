#ifndef COMPRESSION_H
#define COMPRESSION_H

namespace Aurora
{
	namespace Utils
	{
		int CompressString(char* src,int srcLen, char** destination, int* destLength);
		int DecompressString(char* src, int srcLen, char** destination, int* destLen,int dupa);
	}

}


#endif
