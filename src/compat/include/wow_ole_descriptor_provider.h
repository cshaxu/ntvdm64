#ifndef WOW_OLE_DESCRIPTOR_PROVIDER_H
#define WOW_OLE_DESCRIPTOR_PROVIDER_H
#include <windows.h>
typedef HGLOBAL (WINAPI *wow_ole_descriptor_provider)(HANDLE, UINT);
BOOL WINAPI wow_ole_descriptor_bind(wow_ole_descriptor_provider);
#endif
