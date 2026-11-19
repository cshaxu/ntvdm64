#ifndef OPENNT_COMPAT_BASE_CLASSIFIER_PATH_H
#define OPENNT_COMPAT_BASE_CLASSIFIER_PATH_H
#include <windows.h>
#include <winternl.h>

struct _RTL_RELATIVE_NAME;
BOOLEAN NTAPI OpenNtBaseDosPathNameToNtPathName_U(PCWSTR, PUNICODE_STRING,
    PWSTR *, struct _RTL_RELATIVE_NAME *);
#endif
