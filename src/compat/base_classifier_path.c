/* DIVERGENCE: original BaseClient vdm.c expects a relative path only when
 * accompanied by a usable borrowed directory handle (OpenNT base/ntdll/
 * curdir.c). Modern RtlDosPathNameToNtPathName_U returns a nonempty relative
 * name with a NULL directory in the observed x86 process and writes 16 bytes
 * rather than the original 12. Request its supported full-path result only;
 * publish an empty original relative result so unchanged BaseClient takes
 * its existing full-path branch. No parser, borrowed handle or new lifetime. */
#include "include/base_classifier_path.h"

/* OpenNT 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c,
 * public/sdk/inc/nturtl.h:205-208. This is the caller's original output ABI,
 * not modern RTL_RELATIVE_NAME_U (which adds a directory reference). */
typedef struct _RTL_RELATIVE_NAME {
    STRING RelativeName;
    HANDLE ContainingDirectory;
} RTL_RELATIVE_NAME;

BOOLEAN NTAPI RtlDosPathNameToNtPathName_U(PCWSTR, PUNICODE_STRING,
    PWSTR *, PVOID);
BOOLEAN NTAPI OpenNtBaseDosPathNameToNtPathName_U(PCWSTR name,
    PUNICODE_STRING path, PWSTR *part, RTL_RELATIVE_NAME *relative)
{
    BOOLEAN result=RtlDosPathNameToNtPathName_U(name,path,part,NULL);
    if(result && relative) ZeroMemory(relative,sizeof(*relative));
    return result;
}
