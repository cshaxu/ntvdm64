#ifndef WOW_MENU_RESOURCE_BINDINGS_H
#define WOW_MENU_RESOURCE_BINDINGS_H
#include <windows.h>
#include "wow_menu_parser_bindings.h"

#include "wow_shared_user_abi.h"
#include "wow_user_registered_callbacks.h"

/* Use the one original WOW registration. Before registration these are
 * ordinary native resources, as in USER's initial resource call table. */
static __inline HANDLE wow_user_find_resource(HANDLE module, LPCSTR type,
    LPCSTR name, WORD language)
{
    const PFNWOWHANDLERSIN *calls = wow_user_registered_callbacks();
    return calls->pfnFindResourceEx ?
        calls->pfnFindResourceEx(module, type, name, language) :
        FindResourceExA(module, type, name, language);
}
static __inline HANDLE wow_user_load_resource(HANDLE module, HANDLE resource)
{
    const PFNWOWHANDLERSIN *calls = wow_user_registered_callbacks();
    return calls->pfnLoadResource ? calls->pfnLoadResource(module, resource) :
        LoadResource(module, resource);
}
static __inline LPSTR wow_user_lock_resource(HANDLE resource, HANDLE module)
{
    const PFNWOWHANDLERSIN *calls = wow_user_registered_callbacks();
    return calls->pfnLockResource ? calls->pfnLockResource(resource, module) :
        (LPSTR)LockResource(resource);
}
static __inline BOOL wow_user_unlock_resource(HANDLE resource, HANDLE module)
{
    const PFNWOWHANDLERSIN *calls = wow_user_registered_callbacks();
    return calls->pfnUnlockResource ? calls->pfnUnlockResource(resource, module) :
        UnlockResource(resource);
}
HANDLE WINAPI WOWFindResourceExWCover(HANDLE, LPCWSTR, LPCWSTR, WORD);
int WCSToMBEx(WORD, LPCWSTR, int, LPSTR *, int, BOOL);
HMENU WINAPI wow_user_load_menuA(HINSTANCE, LPCSTR);
HMENU WINAPI wow_user_load_menuW(HINSTANCE, LPCWSTR);

#define FINDRESOURCEEXA(module, name, type, language) \
    wow_user_find_resource(module, type, name, language)
#ifdef WOW_ORIGINAL_MENU_CLIENT
#define LoadMenuA wow_user_load_menuA
#define LoadMenuW wow_user_load_menuW
#define FINDRESOURCEA(module, name, type) wow_user_find_resource(module, type, name, 0)
#define FINDRESOURCEW(module, name, type) WOWFindResourceExWCover(module, (LPCWSTR)(type), name, 0)
#define LOADRESOURCE wow_user_load_resource
#define LOCKRESOURCE wow_user_lock_resource
#define UNLOCKRESOURCE wow_user_unlock_resource
#endif
#endif
