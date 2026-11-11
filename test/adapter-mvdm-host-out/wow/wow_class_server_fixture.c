#include "wow_class_register_bindings.h"
#include <stdio.h>
static unsigned errors,registrations,resolutions;
static BOOL fail_registration,valid_cpd;
static wow_class_lookup_context classes;
static CLSMENUNAME menu;
static wow_class_callproc cpd;
#define CHECK(x) do { if(!(x)){++errors;printf("FAIL %d error=%lu\n",__LINE__,GetLastError());} } while(0)
static wow_class_lookup_entry *WINAPI publish(wow_class_register_context *c,
    WNDCLASSEXA *wc,WORD fnid,DWORD flags)
{
    UNICODE_STRING name;
    DWORD initial[2]={0,0};
    ATOM atom;
    (void)c;++registrations;CHECK(fnid==0 && flags==2);
    if(fail_registration)return NULL;
    name.Buffer=L"S40_CLASS_SERVER";name.Length=32;name.MaximumLength=34;
    atom=wow_class_words_register(&classes,wc,&name,initial,&menu);
    return atom ? ((wc->style & CS_GLOBALCLASS) ? classes.pclsPublicList : classes.pclsPrivateList) : NULL;
}
static wow_class_callproc *WINAPI resolve(wow_class_register_context *c,HANDLE h)
{
    (void)c;++resolutions;CHECK(h==(HANDLE)0xffff1234);
    return valid_cpd ? &cpd : NULL;
}
int __cdecl main(void)
{
    wow_task_order_thread thread={0};
    wow_task_order_entry task={0};
    wow_class_register_context context={0};
    WNDCLASSEXA wc={sizeof(wc)};
    DWORD words[2]={0x12345678,0x87654321};
    CLSMENUNAME returned;
    unsigned i;
    context.thread=&thread;context.register_class=publish;context.resolve_callproc=resolve;
    wc.hInstance=GetModuleHandleW(NULL);wc.lpszClassName="S40_CLASS_SERVER";
    menu.pszClientAnsiMenuName="client-menu";menu.pwszClientUnicodeMenuName=L"client-menu";
    task.hTaskWow=0x4321;cpd.pfnClientPrevious=(ULONG_PTR)DefWindowProcA;
    for(i=0;i!=5;++i) {
        ATOM atom;
        wc.lpfnWndProc=(i==4) ? (WNDPROC)0xffff1234 : DefWindowProcA;
        thread.TIF_flags=(i==1 || i==2 || i==4) ? WOW_TASK_TIF_16BIT : 0;
        thread.ptdb=(i==1 || i==3 || i==4) ? &task : NULL;
        valid_cpd=TRUE;
        atom=_RegisterClassEx(&wc,(PROC)DefWindowProcA,&menu,0,2,i==2 ? NULL : words,&context);
        CHECK(atom!=0);if(!atom)return 2;
        CHECK(classes.pclsPrivateList->hTaskWow==((i==1 || i==4) ? 0x4321u : 0u));
        CHECK(classes.pclsPrivateList->lpfnWorker==(PROC)DefWindowProcA);
        CHECK(classes.pclsPrivateList->adwWOW[0]==(i==2 ? 0u : words[0]));
        CHECK(classes.pclsPrivateList->adwWOW[1]==(i==2 ? 0u : words[1]));
        CHECK(classes.pclsPrivateList->cWndReferenceCount==0);
        CHECK(wc.lpfnWndProc==DefWindowProcA);
        CHECK(wow_class_words_unregister(&classes,atom,wc.hInstance,&returned));
        CHECK(returned.pszClientAnsiMenuName==menu.pszClientAnsiMenuName);
        CHECK(returned.pwszClientUnicodeMenuName==menu.pwszClientUnicodeMenuName);
        CHECK(classes.pclsPrivateList==NULL);
    }
    fail_registration=TRUE;valid_cpd=FALSE;wc.lpfnWndProc=(WNDPROC)0xffff1234;
    CHECK(!_RegisterClassEx(&wc,NULL,&menu,0,2,words,&context));
    CHECK(wc.lpfnWndProc==(WNDPROC)0xffff1234 && classes.pclsPrivateList==NULL);
    CHECK(registrations==6 && resolutions==2);
    /* Original private lookup compares the high word of packed modules;
     * public lookup ignores the requested module entirely. */
    fail_registration=FALSE;valid_cpd=FALSE;wc.lpfnWndProc=DefWindowProcA;
    wc.hInstance=(HINSTANCE)0x23451234;
    for(i=0;i!=2;++i) {
        ATOM atom;
        BOOL removed;
        HINSTANCE requested=i ? (HINSTANCE)0x45671234 : (HINSTANCE)0x23455678;
        wc.style=i ? CS_GLOBALCLASS : 0;
        atom=_RegisterClassEx(&wc,NULL,&menu,0,2,words,&context);
        CHECK(atom!=0);if(!atom)return 2;
        if(!i) {
            returned.pszClientAnsiMenuName=(LPSTR)1;
            returned.pwszClientUnicodeMenuName=(LPWSTR)2;
            returned.pusMenuName=(PUNICODE_STRING)3;
            CHECK(!wow_class_words_unregister(&classes,atom,(HINSTANCE)0x45671234,&returned));
            CHECK(GetLastError()==ERROR_CLASS_DOES_NOT_EXIST);
            CHECK(classes.pclsPrivateList && classes.pclsPrivateList->atomClassName==atom);
            CHECK(returned.pszClientAnsiMenuName==(LPSTR)1 &&
                returned.pwszClientUnicodeMenuName==(LPWSTR)2 && returned.pusMenuName==(PUNICODE_STRING)3);
        }
        removed=wow_class_words_unregister(&classes,atom,requested,&returned);
        printf("module_case=%u removed=%u failure_error=%lu\n",i,(unsigned)removed,removed ? 0ul : GetLastError());
        CHECK(removed);
        if(!removed)CHECK(wow_class_words_unregister(&classes,atom,wc.hInstance,&returned));
        CHECK(!classes.pclsPrivateList && !classes.pclsPublicList);
    }
    printf("WOW_CLASS_SERVER errors=%u registrations=%u cpd_resolutions=%u\n",errors,registrations,resolutions);
    return errors!=0;
}
