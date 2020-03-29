 #include "base64.h"
//---------------------------------------------------------------------------
const char base64_wordbook[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//---------------------------------------------------------------------------
int WINAPI base64_get_encode_length(int length)
{
	return((length+2)/3*4);
}
int WINAPI base64_get_decode_length(int length)
{
	return(length/4*3);
}

int WINAPI base64_encode(const unsigned char *buffer0, int bufferlength0, char *buffer1, int bufferlength1)
{
    int prepare;
    int offset0=0;
	int offset1;
	int i;
	char *p;

	p=buffer1;
	bufferlength1=base64_get_encode_length(bufferlength0);
    while(offset0<bufferlength0)
    {  
        offset1=0;
        prepare=0;
        while(offset1<3)
        {     
            if(offset0>=bufferlength0)
            {  
                break;
            } 
            //将data前8*3位移入prepare的低24位 
            prepare=((prepare<<8)|(buffer0[offset0]&0xFF));
            offset0++;
            offset1++;
        }
        //将有效数据移到以prepare的第24位起始位置
        prepare=(prepare<<((3-offset1)<<3));
        for(i=0;i<4;i++)
        {
            //最后一位或两位
            if(offset1<i)
            {
                *p='=';
            }
            else
            {
                //24位数据
				*p=base64_wordbook[(prepare>>((3-i)*6))&0x3F];
            }
			
            p++;
        }
    }

    return(bufferlength1);
}

int WINAPI base64_decode(const char *buffer0,int bufferlength0,unsigned char *buffer1,int bufferlength1)
{
	unsigned char *p;
	int count=0;
	int offset0=0;
	int offset1;
	int prepare;
    int i;

	if(buffer0[bufferlength0-1]=='=')
	{
		count++;
	}
	if(buffer0[bufferlength0-2]=='=')
	{
		count++;
	}  

	p=buffer1;
    while(offset0<bufferlength0-count)
    {
		offset1=0;
        prepare=0;
        while(offset1<4)
        {  
            if(offset0>=bufferlength0-count)
            {  
                break;
            }
			if(buffer0[offset0]>='A' && buffer0[offset0]<='Z')
			{
				prepare=(prepare<<6)|(buffer0[offset0]-'A');
			}
			else
			{
				if(buffer0[offset0]>='a' && buffer0[offset0]<='z')
				{
					prepare=(prepare<<6)|(buffer0[offset0]-'a'+26);
				}
				else
				{
					if(buffer0[offset0]>='0' && buffer0[offset0]<='9')
					{
						prepare=(prepare<<6)|(buffer0[offset0]-'0'+52);
					}
					else
					{
						if(buffer0[offset0]=='+')
						{
							prepare=(prepare<<6)|62;
						}
						else
						{
							prepare=(prepare<<6)|63;
						}
					}
				}
			}
			offset0++;
			offset1++;
        }
        prepare=prepare<<((4-offset1)*6);
        for(i=0;i<3;i++)
        {
			if(i==offset1)  
            {
                break;
            }
            *p++=(char)((prepare>>((2-i)*8))&0xFF);
        }
    }
    if(buffer0[bufferlength0-1]=='=')
    {
		p--;
	}
    return((int)(p-buffer1));
}
//---------------------------------------------------------------------------