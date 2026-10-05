/* Selected original classifier, not a suffix/MZ parser. This narrow owned
 * entry is linked by nthook32 without bringing in launcher/service state. */
#include <nt.h>
#include <base_classifier.h>
#include "nthook32-dll/hook.h"
#include "image_classification.h"
/* Classifier-only composition owns these original suffix descriptors; it
 * does not select run16/main.c or the service/worker implementation. */
UNICODE_STRING BaseDotComSuffixName={8,10,L".com"};
UNICODE_STRING BaseDotPifSuffixName={8,10,L".pif"};
UNICODE_STRING BaseDotExeSuffixName={8,10,L".exe"};
DWORD nthook_legacy_type(PCWSTR application,DWORD *type)
{
    DWORD candidate;
    if(!application || !type)return ERROR_INVALID_PARAMETER;
    if(!OpenNtBaseGetBinaryTypeW(application,&candidate))return GetLastError();
    if(candidate!=SCS_DOS_BINARY && candidate!=SCS_WOW_BINARY && candidate!=SCS_PIF_BINARY)
        return ERROR_NOT_SUPPORTED;
    *type=candidate;return ERROR_SUCCESS;
}
DWORD nthook_native_subsystem(HANDLE child,DWORD *subsystem)
{
    WCHAR image[MAX_PATH];DWORD count=MAX_PATH;
    if(!QueryFullProcessImageNameW(child,0,image,&count))return GetLastError();
    return run16_classify_native_image(image,subsystem);
}
