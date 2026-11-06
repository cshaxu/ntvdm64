#include "file_handle.h"

DWORD win32_process_image_validate_dll_handle(HANDLE file)
{
    IMAGE_DOS_HEADER dos;IMAGE_NT_HEADERS nt;LARGE_INTEGER pos,size;DWORD read=0;
    BY_HANDLE_FILE_INFORMATION info;
    if(!GetFileInformationByHandle(file,&info))return GetLastError();
    if(info.dwFileAttributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT))return ERROR_ACCESS_DENIED;
    if(!GetFileSizeEx(file,&size))return GetLastError();
    if(!ReadFile(file,&dos,sizeof(dos),&read,NULL) || read!=sizeof(dos) ||
       dos.e_magic!=IMAGE_DOS_SIGNATURE || dos.e_lfanew<64 || dos.e_lfanew>1024*1024)return ERROR_BAD_EXE_FORMAT;
    pos.QuadPart=dos.e_lfanew;
    if(size.QuadPart<pos.QuadPart+(LONGLONG)sizeof(nt) || !SetFilePointerEx(file,pos,NULL,FILE_BEGIN) ||
       !ReadFile(file,&nt,sizeof(nt),&read,NULL) || read!=sizeof(nt))return ERROR_BAD_EXE_FORMAT;
    if(nt.Signature!=IMAGE_NT_SIGNATURE || !(nt.FileHeader.Characteristics&IMAGE_FILE_DLL) ||
       nt.FileHeader.Machine!=(sizeof(void*)==8?IMAGE_FILE_MACHINE_AMD64:IMAGE_FILE_MACHINE_I386) ||
       nt.FileHeader.SizeOfOptionalHeader<sizeof(IMAGE_OPTIONAL_HEADER) ||
       nt.OptionalHeader.Magic!=(sizeof(void*)==8?IMAGE_NT_OPTIONAL_HDR64_MAGIC:IMAGE_NT_OPTIONAL_HDR32_MAGIC))return ERROR_BAD_EXE_FORMAT;
    return ERROR_SUCCESS;
}

