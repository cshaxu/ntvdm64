#ifndef LIB_WIN32_PROCESS_ATTRIBUTES_H
#define LIB_WIN32_PROCESS_ATTRIBUTES_H
#include <windows.h>
/* Initialize a zeroed state; the attribute value is borrowed until destroy.
 * Pass attribute 0 with NULL value and size 0 to leave the list empty.
 * After any attempted initialization the caller must destroy the state. */
typedef struct win32_process_attributes {
    LPPROC_THREAD_ATTRIBUTE_LIST list;
    BOOL initialized;
} win32_process_attributes;
DWORD win32_process_attributes_init(win32_process_attributes *state, DWORD_PTR attribute,
    void *value, SIZE_T size);
void win32_process_attributes_destroy(win32_process_attributes *state);
#endif
