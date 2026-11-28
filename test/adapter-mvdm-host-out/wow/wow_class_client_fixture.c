#include "wow_control_entry_fixture_provider.h"
#include "wow_class_client_fixture_provider.h"
#include "wow_class_client_bindings.h"
#include "wow_task_init_bindings.h"
#include <stdio.h>

wow_class_client_context *WINAPI wow_class_client_current(void);
/* Match XP WNDPROC_WOW: both tag bits identify an encoded guest procedure. */
#define FIXTURE_GUEST_PROC (WOW_WINDOW_PROC_TAG | 0x12340020u)
static BOOL cpd_available;
static unsigned cpd_calls;
static wow_class_callproc cpd={FIXTURE_GUEST_PROC};
static wow_class_callproc *WINAPI resolve_cpd(HANDLE handle)
{
    ++cpd_calls;
    return handle==(HANDLE)0xffff1234 && cpd_available ? &cpd : NULL;
}
static unsigned errors, version_calls;
static DWORD version = 0x030a;
static BOOL raise_version;
static HINSTANCE module = (HINSTANCE)0x23451234;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr,"FAIL line=%d error=%lu\n",__LINE__,GetLastError()); } } while (0)
static DWORD WINAPI get_version(HANDLE value)
{
    CHECK(value == module);
    ++version_calls;
    if (raise_version) RaiseException(0xe0420038,0,0,NULL);
    return version;
}
static DWORD WINAPI callback(HWND hwnd,UINT msg,UINT wp,LONG lp,DWORD proc,PVOID words)
{
    (void)hwnd;(void)msg;(void)wp;(void)lp;(void)proc;(void)words;
    CHECK(FALSE); return 0; /* This fixture does not create windows. */
}
extern LPWSTR WINAPI ClassNameToVersion(LPCWSTR, LPWSTR, LPWSTR *, BOOL);
extern UINT WINAPI wow_class_client_atom_name(ATOM, PUNICODE_STRING);
static void class_version_resolution(void)
{
    static const char manifest[] =
        "<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">"
        "<assemblyIdentity type=\"win32\" name=\"T485.ClassVersionFixture\" version=\"1.2.3.4\"/>"
        "<file name=\"fixture.exe\"><windowClass versioned=\"yes\">S15_VERSIONED_CLASS</windowClass></file>"
        "</assembly>";
    static const WCHAR name[] = L"S15_VERSIONED_CLASS";
    static const char ansi_name[] = "S15_VERSIONED_CLASS";
    WCHAR output[256], temp[MAX_PATH], path[MAX_PATH];
    ACTCTXW context = {sizeof(context)};
    HANDLE file, activation;
    DWORD written;
    ULONG_PTR cookie;
    LPWSTR resolved;
    UNICODE_STRING atom_name={0,sizeof(output),output};
    CHECK(wow_class_client_atom_name(1,&atom_name)==2 && !wcscmp(output,L"#1"));
    CHECK(wow_class_client_atom_name(0xbfff,&atom_name)==6 && !wcscmp(output,L"#49151"));
    atom_name.MaximumLength=2*sizeof(WCHAR);
    CHECK(wow_class_client_atom_name(0xbfff,&atom_name)==1 && !wcscmp(output,L"#"));
    CHECK(ClassNameToVersion(name,output,NULL,FALSE)==name);
    CHECK(ClassNameToVersion(NULL,output,NULL,FALSE)==NULL);
    if (!GetTempPathW(ARRAYSIZE(temp),temp) || !GetTempFileNameW(temp,L"cvr",0,path)) {
        CHECK(FALSE); return;
    }
    file=CreateFileW(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE){CHECK(FALSE);DeleteFileW(path);return;}
    CHECK(WriteFile(file,manifest,sizeof(manifest)-1,&written,NULL) && written==sizeof(manifest)-1);
    CloseHandle(file);
    context.lpSource=path;
    activation=CreateActCtxW(&context);
    CHECK(activation!=INVALID_HANDLE_VALUE);
    if(activation!=INVALID_HANDLE_VALUE) {
        if(ActivateActCtx(activation,&cookie)) {
            resolved=ClassNameToVersion(name,output,NULL,FALSE);
            CHECK(resolved==output && wcscmp(output,name)!=0 && wcsstr(output,name)!=NULL);
            resolved=ClassNameToVersion((LPCWSTR)ansi_name,output,NULL,TRUE);
            CHECK(resolved==output && strcmp((LPCSTR)output,ansi_name)!=0 &&
                strstr((LPCSTR)output,ansi_name)!=NULL);
            CHECK(DeactivateActCtx(0,cookie));
        } else CHECK(FALSE);
        ReleaseActCtx(activation);
    }
    CHECK(DeleteFileW(path));
    CHECK(ClassNameToVersion(name,output,NULL,FALSE)==name);
}
static void version_domains(void)
{
    wow_class_lookup_context classes={0};
    wow_task_client_info client_info={0,0x030a};
    wow_task_order_thread thread={0};
    wow_class_client_context context={&classes,get_version,callback,&thread,NULL};
    WNDCLASSA wc={0};
    WNDCLASSEXA native_class={0};
    DWORD words[2]={0};
    unsigned task_version,module_version,calls;
    ATOM atom;
    thread.pClientInfo=&client_info;
    wc.lpfnWndProc=(WNDPROC)FIXTURE_GUEST_PROC;wc.hInstance=module;
    wc.lpszClassName="S40_VERSION_DOMAINS";
    for(task_version=0;task_version<2;++task_version) {
        client_info.dwExpWinVer=task_version ? 0x0400 : 0x030a;
        for(module_version=0;module_version<2;++module_version) {
            version=module_version ? 0x0400 : 0x030a;
            wc.style=0x40000000;
            atom=wow_class_client_register_guest(&context,&wc,words);
            if(module_version) {
                CHECK(!atom && GetLastError()==ERROR_INVALID_PARAMETER);
            } else {
                CHECK(atom!=0);
                native_class.cbSize=sizeof(native_class);
                CHECK(GetClassInfoExA(module,wc.lpszClassName,&native_class));
                CHECK(!(native_class.style&0x40000000));
                CHECK(wow_class_client_unregister(&context,wc.lpszClassName,module));
            }
            CHECK(classes.pclsPrivateList==NULL && wow_class_client_current()==NULL);
        }
    }
    /* USER module validation uses the task version before querying module
     * version. A low module version cannot override this task-level rule. */
    version=0x030a;client_info.dwExpWinVer=0x0400;
    wc.style=0;wc.hInstance=GetModuleHandleA("user32.dll");
    calls=version_calls;
    CHECK(!wow_class_client_register_guest(&context,&wc,words));
    CHECK(GetLastError()==ERROR_INVALID_PARAMETER && calls==version_calls);
    CHECK(classes.pclsPrivateList==NULL && wow_class_client_current()==NULL);
}
static void procedure_encoding(void)
{
    /* Include the selector/tag boundary: 7fff encodes to ffff, whereas
     * ffff encodes to fffb through the original virtual-LDT-bit rule.
     * A CPD-shaped bit pattern alone cannot establish native ownership. */
    static const DWORD guest[] = {
        0x03b70b98u, 0x83b70b98u, 0x7fff0b98u, 0xffff0b98u
    };
    unsigned i;
    for (i=0; i<ARRAYSIZE(guest); ++i) {
        wow_class_lookup_context classes={0};
        wow_task_client_info client_info={0,0x030a};
        wow_task_order_thread thread={0};
        wow_class_client_context context={&classes,get_version,callback,&thread,NULL};
        WNDCLASSA wc={0};
        DWORD words[2]={guest[i],0};
        DWORD encoded=(guest[i] & ~0x00060000u) | ((guest[i] & WOW_WINDOW_PROC_TAG) >> 13) | WOW_WINDOW_PROC_TAG;
        DWORD decoded;
        ATOM atom;
        wow_class_words_binding *binding;
        wow_window_dispatch_target target;
        /* Exactly WU32RegisterClass's producer contract, followed by the
         * original W32Win16WndProcEx decoding contract. */
        thread.pClientInfo=&client_info;
        wc.lpfnWndProc=(WNDPROC)encoded;wc.hInstance=module;
        wc.lpszClassName="S2_WOW_PROC_ENCODING";
        atom=wow_class_client_register_guest(&context,&wc,words);
        CHECK(atom!=0);
        if (!atom) continue;
        binding=wow_class_words_acquire(&classes,atom,module);
        CHECK(binding && wow_class_words_target(binding,&target));
        if (binding) {
            CHECK(target.procedure==encoded);
            decoded=(target.procedure & ~WOW_WINDOW_PROC_TAG) |
                ((target.procedure & 0x00060000u) << 13) | 0x00060000u;
            CHECK(decoded==guest[i]);
            wow_class_words_release(binding);
        }
        CHECK(wow_class_client_unregister(&context,wc.lpszClassName,module));
    }
}

