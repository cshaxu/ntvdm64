/* static char SccsID[] = " @(#)copy_func.c	1.6 6/24/91 Copyright Insignia Solutions Ltd."; */
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>

#include "host_def.h"
#include "insignia.h"
#include "xt.h"

void
bwdcopy(
    char *src,
    char *dest,
    int len
    )
{
#if 0
#error DIVERGENCE MVDM-HOST-DIV-496: original literal #if 0 excludes this legacy branch; see mirror register.
#line 23
#else
      int i;

      i = len - 1;
      memcpy(dest - i , src - i, len);

#endif
}


void
bwd_dest_copy(
    char *src,
    char *dest,
    int len
    )
{
     int loop;

     for(loop = 0; loop < len; loop++)
         *dest-- = *src++;
}


void
memfill(
    unsigned char data,
    unsigned char *l_addr_in,
    unsigned char *h_addr_in
    )
{
     unsigned int len;

     len = h_addr_in + 1 - l_addr_in;
     memset(l_addr_in, data, len);

#if 0
#error DIVERGENCE MVDM-HOST-DIV-496: original literal #if 0 excludes this legacy branch; see mirror register.
#line 79
#endif
}

void
fwd_word_fill(
   unsigned short data,
   unsigned char *l_addr_in,
   int len
   )
{
        unsigned int data4, count;
	unsigned char *l_addr = l_addr_in;
	unsigned char *h_addr = l_addr_in+(len<<1);
	unsigned int *l_addr4,*h_addr4;

	l_addr4 = (unsigned int *)(((unsigned int)l_addr+3) & (~3));
	h_addr4 = (unsigned int *)(((unsigned int)h_addr) & (~3));
#ifdef	LITTLEND
        data = ((data >> 8) & 0xff) | ((data << 8) & 0xff00);
#endif
	if(h_addr4 > l_addr4)
	{
		switch((unsigned char *)l_addr4-l_addr)
		{
			case 3:
				data = (data>>8) | (data<<8);
				*l_addr++ = (unsigned char)(data);
			case 2:
				*(unsigned short *)l_addr = data;
				break;
			case 1:
				data = (data>>8) | (data<<8);
				*l_addr = (unsigned char)data;
		}
                data4 = data+(data<<16);

#if 0
#error Excluded unsupported original constant-false fill branch
                l_addr = (unsigned char *)l_addr4;
#else
                count = h_addr4 - l_addr4;
                RtlFillMemoryUlong(l_addr4, count << 2, data4);
                l_addr = (unsigned char *)l_addr4;
                l_addr += count << 2;
#endif

	}
	switch(h_addr-l_addr)
	{
		case 5:
/*
			data = (data>>8) | (data<<8);
*/
        		data = ((data >> 8) & 0xff) | ((data << 8) & 0xff00);
                        *l_addr++ = (unsigned char)data;
			*(unsigned int *)l_addr = data | (data<<16);
			break;
		case 7:
/*
			data = (data>>8) | (data<<8);
*/
        		data = ((data >> 8) & 0xff) | ((data << 8) & 0xff00);
                        *l_addr++ = (unsigned char)data;
		case 6:
                        *l_addr++ = (unsigned char)data;
			*l_addr++ = data >> 8;
		case 4:
                        *l_addr++ = (unsigned char)data;
			*l_addr++ = data >> 8;
                        *l_addr++ = (unsigned char)data;
			*l_addr++ = data >> 8;
			break;
		case 3:
/*
			data = (data>>8) | (data<<8);
*/
        		data = ((data >> 8) & 0xff) | ((data << 8) & 0xff00);
			*l_addr++ = (unsigned char)(data);
		case 2:
                        *l_addr++ = (unsigned char)data;
			*l_addr++ = data >> 8;
			break;
		case 1:
/*
			data = (data>>8) | (data<<8);
*/
        		data = ((data >> 8) & 0xff) | ((data << 8) & 0xff00);
			*l_addr = (unsigned char)data;
        }
}




void
memset4(
    unsigned int data,
    unsigned int *laddr,
    unsigned int count
    )
{
#if 0
#error DIVERGENCE MVDM-HOST-DIV-496: original literal #if 0 excludes this legacy branch; see mirror register.
#line 185
#else

   /*
    * The argument, count, as passed to this function is the number of 4-byte
    * items to fill, all RtlFill type functions need their count in bytes, so
    * multiply count by 4. (JJS - 6/9/95).
    */
   RtlFillMemoryUlong(laddr, count << 2, data);

#endif
}
