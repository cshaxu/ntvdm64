/* Link actual WU32 bodies and actual alias/page-domain services, not extracted
 * function copies. The low offsets deliberately take original IsThunkWindowProc's
 * non-thunk path without mapping or executing an invented guest selector. */
#include "precomp.h"
#include "wow_user_thunk_scope.h"
#include "wow_user_private_access.h"

#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr,"TYPED CALLER FAIL line=%d error=%lu\n",__LINE__,GetLastError()); } } while (0)

C_ASSERT(sizeof(GETWINDOWLONG16)==sizeof(GETCLASSLONG16));
C_ASSERT(FIELD_OFFSET(GETWINDOWLONG16,f1)==FIELD_OFFSET(GETCLASSLONG16,f1));
C_ASSERT(FIELD_OFFSET(GETWINDOWLONG16,f2)==FIELD_OFFSET(GETCLASSLONG16,f2));
C_ASSERT(sizeof(SETWINDOWLONG16)==sizeof(SETCLASSLONG16));
C_ASSERT(FIELD_OFFSET(SETWINDOWLONG16,f1)==FIELD_OFFSET(SETCLASSLONG16,f1));
C_ASSERT(FIELD_OFFSET(SETWINDOWLONG16,f2)==FIELD_OFFSET(SETCLASSLONG16,f2));
C_ASSERT(FIELD_OFFSET(SETWINDOWLONG16,f3)==FIELD_OFFSET(SETCLASSLONG16,f3));

static ULONG query(HWND window, BOOL is_class)
{
    BYTE storage[sizeof(VDMFRAME)+sizeof(GETWINDOWLONG16)]={0};
    PVDMFRAME frame=(PVDMFRAME)storage;
    PGETWINDOWLONG16 args=(PGETWINDOWLONG16)&frame->bArgs;
    args->f1=GETHWND16(window);
    args->f2=(SHORT)(is_class ? GCL_WNDPROC : GWL_WNDPROC);
    return wow_user_invoke_thunk(is_class ? WU32GetClassLong : WU32GetWindowLong,frame);
}
static ULONG update(HWND window, BOOL is_class, DWORD value)
{
    BYTE storage[sizeof(VDMFRAME)+sizeof(SETWINDOWLONG16)]={0};
    PVDMFRAME frame=(PVDMFRAME)storage;
    PSETWINDOWLONG16 args=(PSETWINDOWLONG16)&frame->bArgs;
    args->f1=GETHWND16(window);
    args->f2=(SHORT)(is_class ? GCL_WNDPROC : GWL_WNDPROC);
    args->f3=value;
    return wow_user_invoke_thunk(is_class ? WU32SetClassLong : WU32SetWindowLong,frame);
}
int __cdecl wow_typed_procedure_callers(HWND window)
{
    unsigned errors=0,kind;
    BOOL previous_guest;
    DWORD original,encoded,original_words;
    for(kind=0;kind<2;++kind) {
        original_words=kind ? GetClassLong(window,GCL_WOWvpfnWndProc) : GetWindowLong(window,GWL_WOWvpfnWndProc);
        original=wow_user_set_procedure(window,(BOOL)kind,0xffff0004,TRUE,&previous_guest);
        CHECK(original && previous_guest);
        CHECK(query(window,(BOOL)kind)==0x7fff0004);
        CHECK(update(window,(BOOL)kind,0x7fff0008)==0x7fff0004);
        CHECK(query(window,(BOOL)kind)==0x7fff0008);
        CHECK(update(window,(BOOL)kind,0x7fff0004)==0x7fff0008);
        encoded=wow_user_set_procedure(window,(BOOL)kind,original,TRUE,&previous_guest);
        CHECK(encoded==0xffff0004 && previous_guest);
        if(kind) SETWC(window,GCL_WOWvpfnWndProc,original_words);
        else SETWL(window,GWL_WOWvpfnWndProc,original_words);
    }
    fprintf(stderr,"WOW_TYPED_PROCEDURE_CALLERS errors=%u actual_wu32=4 guest_ffff=1 setters=4 getters=4\n",errors);
    return errors!=0;
}
