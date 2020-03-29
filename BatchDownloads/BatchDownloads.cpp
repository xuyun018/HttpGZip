// BatchDownloads.cpp : 定义控制台应用程序的入口点。
//
//---------------------------------------------------------------------------
#define _WIN32_WINNT	0x0600
//---------------------------------------------------------------------------
#include "XYWinINet.h"

#include "zlib.h"

#include <stdio.h>
//---------------------------------------------------------------------------
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
//---------------------------------------------------------------------------
#define ZLIB_CHUNK															100000
//---------------------------------------------------------------------------
int inflate_read(char *source, int len, char **dest, int gzip)
{
	int ret;
	unsigned have;
	z_stream strm;
	unsigned char *out;
	char *p;
	int offset;
	int totalsize = 0;

	/*   allocate   inflate   state   */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;

	if (gzip)
	{
		ret = inflateInit2(&strm, 47);
	}
	else
	{
		ret = inflateInit(&strm);
	}

	if (ret != Z_OK)
	{
		return(totalsize);
	}

	out = (unsigned char *)MALLOC(ZLIB_CHUNK);

	strm.avail_in = len;
	strm.next_in = (unsigned char *)source;

	/*   run   inflate()   on   input   until   output   buffer   not   full   */
	do
	{
		offset = totalsize;

		strm.avail_out = ZLIB_CHUNK;
		strm.next_out = out;
		ret = inflate(&strm, Z_NO_FLUSH);
		//assert(ret != Z_STREAM_ERROR);     /*   state   not   clobbered   */ 
		switch (ret)
		{
		case Z_NEED_DICT:
			ret = Z_DATA_ERROR;           /*   and   fall   through   */
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			if (offset > 0)
			{
				FREE(*dest);
				*dest = NULL;
			}

			FREE(out);

			inflateEnd(&strm);
			return(0);
		}
		have = ZLIB_CHUNK - strm.avail_out;
		totalsize += have;
		p = (char *)MALLOC(totalsize);
		if (offset > 0)
		{
			memcpy(p, *dest, offset);

			FREE(*dest);
		}
		memcpy(p + offset, out, have);
		*dest = p;
	} while (strm.avail_out == 0);

	FREE(out);

	/*   clean   up   and   return   */
	(void)inflateEnd(&strm);

	//return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
	return(totalsize);
}

UINT WriteFileBuffer(const BYTE *buffer, UINT buffersize, const TCHAR *filename)
{
	HANDLE hfile;
	DWORD numberofbytes;
	UINT result = 0;

	hfile = CreateFile(filename, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile != INVALID_HANDLE_VALUE)
	{
		SetFilePointer(hfile, 0, NULL, FILE_END);

		if (WriteFile(hfile, buffer, buffersize, &numberofbytes, NULL) && numberofbytes > 0)
		{
			result += numberofbytes;
		}

		CloseHandle(hfile);
	}
	return(result);
}

int _tmain(int argc, _TCHAR* argv[])
{
	unsigned char *buffer;
	unsigned char *postbuffer = NULL;
	unsigned int buffersize;
	unsigned int postsize = 0;
	unsigned int l;
	DWORD receivestatus = 0;

	buffersize = 1024 * 1024;
	buffer = (unsigned char *)MALLOC(buffersize);
	if (buffer)
	{
		l = HttpConversation(NULL, L"Accept-Encoding: gzip", L"GET", L"http://www.hao123.com", NULL, NULL, NULL, &receivestatus, buffer, buffersize, postbuffer, postsize, 0);

		if (l > 0)
		{
			char *dest;
			int destlength = inflate_read((char *)buffer, l, &dest, 1);

			WriteFileBuffer((const BYTE *)dest, destlength, L"C:\\Logs\\testlog.txt");

			if (dest != NULL)
			{
				FREE(dest);
			}
		}
		printf("%s\r\n", buffer);

		FREE(buffer);
	}
	printf("%d, %d\r\n", l, receivestatus);

	getchar();

	return 0;
}

