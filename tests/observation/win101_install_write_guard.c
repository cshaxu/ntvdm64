/* Test-only restricted installation guard. Never link into a product image. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "../../src/lib-win32/process-hook/startup_hooks.h"
#define CAP 32768u
#ifndef GUARD_TARGET1
#define GUARD_TARGET1 L"O:\\win101"
#define GUARD_TARGET2 L"O:\\win101t"
#define GUARD_MEDIA L"O:\\tmp\\w101m"
#endif
typedef NTSTATUS (NTAPI *CreateFn)(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,PIO_STATUS_BLOCK,PLARGE_INTEGER,ULONG,ULONG,ULONG,ULONG,PVOID,ULONG);
typedef NTSTATUS (NTAPI *OpenFn)(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,PIO_STATUS_BLOCK,ULONG,ULONG);
typedef NTSTATUS (NTAPI *WriteFn)(HANDLE,HANDLE,PVOID,PVOID,PIO_STATUS_BLOCK,PVOID,ULONG,PLARGE_INTEGER,PULONG);
typedef NTSTATUS (NTAPI *SetFn)(HANDLE,PIO_STATUS_BLOCK,PVOID,ULONG,FILE_INFORMATION_CLASS);
static CreateFn real_create; static OpenFn real_open; static WriteFn real_write; static SetFn real_set;
static WCHAR root[CAP],install_target[CAP],stage_target[CAP],media_target[CAP];
static HANDLE log_handle=INVALID_HANDLE_VALUE;
static __declspec(thread) BOOL nested;
static BOOL prefix(const WCHAR *s,const WCHAR *p) {size_t n=wcslen(p);return wcslen(s)>=n && !_wcsnicmp(s,p,n);}
static BOOL append(WCHAR *s,const WCHAR *part) {size_t a=wcslen(s),b=wcslen(part);return a<CAP && b<CAP-a && wcscat_s(s,CAP,part)==0;}
static void handle_name(HANDLE h,WCHAR *out) {DWORD n=GetFinalPathNameByHandleW(h,out,CAP,FILE_NAME_NORMALIZED);if(!n || n>=CAP)out[0]=0;}
static void normalize(const WCHAR *input,WCHAR *out) {
    WCHAR *p=HeapAlloc(GetProcessHeap(),0,CAP*sizeof(WCHAR));
    WCHAR *target=HeapAlloc(GetProcessHeap(),0,CAP*sizeof(WCHAR));
    out[0]=0;if(!p || !target)goto done;
    if(prefix(input,L"\\??\\") || prefix(input,L"\\\\?\\"))input+=4;
    if(wcsncpy_s(p,CAP,input,_TRUNCATE))goto done;
    for(int i=0;i<4 && wcslen(p)>1 && p[1]==L':';++i) {
        WCHAR drive[3]={p[0],L':',0};
        if(!QueryDosDeviceW(drive,target,CAP) || wcsncmp(target,L"\\??\\",4))break;
        if(wcslen(target)+wcslen(p+2)>=CAP)goto done;
        if(wcscat_s(target,CAP,p+2))goto done;
        memmove(p,target+4,(wcslen(target+4)+1)*sizeof(WCHAR));
    }
    {DWORD n=GetFullPathNameW(p,CAP,out,NULL);if(!n || n>=CAP)out[0]=0;}
 done: if(p)HeapFree(GetProcessHeap(),0,p);if(target)HeapFree(GetProcessHeap(),0,target);
}
static BOOL inside(const WCHAR *p,const WCHAR *base) {size_t n=wcslen(base),m=wcslen(p);return n && m>=n && !_wcsnicmp(p,base,n) && (m==n || p[n]==L'\\');}
static BOOL owned(const WCHAR *p) {return inside(p,root)||inside(p,install_target)||inside(p,stage_target)||inside(p,media_target);}
static BOOL device(const WCHAR *p) {return prefix(p,L"\\Device\\NamedPipe\\") || prefix(p,L"\\Device\\ConDrv\\") || prefix(p,L"\\Device\\Mailslot\\") || prefix(p,L"\\??\\PIPE\\") || !_wcsicmp(p,L"\\??\\CONIN$") || !_wcsicmp(p,L"\\??\\CONOUT$") || !_wcsicmp(p,L"\\??\\NUL") || !_wcsicmp(p,L"\\Device\\Null") || !_wcsicmp(p,L"\\Device\\Beep") || !_wcsicmp(p,L"\\Device\\KeyboardClass0");}
static void object_name(POBJECT_ATTRIBUTES a,WCHAR *out) {
    WCHAR *p=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,CAP*sizeof(WCHAR));out[0]=0;
    if(!p || !a || !a->ObjectName || !a->ObjectName->Buffer || (a->ObjectName->Length&1))goto done;
    {size_t n=a->ObjectName->Length/2;if(n>=CAP)goto done;memcpy(p,a->ObjectName->Buffer,n*sizeof(WCHAR));p[n]=0;}
    if(device(p)){wcscpy_s(out,CAP,p);goto done;}
    if(a->RootDirectory && p[0] && p[0]!=L'\\') {handle_name(a->RootDirectory,out);if(!out[0] || !append(out,L"\\") || !append(out,p)){out[0]=0;goto done;}wcscpy_s(p,CAP,out);}
    normalize(p,out);
 done:if(p)HeapFree(GetProcessHeap(),0,p);
}
static void record(const WCHAR *op,const WCHAR *p,BOOL allow) {
    WCHAR *line=HeapAlloc(GetProcessHeap(),0,CAP*sizeof(WCHAR));IO_STATUS_BLOCK io={0};int n;
    if(!line)return;n=swprintf_s(line,CAP,L"%s %s %s\r\n",allow?L"ALLOW":L"DENY",op,p);
    if(n>0)real_write(log_handle,NULL,NULL,NULL,&io,line,(ULONG)n*2,NULL,NULL);
    HeapFree(GetProcessHeap(),0,line);
}
static NTSTATUS refuse(PIO_STATUS_BLOCK io) {NTSTATUS s=(NTSTATUS)0xc0000022;if(io){io->Status=s;io->Information=0;}return s;}
static BOOL modifying(ACCESS_MASK a,ULONG disposition,ULONG options) {return (a&(GENERIC_WRITE|GENERIC_ALL|FILE_WRITE_DATA|FILE_APPEND_DATA|FILE_WRITE_EA|FILE_WRITE_ATTRIBUTES|DELETE|WRITE_DAC|WRITE_OWNER)) || disposition!=1 || (options&0x1000);}
static NTSTATUS NTAPI create_file(PHANDLE h,ACCESS_MASK a,POBJECT_ATTRIBUTES o,PIO_STATUS_BLOCK io,PLARGE_INTEGER z,ULONG attrs,ULONG share,ULONG d,ULONG options,PVOID ea,ULONG len) {
    WCHAR *p;BOOL allow;if(nested || !modifying(a,d,options))return real_create(h,a,o,io,z,attrs,share,d,options,ea,len);
    nested=TRUE;p=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,CAP*sizeof(WCHAR));if(!p){nested=FALSE;return refuse(io);}object_name(o,p);allow=device(p)||owned(p);
    if(!allow && d==3 && !modifying(a,1,options)){record(L"readonly-open-no-create",p,TRUE);HeapFree(GetProcessHeap(),0,p);nested=FALSE;return real_create(h,a,o,io,z,attrs,share,1,options,ea,len);}
    record(L"create",p,allow);HeapFree(GetProcessHeap(),0,p);nested=FALSE;
    return allow?real_create(h,a,o,io,z,attrs,share,d,options,ea,len):refuse(io);
}
static NTSTATUS NTAPI open_file(PHANDLE h,ACCESS_MASK a,POBJECT_ATTRIBUTES o,PIO_STATUS_BLOCK io,ULONG share,ULONG options) {
    WCHAR *p;BOOL allow;if(nested || !modifying(a,1,options))return real_open(h,a,o,io,share,options);
    nested=TRUE;p=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,CAP*sizeof(WCHAR));if(!p){nested=FALSE;return refuse(io);}object_name(o,p);allow=device(p)||owned(p);record(L"open",p,allow);HeapFree(GetProcessHeap(),0,p);nested=FALSE;
    return allow?real_open(h,a,o,io,share,options):refuse(io);
}
static void normalized_handle(HANDLE h,WCHAR *p) {WCHAR *raw=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,CAP*sizeof(WCHAR));p[0]=0;if(raw){handle_name(h,raw);normalize(raw,p);HeapFree(GetProcessHeap(),0,raw);}}
static NTSTATUS NTAPI write_file(HANDLE h,HANDLE e,PVOID apc,PVOID c,PIO_STATUS_BLOCK io,PVOID b,ULONG n,PLARGE_INTEGER off,PULONG key) {
    WCHAR *p;BOOL allow;if(nested || GetFileType(h)!=FILE_TYPE_DISK)return real_write(h,e,apc,c,io,b,n,off,key);
    nested=TRUE;p=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,CAP*sizeof(WCHAR));if(!p){nested=FALSE;return refuse(io);}normalized_handle(h,p);allow=owned(p);record(L"write",p,allow);HeapFree(GetProcessHeap(),0,p);nested=FALSE;
    return allow?real_write(h,e,apc,c,io,b,n,off,key):refuse(io);
}
static NTSTATUS NTAPI set_file(HANDLE h,PIO_STATUS_BLOCK io,PVOID b,ULONG n,FILE_INFORMATION_CLASS cls) {
    int k=(int)cls;WCHAR *p;BOOL allow;
    if(nested || GetFileType(h)!=FILE_TYPE_DISK || !(k==4||k==10||k==11||k==13||k==19||k==20||k==40||k==64||k==65||k==72))return real_set(h,io,b,n,cls);
    nested=TRUE;p=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,CAP*sizeof(WCHAR));if(!p){nested=FALSE;return refuse(io);}normalized_handle(h,p);allow=owned(p);
    if(allow && (k==10||k==11||k==65||k==72)) {
        struct Rename {ULONG flags;HANDLE parent;ULONG length;WCHAR name[1];};
        struct Rename *r=(struct Rename *)b;size_t head=offsetof(struct Rename,name);WCHAR *dest=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,CAP*sizeof(WCHAR));
        if(!dest || !r || n<head || r->length>n-head || (r->length&1) || !r->length || r->length/2>=CAP)allow=FALSE;
        else {
            memcpy(dest,r->name,r->length);dest[r->length/2]=0;
            if(dest[0]!=L'\\' && !(wcslen(dest)>1 && dest[1]==L':')) {
                WCHAR *base=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,CAP*sizeof(WCHAR));
                if(!base)allow=FALSE;else {if(r->parent)handle_name(r->parent,base);else {WCHAR *slash;wcscpy_s(base,CAP,p);slash=wcsrchr(base,L'\\');if(slash)*slash=0;else base[0]=0;}
                    if(!base[0] || !append(base,L"\\") || !append(base,dest))allow=FALSE;else wcscpy_s(dest,CAP,base);HeapFree(GetProcessHeap(),0,base);}
            }
            if(allow){normalize(dest,p);allow=owned(p);}
        }
        if(dest)HeapFree(GetProcessHeap(),0,dest);
    }
    record(L"set-info",p,allow);HeapFree(GetProcessHeap(),0,p);nested=FALSE;
    return allow?real_set(h,io,b,n,cls):refuse(io);
}
static void __cdecl initialize_guard(void) {
    WCHAR *env=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,CAP*sizeof(WCHAR));WCHAR *expected=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,CAP*sizeof(WCHAR));DWORD n;HMODULE nt;
    if(!env || !expected)ExitProcess(127);
    n=GetEnvironmentVariableW(L"WIN101_GUARD_ROOT",env,CAP);if(!n || n>=CAP)ExitProcess(127);normalize(env,root);
    while(wcslen(root)>3 && root[wcslen(root)-1]==L'\\')root[wcslen(root)-1]=0;
    normalize(L"O:\\win101-setup",expected);if(!root[0] || (!wcsstr(root,L"\\build\\") && _wcsicmp(root,expected)))ExitProcess(127);
    n=GetEnvironmentVariableW(L"WIN101_GUARD_TARGET",env,CAP);if(n){if(n>=CAP)ExitProcess(127);normalize(env,install_target);normalize(GUARD_TARGET1,expected);if(_wcsicmp(install_target,expected)){normalize(GUARD_TARGET2,expected);if(_wcsicmp(install_target,expected))ExitProcess(127);}}
    n=GetEnvironmentVariableW(L"WIN101_GUARD_STAGE",env,CAP);if(n){if(n>=CAP)ExitProcess(127);normalize(env,stage_target);if(!prefix(stage_target,L"O:\\tmp\\n") || wcslen(stage_target)!=11 || wcscpy_s(expected,CAP,stage_target) || !append(expected,L"\\.ntvdm64-test-stage.json") || GetFileAttributesW(expected)==INVALID_FILE_ATTRIBUTES)ExitProcess(127);}
    n=GetEnvironmentVariableW(L"WIN101_GUARD_MEDIA",env,CAP);if(n){if(n>=CAP)ExitProcess(127);normalize(env,media_target);normalize(GUARD_MEDIA,expected);if(_wcsicmp(media_target,expected))ExitProcess(127);}
    nt=GetModuleHandleW(L"ntdll.dll");real_create=(CreateFn)GetProcAddress(nt,"NtCreateFile");real_open=(OpenFn)GetProcAddress(nt,"NtOpenFile");real_write=(WriteFn)GetProcAddress(nt,"NtWriteFile");real_set=(SetFn)GetProcAddress(nt,"NtSetInformationFile");
    if(!real_create || !real_open || !real_write || !real_set || wcscpy_s(expected,CAP,root) || !append(expected,L"\\install-writes.txt"))ExitProcess(127);
    log_handle=CreateFileW(expected,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);if(log_handle==INVALID_HANDLE_VALUE)ExitProcess(127);
    HeapFree(GetProcessHeap(),0,env);HeapFree(GetProcessHeap(),0,expected);
    if(process_hook_startup_begin() || process_hook_startup_attach(&real_create,create_file) || process_hook_startup_attach(&real_open,open_file) || process_hook_startup_attach(&real_write,write_file) || process_hook_startup_attach(&real_set,set_file) || process_hook_startup_commit())ExitProcess(127);
}
#pragma section(".CRT$XCU",read)
__declspec(allocate(".CRT$XCU")) void (__cdecl *win101_guard_entry)(void)=initialize_guard;
