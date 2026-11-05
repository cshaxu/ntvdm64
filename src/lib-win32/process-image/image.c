/* Adapted from fixed installer.cpp; see project source provenance. */
#include "image.h"
#include <string.h>
DWORD win32_process_image_machine(HANDLE child,WORD *machine)
{
    USHORT process=0,native=0;
    if(!child || !machine)return ERROR_INVALID_PARAMETER;
    if(!IsWow64Process2(child,&process,&native))return GetLastError();
    if(native!=IMAGE_FILE_MACHINE_AMD64)return ERROR_NOT_SUPPORTED;
    if(process==IMAGE_FILE_MACHINE_UNKNOWN)*machine=IMAGE_FILE_MACHINE_AMD64;
    else if(process==IMAGE_FILE_MACHINE_I386)*machine=IMAGE_FILE_MACHINE_I386;
    else return ERROR_NOT_SUPPORTED;
    return ERROR_SUCCESS;
}
DWORD win32_process_image_validate_ex(PCWSTR path,WORD machine,BOOL dll,WORD *subsystem_out)
{
    HANDLE file=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return GetLastError();
    IMAGE_DOS_HEADER dos;IMAGE_FILE_HEADER header;DWORD signature,read,error=ERROR_BAD_EXE_FORMAT;
    LARGE_INTEGER size,offset;WORD magic,subsystem;BYTE optional[sizeof(IMAGE_OPTIONAL_HEADER64)];
    if(!GetFileSizeEx(file,&size)){error=GetLastError();goto done;}
    if(!ReadFile(file,&dos,sizeof(dos),&read,NULL) || read!=sizeof(dos) || dos.e_magic!=IMAGE_DOS_SIGNATURE || dos.e_lfanew<(LONG)sizeof(dos))goto done;
    offset.QuadPart=dos.e_lfanew;
    if(offset.QuadPart>size.QuadPart-24 || !SetFilePointerEx(file,offset,NULL,FILE_BEGIN))goto done;
    if(!ReadFile(file,&signature,4,&read,NULL) || read!=4 || signature!=IMAGE_NT_SIGNATURE ||
       !ReadFile(file,&header,sizeof(header),&read,NULL) || read!=sizeof(header) || header.Machine!=machine ||
       !(header.Characteristics&IMAGE_FILE_EXECUTABLE_IMAGE) || !!(header.Characteristics&IMAGE_FILE_DLL)!=!!dll ||
       header.SizeOfOptionalHeader<70 || header.SizeOfOptionalHeader>sizeof(optional) ||
       size.QuadPart-offset.QuadPart-24<header.SizeOfOptionalHeader ||
       !ReadFile(file,optional,header.SizeOfOptionalHeader,&read,NULL) || read!=header.SizeOfOptionalHeader)goto done;
    memcpy(&magic,optional,2);memcpy(&subsystem,optional+68,2);
    if(magic!=(machine==IMAGE_FILE_MACHINE_I386?IMAGE_NT_OPTIONAL_HDR32_MAGIC:IMAGE_NT_OPTIONAL_HDR64_MAGIC) ||
       (subsystem!=IMAGE_SUBSYSTEM_WINDOWS_GUI && subsystem!=IMAGE_SUBSYSTEM_WINDOWS_CUI))goto done;
    if(subsystem_out)*subsystem_out=subsystem;
    error=ERROR_SUCCESS;
done:
    CloseHandle(file);return error;
}
DWORD win32_process_image_validate(PCWSTR path,WORD machine,BOOL dll)
{return win32_process_image_validate_ex(path,machine,dll,NULL);}

BOOL win32_process_image_is_legacy(PCWSTR image)
{
    HANDLE file=CreateFileW(image,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,
        NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return FALSE;
    IMAGE_DOS_HEADER dos;DWORD read,signature=0;BOOL legacy=FALSE;
    if(ReadFile(file,&dos,sizeof(dos),&read,NULL) && read==sizeof(dos) &&
            dos.e_magic==IMAGE_DOS_SIGNATURE){
        legacy=TRUE;
        if(dos.e_lfanew>=0){
            LARGE_INTEGER offset;offset.QuadPart=dos.e_lfanew;
            if(!SetFilePointerEx(file,offset,NULL,FILE_BEGIN) ||
                    !ReadFile(file,&signature,sizeof(signature),&read,NULL))legacy=FALSE;
            else if(read==sizeof(signature) && signature==IMAGE_NT_SIGNATURE)legacy=FALSE;
        }
    }
    CloseHandle(file);return legacy;
}
