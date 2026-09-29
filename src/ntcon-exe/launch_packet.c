/* Project-owned native launch packet codec; shared by submitter and NTCON. */
#include "interface/native_launch.h"
#include <string.h>
#include <wchar.h>

DWORD run16_native_launch_pack(const run16_native_start *start,BYTE **output,DWORD *bytes)
{
    PCWSTR strings[4],entry;
    run16_native_launch_packet header={0};
    uint64_t total=sizeof(header);
    DWORD i;
    BYTE *cursor;
    if(!output || !bytes) return ERROR_INVALID_PARAMETER;
    *output=NULL;*bytes=0;
    if(!start || !start->command || !*start->command || !start->directory ||
        !*start->directory || !start->environment || start->console_mask>7) return ERROR_INVALID_PARAMETER;
    strings[0]=start->application ? start->application : L"";
    strings[1]=start->command;strings[2]=start->directory;strings[3]=start->environment;
    for(i=0;i<3;++i) {
        size_t count=wcslen(strings[i])+1;
        if(count>32767) return ERROR_FILENAME_EXCED_RANGE;
        header.characters[i]=(uint32_t)count;
    }
    entry=strings[3];
    if(!*entry) header.characters[3]=2;
    else {
        while(*entry) entry+=wcslen(entry)+1;
        if((uint64_t)(entry-strings[3])+1>MAXDWORD) return ERROR_ARITHMETIC_OVERFLOW;
        header.characters[3]=(uint32_t)(entry-strings[3])+1;
    }
    for(i=0;i<4;++i) total+=(uint64_t)header.characters[i]*sizeof(WCHAR);
    if(total>MAXDWORD) return ERROR_ARITHMETIC_OVERFLOW;
    header.console_mask=start->console_mask;
    for(i=0;i<3;++i) header.standard[i]=(uint64_t)(ULONG_PTR)start->standard[i];
    for(i=0;i<2;++i) header.capabilities[i]=(uint64_t)(ULONG_PTR)start->capabilities[i];
    *output=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(SIZE_T)total);
    if(!*output) return ERROR_NOT_ENOUGH_MEMORY;
    memcpy(*output,&header,sizeof(header));cursor=*output+sizeof(header);
    for(i=0;i<4;++i) {
        SIZE_T length=(SIZE_T)header.characters[i]*sizeof(WCHAR);
        /* An empty environment is explicitly a double NUL, even if the input
         * pointer designates only one readable terminating character. */
        if(i!=3 || *strings[3]) memcpy(cursor,strings[i],length);
        cursor+=length;
    }
    *bytes=(DWORD)total;
    return ERROR_SUCCESS;
}

DWORD run16_native_launch_unpack(BYTE *payload,DWORD bytes,run16_native_launch_packet *header,WCHAR **strings)
{
    uint64_t total=sizeof(*header);
    DWORD i,offset;
    BYTE *cursor;
    if(!payload || bytes<sizeof(*header)) return ERROR_INVALID_DATA;
    memcpy(header,payload,sizeof(*header));
    if(header->console_mask>7) return ERROR_INVALID_DATA;
    for(i=0;i<4;++i) {
        if(header->characters[i]<(i==3 ? 2u : 1u) || (i<3 && header->characters[i]>32767))
            return ERROR_INVALID_DATA;
        total+=(uint64_t)header->characters[i]*sizeof(WCHAR);
    }
    if(total!=bytes) return ERROR_INVALID_DATA;
    cursor=payload+sizeof(*header);
    for(i=0;i<4;++i) {
        strings[i]=(WCHAR *)cursor;
        if(strings[i][header->characters[i]-1]) return ERROR_INVALID_DATA;
        if(i<3 && wcslen(strings[i])+1!=header->characters[i]) return ERROR_INVALID_DATA;
        cursor+=(SIZE_T)header->characters[i]*sizeof(WCHAR);
    }
    if(!*strings[1] || !*strings[2] || strings[3][header->characters[3]-2]) return ERROR_INVALID_DATA;
    if(!*strings[3]) return header->characters[3]==2 ? ERROR_SUCCESS : ERROR_INVALID_DATA;
    offset=0;
    while(offset<header->characters[3]-1 && strings[3][offset])
        offset+=(DWORD)wcslen(strings[3]+offset)+1;
    return offset==header->characters[3]-1 ? ERROR_SUCCESS : ERROR_INVALID_DATA;
}
