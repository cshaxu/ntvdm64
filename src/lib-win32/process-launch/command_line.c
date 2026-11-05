#include "command_line.h"
/* Locate the raw next argument without reserializing the target command line. */
PCWSTR win32_process_next_argument(PCWSTR p)
{
    BOOL quoted=FALSE;
    while(*p==L' ' || *p==L'\t')++p;
    while(*p){
        unsigned slashes=0;
        if(!quoted && (*p==L' ' || *p==L'\t'))break;
        while(*p==L'\\'){++slashes;++p;}
        if(*p==L'"'){if(!(slashes&1))quoted=!quoted;++p;}
        else if(*p && (quoted || (*p!=L' ' && *p!=L'\t')))++p;
    }
    while(*p==L' ' || *p==L'\t')++p;
    return p;
}
