#ifndef BASE64_H
#define BASE64_H
//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------
int WINAPI base64_get_encode_length(int length);
int WINAPI base64_get_decode_length(int length);

int WINAPI base64_encode(const unsigned char *buffer0, int bufferlength0, char *buffer1, int bufferlength1);
int WINAPI base64_decode(const char *buffer0, int bufferlength0, unsigned char *buffer1, int bufferlength1);
//---------------------------------------------------------------------------
#endif
