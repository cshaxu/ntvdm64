#include "remote.h"
#include <stdint.h>

DWORD win32_process_image_remote_header(HANDLE process,const void *base,IMAGE_NT_HEADERS *header)
{
    IMAGE_DOS_HEADER dos;SIZE_T count=0;
    if(!ReadProcessMemory(process,base,&dos,sizeof(dos),&count))return GetLastError();
    if(count!=sizeof(dos) || dos.e_magic!=IMAGE_DOS_SIGNATURE || dos.e_lfanew<64 || dos.e_lfanew>1024*1024)
        return ERROR_BAD_EXE_FORMAT;
    if((uintptr_t)base>UINTPTR_MAX-(uintptr_t)dos.e_lfanew)return ERROR_BAD_EXE_FORMAT;
    if(!ReadProcessMemory(process,(const BYTE *)base+dos.e_lfanew,header,sizeof(*header),&count))return GetLastError();
    if(count!=sizeof(*header) || header->Signature!=IMAGE_NT_SIGNATURE ||
       header->FileHeader.Machine!=(sizeof(void*)==8?IMAGE_FILE_MACHINE_AMD64:IMAGE_FILE_MACHINE_I386) ||
       header->FileHeader.SizeOfOptionalHeader<sizeof(IMAGE_OPTIONAL_HEADER) ||
       header->OptionalHeader.Magic!=(sizeof(void*)==8?IMAGE_NT_OPTIONAL_HDR64_MAGIC:IMAGE_NT_OPTIONAL_HDR32_MAGIC) ||
       header->OptionalHeader.SizeOfImage<(DWORD)dos.e_lfanew+sizeof(*header))return ERROR_BAD_EXE_FORMAT;
    return ERROR_SUCCESS;
}

BOOL win32_process_image_remote_executable(HANDLE process,const void *address,const void *base)
{
    MEMORY_BASIC_INFORMATION info;DWORD protection;
    if(!VirtualQueryEx(process,address,&info,sizeof(info)) || info.AllocationBase!=base ||
       info.State!=MEM_COMMIT || info.Type!=MEM_IMAGE || (info.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return FALSE;
    protection=info.Protect&0xff;
    return protection==PAGE_EXECUTE || protection==PAGE_EXECUTE_READ ||
           protection==PAGE_EXECUTE_READWRITE || protection==PAGE_EXECUTE_WRITECOPY;
}

BOOL win32_process_image_remote_range(DWORD rva,SIZE_T bytes,DWORD image_size)
{return rva<=image_size && bytes<=image_size-rva;}

DWORD win32_process_image_remote_read(HANDLE process,const BYTE *base,DWORD rva,void *value,SIZE_T bytes,DWORD image_size)
{
    SIZE_T count=0;
    if(!win32_process_image_remote_range(rva,bytes,image_size) || (uintptr_t)base>UINTPTR_MAX-rva)return ERROR_BAD_EXE_FORMAT;
    if(!ReadProcessMemory(process,base+rva,value,bytes,&count))return GetLastError();
    return count==bytes?ERROR_SUCCESS:ERROR_PARTIAL_COPY;
}

