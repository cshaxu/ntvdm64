#include "application_search.h"
#include <wchar.h>
#include <string.h>

#define SEARCH_PATH_CHARS 32768u

static BOOL has_extension(PCWSTR image)
{
    PCWSTR dot=wcsrchr(image,L'.'),slash=wcsrchr(image,L'\\');
    PCWSTR forward=wcsrchr(image,L'/');
    if(forward && (!slash || forward>slash))slash=forward;
    return dot && (!slash || dot>slash);
}

static DWORD search_directory(PCWSTR directory,PCWSTR image,PWSTR output,DWORD capacity)
{
    static PCWSTR const suffixes[]={L".com",L".exe",L".bat",L".pif"};
    WCHAR candidate[SEARCH_PATH_CHARS],absolute[SEARCH_PATH_CHARS];
    size_t dir=directory ? wcslen(directory) : 0u,length=wcslen(image),suffix;
    BOOL extension=has_extension(image);
    unsigned int index,count=extension ? 1u : ARRAYSIZE(suffixes);
    DWORD characters,attributes,error;
    for(index=0;index<count;++index) {
        PCWSTR append=extension ? L"" : suffixes[index];
        suffix=wcslen(append);
        if(dir+length+suffix+2u>ARRAYSIZE(candidate))return ERROR_FILENAME_EXCED_RANGE;
        if(dir){memcpy(candidate,directory,dir*sizeof(WCHAR));candidate[dir++]=L'\\';}
        memcpy(candidate+dir,image,length*sizeof(WCHAR));
        memcpy(candidate+dir+length,append,(suffix+1u)*sizeof(WCHAR));
        /* GetFullPathName preserves drive-relative X:foo and does not search. */
        characters=GetFullPathNameW(candidate,ARRAYSIZE(absolute),absolute,NULL);
        if(!characters)return GetLastError();
        if(characters>=ARRAYSIZE(absolute))return ERROR_FILENAME_EXCED_RANGE;
        attributes=GetFileAttributesW(absolute);
        if(attributes!=INVALID_FILE_ATTRIBUTES && !(attributes&FILE_ATTRIBUTE_DIRECTORY)) {
            if(characters>=capacity)return ERROR_INSUFFICIENT_BUFFER;
            memcpy(output,absolute,(characters+1u)*sizeof(WCHAR));
            return ERROR_SUCCESS;
        }
        if(attributes==INVALID_FILE_ATTRIBUTES) {
            error=GetLastError();
            if(error!=ERROR_FILE_NOT_FOUND && error!=ERROR_PATH_NOT_FOUND)return error;
        }
        if(dir)--dir;
    }
    return ERROR_FILE_NOT_FOUND;
}

DWORD common_resolve_application(PCWSTR image,PWSTR output,DWORD capacity)
{
    DWORD error,length,read;
    PWSTR path,entry,next;
    size_t chars;
    if(!output || !capacity)return ERROR_INVALID_PARAMETER;
    output[0]=0;
    if(!image || !*image)return ERROR_INVALID_PARAMETER;
    error=search_directory(NULL,image,output,capacity);
    if(error!=ERROR_FILE_NOT_FOUND || wcschr(image,L'\\') || wcschr(image,L'/') ||
        wcschr(image,L':'))return error;
    length=GetEnvironmentVariableW(L"PATH",NULL,0);
    if(!length)return ERROR_FILE_NOT_FOUND; /* Empty/unset: CWD only. */
    path=HeapAlloc(GetProcessHeap(),0,(size_t)length*sizeof(WCHAR));
    if(!path)return ERROR_NOT_ENOUGH_MEMORY;
    read=GetEnvironmentVariableW(L"PATH",path,length);
    if(!read || read>=length) {
        error=read ? ERROR_INSUFFICIENT_BUFFER : ERROR_FILE_NOT_FOUND;
        goto done;
    }
    entry=path;
    do {
        next=wcschr(entry,L';');
        if(next)*next=0;
        chars=wcslen(entry);
        if(chars>=2u && entry[0]==L'"' && entry[chars-1u]==L'"') {
            entry[chars-1u]=0;++entry;
        }
        error=search_directory(entry,image,output,capacity);
        if(error!=ERROR_FILE_NOT_FOUND)break;
        entry=next ? next+1 : NULL;
    }while(entry);
done:
    HeapFree(GetProcessHeap(),0,path);
    return error;
}
