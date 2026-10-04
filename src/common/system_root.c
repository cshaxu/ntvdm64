#include "system_root.h"
#include <string.h>
#include <wchar.h>

#define ROOT_PATH_CHARS 32768u

static DWORD image_directory(PWSTR image)
{
    PWSTR slash=wcsrchr(image,L'\\');
    if(!slash || slash==image) return ERROR_BAD_PATHNAME;
    /* Keep a drive-root trailing slash: C: is not C:\. */
    if(slash==image+2 && image[1]==L':') slash[1]=0;
    else *slash=0;
    return ERROR_SUCCESS;
}

DWORD common_system_root_w(PWSTR output,DWORD capacity)
{
    PWSTR image;
    DWORD length,error=ERROR_SUCCESS;
    if(!output || !capacity) return ERROR_INVALID_PARAMETER;
    output[0]=0;
    image=HeapAlloc(GetProcessHeap(),0,ROOT_PATH_CHARS*sizeof(WCHAR));
    if(!image) return ERROR_NOT_ENOUGH_MEMORY;
    length=GetModuleFileNameW(NULL,image,ROOT_PATH_CHARS);
    if(!length) error=GetLastError();
    else if(length>=ROOT_PATH_CHARS) error=ERROR_FILENAME_EXCED_RANGE;
    else error=image_directory(image);
    if(!error) {
        length=(DWORD)wcslen(image);
        if(length>=capacity) error=ERROR_INSUFFICIENT_BUFFER;
        else memcpy(output,image,(length+1u)*sizeof(WCHAR));
    }
    HeapFree(GetProcessHeap(),0,image);
    return error;
}

static BOOL relative_path_valid(PCWSTR relative)
{
    PCWSTR segment,next;
    size_t length;
    if(!relative || !*relative || *relative==L'\\' || *relative==L'/' ||
        wcschr(relative,L':')) return FALSE;
    segment=relative;
    for(;;) {
        next=segment;
        while(*next && *next!=L'\\' && *next!=L'/') ++next;
        length=(size_t)(next-segment);
        if(!length || (length==1 && segment[0]==L'.') ||
            (length==2 && segment[0]==L'.' && segment[1]==L'.')) return FALSE;
        if(!*next) return TRUE;
        segment=next+1;
    }
}

DWORD common_product_path_w(PCWSTR relative,PWSTR output,DWORD capacity)
{
    PWSTR root;
    DWORD error;
    size_t length,suffix,separator;
    if(!output || !capacity) return ERROR_INVALID_PARAMETER;
    output[0]=0;
    if(!relative_path_valid(relative)) return ERROR_INVALID_PARAMETER;
    root=HeapAlloc(GetProcessHeap(),0,ROOT_PATH_CHARS*sizeof(WCHAR));
    if(!root) return ERROR_NOT_ENOUGH_MEMORY;
    error=common_system_root_w(root,ROOT_PATH_CHARS);
    if(!error) {
        length=wcslen(root); suffix=wcslen(relative);
        separator=root[length-1]!=L'\\' ? 1u : 0u;
        if(suffix>=capacity || length+separator+suffix>=capacity)
            error=ERROR_INSUFFICIENT_BUFFER;
        else {
            memcpy(output,root,length*sizeof(WCHAR));
            if(separator) output[length++]=L'\\';
            memcpy(output+length,relative,(suffix+1u)*sizeof(WCHAR));
        }
    }
    HeapFree(GetProcessHeap(),0,root);
    return error;
}

static DWORD path_to_ansi(PCWSTR path,PSTR output,DWORD capacity)
{
    BOOL substituted=FALSE;
    BOOL utf8=GetACP()==CP_UTF8;
    DWORD flags=utf8 ? WC_ERR_INVALID_CHARS : WC_NO_BEST_FIT_CHARS;
    PBOOL used=utf8 ? NULL : &substituted;
    int required;
    required=WideCharToMultiByte(CP_ACP,flags,path,-1,NULL,0,NULL,used);
    if(!required) return GetLastError();
    if(substituted) return ERROR_NO_UNICODE_TRANSLATION;
    if((DWORD)required>capacity) return ERROR_INSUFFICIENT_BUFFER;
    if(!WideCharToMultiByte(CP_ACP,flags,path,-1,output,required,NULL,used)) {
        DWORD error=GetLastError();
        output[0]=0;
        return error;
    }
    if(substituted) {output[0]=0;return ERROR_NO_UNICODE_TRANSLATION;}
    return ERROR_SUCCESS;
}

static DWORD local_path_a(PCWSTR relative,PSTR output,DWORD capacity)
{
    PWSTR path;
    DWORD error;
    if(!output || !capacity) return ERROR_INVALID_PARAMETER;
    output[0]=0;
    path=HeapAlloc(GetProcessHeap(),0,ROOT_PATH_CHARS*sizeof(WCHAR));
    if(!path) return ERROR_NOT_ENOUGH_MEMORY;
    error=relative ? common_product_path_w(relative,path,ROOT_PATH_CHARS) :
        common_system_root_w(path,ROOT_PATH_CHARS);
    if(!error) error=path_to_ansi(path,output,capacity);
    HeapFree(GetProcessHeap(),0,path);
    return error;
}

DWORD common_system_root_a(PSTR output,DWORD capacity)
{
    return local_path_a(NULL,output,capacity);
}

DWORD common_product_path_a(PCWSTR relative,PSTR output,DWORD capacity)
{
    if(!relative) {
        if(output && capacity) output[0]=0;
        return ERROR_INVALID_PARAMETER;
    }
    return local_path_a(relative,output,capacity);
}
