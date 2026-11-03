#include "windows.h"
#include "insignia.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#ifdef HUNTER
#line 7
#error DIVERGENCE MVDM-HOST-DIV-519: HUNTER platform branch is not selected; see mirror register.
#endif /* HUNTER */
#line 9

void OutputString(char *);

int _CRTAPI1 printf(const char *str, ...)
{
#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
    return(0);
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

static HANDLE pipe=NULL;

void OutputString(char *str)
{
#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#ifdef HUNTER
#line 66
#error DIVERGENCE MVDM-HOST-DIV-519: HUNTER platform branch is not selected; see mirror register.
#endif /* HUNTER */
#line 69
}

#define WACKY_INPUT	"[BOB&SIMON'SCHEESYINPUT]"
#define WACKY_INPUTLEN	0xff

#define INPUT_API_SIG	0xdefaced

VOID InputString(char *str, int len)
{
#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

int _CRTAPI2 fprintf(FILE *tf, const char *str, ...)
{
#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
    return(0);
}

char *nt_fgets(char *buffer, int len, void *input_stream)
{
    /* Get Line from debug terminal */
    buffer[0] = 0;
    InputString(buffer,len);

    return(buffer);
}

char *nt_gets(char *buffer)
{
    return(nt_fgets(buffer, 500, (void *) 0));
}

#ifndef HUNTER
char * _CRTAPI1 fgets(char *buffer, int len, FILE *input_stream)
{
    int blen;

    // If not processing call to STDIN pass on to standard library function
    if(input_stream != stdin)
    {
	char *ptr = buffer;
	int chr;

	while(--len && (chr = fgetc(input_stream)) != EOF)
	{
	    *ptr++ = (char) chr;
	    if(chr == '\n') break;
	}

	*ptr = (char) 0;
	return(chr == EOF ? NULL : buffer);

    }

    // clear buffer...
    for(blen = 0; blen < len; blen++)
	buffer[blen] = 0;
    nt_fgets(buffer, len, input_stream);
    blen = strlen(buffer);
    if (blen + 1 < len)
    {
	buffer[blen] = '\n';	/* fgets adds newline */
	buffer[blen+1] = '\0';
    }
    return(buffer);
}

char * _CRTAPI1 gets(char *buffer)
{
    return(nt_fgets(buffer, 500, (void *) 0));
}

int _CRTAPI1 puts(const char *buffer)
{
    OutputString((char *)buffer);
    return(1);
}

size_t _CRTAPI1 fwrite(const void *buf, size_t size, size_t len, FILE *stream)
{
    char    *tmp_buf;		// Screw the compiler into avoiding const chk

    tmp_buf = (char *)((DWORD)buf);

    tmp_buf[len] = 0;		// Bullshit write into a const ptr!
#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif  /* PROD */
    return(len);
}
#endif  /* HUNTER */