int __cdecl main(void)
{
    if (!wow_control_entry_bind(&fixture_control_entry_provider) ||
            !wow_window_access_bind(&wow_fixture_window_access_provider) ||
            !wow_class_client_bind(&wow_fixture_class_client_provider)) return 3;
    wow_class_lookup_context classes = {0};
    wow_task_client_info client_info={0,0x030a};
    wow_task_order_thread thread={0};
    wow_task_order_entry task={0};
    wow_class_client_context context = {&classes,get_version,callback,&thread,NULL};
    WNDCLASSA wc = {0};
    WNDCLASSEXA native_class = {0};
    DWORD words[2] = {0x12340020,0x45670030};
    ATOM atom;
    wow_class_words_binding *binding;
    wow_window_dispatch_target target;
    thread.pClientInfo=&client_info;
    wc.lpfnWndProc=(WNDPROC)FIXTURE_GUEST_PROC; wc.hInstance=module;
    wc.lpszClassName="S40_REAL_CLASS_CLIENT"; wc.lpszMenuName="MENU";
    CHECK(!wow_class_client_register_guest(NULL,&wc,words));
    context.thread=NULL;
    CHECK(!wow_class_client_register_guest(&context,&wc,words) && GetLastError()==ERROR_INVALID_STATE);
    context.thread=&thread;
    thread.TIF_flags=WOW_TASK_TIF_16BIT;thread.ptdb=&task;task.hTaskWow=0x4321;
    atom=wow_class_client_register_guest(&context,&wc,words);
    CHECK(atom && version_calls==2 && wow_class_client_current()==NULL);
    CHECK(classes.pclsPrivateList && classes.pclsPrivateList->hTaskWow==0x4321);
    binding=wow_class_words_acquire(&classes,atom,module);
    CHECK(binding != NULL);
    if (binding) {
        CHECK(wow_class_words_value(binding)[0]==words[0]);
        CHECK(wow_class_words_target(binding,&target) && target.procedure==FIXTURE_GUEST_PROC);
        wow_class_words_release(binding);
    }
    native_class.cbSize=sizeof(native_class);
    CHECK(GetClassInfoExA(module,wc.lpszClassName,&native_class));
    CHECK(native_class.lpszMenuName==NULL);
    CHECK(!wow_class_client_register_guest(&context,&wc,words));
    CHECK(wow_class_client_unregister(&context,wc.lpszClassName,module));
    CHECK(classes.pclsPrivateList==NULL && wow_class_client_current()==NULL);
    version=0x0300;
    CHECK(wow_class_client_register_guest(&context,&wc,words)==1);
    CHECK(wow_class_client_unregister(&context,wc.lpszClassName,module));
    version=0x0400; wc.style=0x40000000;
    CHECK(!wow_class_client_register_guest(&context,&wc,words));
    CHECK(GetLastError()==ERROR_INVALID_PARAMETER);
    wc.style=0; raise_version=TRUE;
    __try {
        (void)wow_class_client_register_guest(&context,&wc,words);
        CHECK(FALSE);
    } __except(GetExceptionCode()==0xe0420038 ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        CHECK(wow_class_client_current()==NULL);
    }
    CHECK(classes.pclsPrivateList==NULL);
    raise_version=FALSE;
    version_domains();
    version=0x030a;wc.style=0;wc.hInstance=module;thread.pClientInfo=&client_info;
    wc.lpfnWndProc=(WNDPROC)0xffff1234;
    atom=wow_class_client_register(&context,&wc,words);
    CHECK(atom && cpd_calls==0);
    if(atom) CHECK(wow_class_client_unregister(&context,wc.lpszClassName,module));
    context.resolve_callproc=resolve_cpd;
    atom=wow_class_client_register(&context,&wc,words);
    CHECK(atom && cpd_calls==1);
    if(atom) CHECK(wow_class_client_unregister(&context,wc.lpszClassName,module));
    CHECK(classes.pclsPrivateList==NULL && wow_class_client_current()==NULL);
    cpd_available=TRUE;
    atom=wow_class_client_register(&context,&wc,words);
    CHECK(atom && cpd_calls==2 && classes.pclsPrivateList->hTaskWow==0x4321);
    if(atom) {
        binding=wow_class_words_acquire(&classes,atom,module);
        CHECK(wow_class_words_target(binding,&target) && target.procedure==cpd.pfnClientPrevious);
        wow_class_words_release(binding);
        CHECK(wow_class_client_unregister(&context,wc.lpszClassName,module));
    }
    procedure_encoding();
    class_version_resolution();
    fprintf(stderr,"WOW_CLASS_CLIENT errors=%u version_calls=%u version_domains=5 server_owner=1 cpd=%u\n",errors,version_calls,cpd_calls);
    return errors != 0;
}
