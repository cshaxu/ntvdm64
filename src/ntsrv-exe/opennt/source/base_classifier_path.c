/* DIVERGENCE: original BaseClient vdm.c expects a relative path only when
 * accompanied by a usable borrowed directory handle (OpenNT base/ntdll/
 * curdir.c). Modern RtlDosPathNameToNtPathName_U returns a nonempty relative
 * name with a NULL directory in the observed x86 process and writes 16 bytes
 * rather than the original 12. Request its supported full-path result only;
 * publish an empty original relative result so unchanged BaseClient takes
 * its existing full-path branch. No parser, borrowed handle or new lifetime. */
#include <nt.h>
#include <base_classifier.h>
#undef RtlDosPathNameToNtPathName_U
BOOLEAN NTAPI OpenNtBaseDosPathNameToNtPathName_U(PCWSTR name,
    PUNICODE_STRING path, PWSTR *part, RTL_RELATIVE_NAME *relative)
{
    BOOLEAN result=RtlDosPathNameToNtPathName_U(name,path,part,NULL);
    if(result && relative) ZeroMemory(relative,sizeof(*relative));
    return result;
}
