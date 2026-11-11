/* Test the modern NTDLL write extent, not a path-conversion replacement. */
#include <windows.h>
#include <winternl.h>
#include <stdio.h>
NTSYSAPI BOOLEAN NTAPI RtlDosPathNameToNtPathName_U(PCWSTR,PUNICODE_STRING,PWSTR *,PVOID);
NTSYSAPI NTSTATUS NTAPI NtCreateSection(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,PLARGE_INTEGER,ULONG,ULONG,HANDLE);
/* Original OpenNT public/sdk/inc/nturtl.h prefix. */
typedef struct { STRING RelativeName; HANDLE ContainingDirectory; } OLD_RELATIVE;
int main(void)
{
    struct { OLD_RELATIVE relative; ULONG tail; ULONG guard[4]; } value;
    UNICODE_STRING path;
    LPCWSTR names[] = {L"O:\\ntvdm64\\MEM.EXE",L"MEM.EXE"};
    unsigned i,j;
    if (sizeof(OLD_RELATIVE) != 12) return 2;
    for (i=0;i<2;i++) {
        ZeroMemory(&value,sizeof(value)); ZeroMemory(&path,sizeof(path));
        value.tail=0xa5a5a5a5;
        for(j=0;j<4;j++) value.guard[j]=0xa5a5a5a5;
        if(!RtlDosPathNameToNtPathName_U(names[i],&path,NULL,&value.relative)) return 3;
        /* The fourth DWORD is outside the original 12-byte structure. */
        printf("case=%u old-size=%u extra-dword=%08lx relative-length=%u directory=%p\n",
            i,(unsigned)sizeof(OLD_RELATIVE),value.tail,value.relative.RelativeName.Length,value.relative.ContainingDirectory);
        RtlFreeUnicodeString(&path);
        if(value.tail != 0) return 4;
        for(j=0;j<4;j++) if(value.guard[j] != 0xa5a5a5a5) return 5;
    }
    puts("PASS: modern path conversion writes 16 bytes; original 12-byte prefix requires ABI capacity binding");
    /* Exercise the actual native dependency of original GetBinaryTypeW,
     * not Kernel32's classification API or a replacement header parser.
     * Inputs are opened for execute only; no guest code is run here. */
    {
        LPCWSTR binaries[] = {L"MEM.EXE",L"COMMAND.COM",L"EDIT.COM",L"system32\\WRITE.EXE"};
        /* OpenNT public/sdk/inc/ntstatus.h: PROTECT, NOT_MZ, WIN_16. */
        ULONG expected[] = {0xc0000130,0xc000012f,0xc000012f,0xc0000131};
        for(i=0;i<4;i++) {
            HANDLE file,section=NULL;
            NTSTATUS status;
            file=CreateFileW(binaries[i],FILE_EXECUTE|SYNCHRONIZE,
                FILE_SHARE_READ|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,0,NULL);
            if(file==INVALID_HANDLE_VALUE) return 6;
            status=NtCreateSection(&section,SECTION_ALL_ACCESS,NULL,NULL,
                PAGE_EXECUTE,SEC_IMAGE,file);
            CloseHandle(file);
            printf("image=%ls native-section-status=%08lx section-present=%u\n",
                binaries[i],(ULONG)status,section!=NULL);
            if(section) CloseHandle(section);
            if((ULONG)status!=expected[i] || section!=NULL) return 7;
        }
        puts("PASS: all four native image outcomes match original classifier branches");
    }
    return 0;
}
