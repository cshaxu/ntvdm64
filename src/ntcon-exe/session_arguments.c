#include "session_arguments.h"

static BOOL parse_hex(const WCHAR *text,uint64_t maximum,uint64_t *value)
{
    uint64_t result=0;
    unsigned digit;
    if(!text || !value || !*text)return FALSE;
    if(text[0]==L'0' && (text[1]==L'x' || text[1]==L'X'))text+=2;
    if(!*text)return FALSE;
    for(;*text;++text) {
        if(*text>=L'0' && *text<=L'9')digit=(unsigned)(*text-L'0');
        else if(*text>=L'a' && *text<=L'f')digit=(unsigned)(*text-L'a')+10;
        else if(*text>=L'A' && *text<=L'F')digit=(unsigned)(*text-L'A')+10;
        else return FALSE;
        if(result>(maximum-digit)/16)return FALSE;
        result=result*16+digit;
    }
    if(!result)return FALSE;
    *value=result;
    return TRUE;
}
BOOL frontend_session_resource(const WCHAR *text,UINT_PTR *value)
{
    uint64_t parsed;
    if(!value || !parse_hex(text,(uint64_t)UINTPTR_MAX,&parsed))return FALSE;
    *value=(UINT_PTR)parsed;
    return TRUE;
}
BOOL frontend_session_window(const WCHAR *text,uint64_t *value)
{return parse_hex(text,UINT64_MAX,value);}
