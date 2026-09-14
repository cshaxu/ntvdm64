/* Test the modern NTDLL write extent, not a path-conversion replacement. */
#include <windows.h>
#include <winternl.h>
#include <stdio.h>
NTSYSAPI BOOLEAN NTAPI RtlDosPathNameToNtPathName_U(PCWSTR,PUNICODE_STRING,PWSTR *,PVOID);
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
        printf("case=%u old-size=%u extra-dword=%08lx relative-length=%u\n",
            i,(unsigned)sizeof(OLD_RELATIVE),value.tail,value.relative.RelativeName.Length);
        RtlFreeUnicodeString(&path);
        if(value.tail != 0) return 4;
        for(j=0;j<4;j++) if(value.guard[j] != 0xa5a5a5a5) return 5;
    }
    puts("PASS: modern path conversion writes 16 bytes; original 12-byte prefix requires ABI capacity binding");
    return 0;
}
