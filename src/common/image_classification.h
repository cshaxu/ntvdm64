/* Shared native image-section metadata. Original DOS/WOW classification and
 * shell parsing stay with their current callers; no PE parser or new search. */
#ifndef COMMON_IMAGE_CLASSIFICATION_H
#define COMMON_IMAGE_CLASSIFICATION_H
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif
DWORD common_classify_native_image(PCWSTR application,DWORD *machine,DWORD *subsystem,
    PWSTR final_path,DWORD final_capacity);
DWORD common_process_machine(HANDLE process,DWORD *machine);
/* Actual process image path in the native namespace, safe to reopen at either
 * caller width. Borrowed process handle; no search or redirection mutation. */
DWORD common_process_image_path(HANDLE process,PWSTR path,DWORD capacity);
DWORD common_classify_native_process(HANDLE process,DWORD *machine,DWORD *subsystem);
#ifdef __cplusplus
}
#endif
#endif
