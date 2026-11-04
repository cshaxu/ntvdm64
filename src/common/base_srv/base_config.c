/* NT4 reads WOW registry templates. Standalone binds caller-owned immutable
 * package paths and preserves original argument construction in vdm.c.
 * ANSI conversion and file validation belong to run16 before this binding. */
#include <base_config.h>
#include <string.h>
#include <wchar.h>
static __declspec(thread) const OPENNT_BASE_VDM_CONFIG *boundConfig;

BOOL OpenNtBaseInitializeVdmConfig(OPENNT_BASE_VDM_CONFIG *config,
    PCSTR image, PCSTR kernelStem)
{
    size_t imageLength, kernelLength;
    if (!config) return FALSE;
    ZeroMemory(config,sizeof(*config));
    if (!image || !kernelStem) return FALSE;
    imageLength=strnlen(image,MAX_VDM_CFG_LINE);
    kernelLength=strnlen(kernelStem,MAX_VDM_CFG_LINE);
    if (!imageLength || !kernelLength ||
        imageLength+4+kernelLength>=MAX_VDM_CFG_LINE) return FALSE;
    if (strpbrk(image,"\"\r\n") || strpbrk(kernelStem,"\"\r\n")) return FALSE;
    memcpy(config->Dos,image,imageLength+1);
    memcpy(config->Wow,image,imageLength);
    memcpy(config->Wow+imageLength," -a ",4);
    memcpy(config->Wow+imageLength+4,kernelStem,kernelLength+1);
    config->ImageLength=(ULONG)imageLength;
    return TRUE;
}

const OPENNT_BASE_VDM_CONFIG *OpenNtBaseBindVdmConfig(const OPENNT_BASE_VDM_CONFIG *config)
{
    const OPENNT_BASE_VDM_CONFIG *previous=boundConfig;
    boundConfig=config;
    return previous;
}

BOOL BaseGetVDMKeyword(LPWSTR keyword, LPSTR output, LPDWORD capacity,
    LPWSTR sizeKeyword, LPDWORD vdmSize)
{
    PCSTR value;
    size_t length;
    if (!boundConfig || !boundConfig->ImageLength || !keyword || !output ||
        !capacity || !sizeKeyword || !vdmSize) return FALSE;
    if (!wcscmp(keyword,CMDLINE) && !wcscmp(sizeKeyword,DOSSIZE)) value=boundConfig->Dos;
    else if (!wcscmp(keyword,WOWCMDLINE) && !wcscmp(sizeKeyword,WOWSIZE)) value=boundConfig->Wow;
    else return FALSE;
    length=strlen(value);
    if (length>=*capacity) return FALSE;
    memcpy(output,value,length+1);
    *capacity=(DWORD)length;
    *vdmSize=16L*1024L*1024L; /* Original BaseGetVDMKeyword reservation. */
    return TRUE;
}

PCHAR OpenNtBaseVdmImageEnd(PCHAR line)
{
    if (!boundConfig || !boundConfig->ImageLength || !line ||
        strncmp(line,boundConfig->Dos,boundConfig->ImageLength)) return NULL;
    return line+boundConfig->ImageLength;
}
