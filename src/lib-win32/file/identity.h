#ifndef LIB_WIN32_FILE_IDENTITY_H
#define LIB_WIN32_FILE_IDENTITY_H
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Opens both paths for attributes, compares volume/file identities and closes
 * both handles. On failure *same is FALSE; paths and output are borrowed. */
DWORD win32_file_same(PCWSTR left,PCWSTR right,BOOL *same);
#ifdef __cplusplus
}
#endif
#endif
