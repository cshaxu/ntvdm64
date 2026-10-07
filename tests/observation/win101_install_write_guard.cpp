// Test-only install guard. Linked into a private NTVDM copy, never published.
// Restricts disk mutations to the validated build-owned installation root.
// Device I/O and read-only file access retain their actual Windows results.
#include <windows.h>
#include <winternl.h>
#include <string>
#include "../../src/nthook32-dll/detours/detours.h"
typedef NTSTATUS (NTAPI *CreateFn)(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,PIO_STATUS_BLOCK,PLARGE_INTEGER,ULONG,ULONG,ULONG,ULONG,PVOID,ULONG);
typedef NTSTATUS (NTAPI *OpenFn)(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,PIO_STATUS_BLOCK,ULONG,ULONG);
typedef NTSTATUS (NTAPI *WriteFn)(HANDLE,HANDLE,PVOID,PVOID,PIO_STATUS_BLOCK,PVOID,ULONG,PLARGE_INTEGER,PULONG);
typedef NTSTATUS (NTAPI *SetFn)(HANDLE,PIO_STATUS_BLOCK,PVOID,ULONG,FILE_INFORMATION_CLASS);
static CreateFn real_create; static OpenFn real_open; static WriteFn real_write; static SetFn real_set;
static std::wstring root, install_target;
static HANDLE log_handle=INVALID_HANDLE_VALUE;
static __declspec(thread) bool nested;
static std::wstring handle_name(HANDLE h) {
    wchar_t path[32768]; DWORD n=GetFinalPathNameByHandleW(h,path,32768,FILE_NAME_NORMALIZED);
    return n && n<32768 ? std::wstring(path,n) : L"";
}
static bool prefix(const std::wstring& s,const wchar_t* p) {
    size_t n=wcslen(p); return s.size()>=n && !_wcsnicmp(s.c_str(),p,n);
}
static std::wstring normalize(std::wstring p) {
    if(prefix(p,L"\\??\\") || prefix(p,L"\\\\?\\")) p=p.substr(4);
    for(int i=0;i<4 && p.size()>1 && p[1]==L':';i++) {
        wchar_t drive[3]={p[0],L':',0},target[32768];
        if(!QueryDosDeviceW(drive,target,32768) || wcsncmp(target,L"\\??\\",4)) break;
        p=std::wstring(target+4)+p.substr(2);
    }
    wchar_t full[32768]; DWORD n=GetFullPathNameW(p.c_str(),32768,full,NULL);
    return n && n<32768 ? std::wstring(full,n) : L"";
}
static bool owned(const std::wstring& p) {
    bool staging=p.size()>=root.size() && !_wcsnicmp(p.c_str(),root.c_str(),root.size()) &&
        (p.size()==root.size() || p[root.size()]==L'\\');
    bool destination=!install_target.empty() && p.size()>=install_target.size() &&
        !_wcsnicmp(p.c_str(),install_target.c_str(),install_target.size()) &&
        (p.size()==install_target.size() || p[install_target.size()]==L'\\');
    return staging || destination;
}
static bool device(const std::wstring& p) {
    return prefix(p,L"\\Device\\NamedPipe\\") || prefix(p,L"\\Device\\ConDrv\\") ||
        prefix(p,L"\\Device\\Mailslot\\") || prefix(p,L"\\??\\PIPE\\") ||
        !_wcsicmp(p.c_str(),L"\\??\\CONIN$") || !_wcsicmp(p.c_str(),L"\\??\\CONOUT$") ||
        !_wcsicmp(p.c_str(),L"\\??\\NUL") || !_wcsicmp(p.c_str(),L"\\Device\\Null") ||
        !_wcsicmp(p.c_str(),L"\\Device\\Beep") || !_wcsicmp(p.c_str(),L"\\Device\\KeyboardClass0");
}
static std::wstring object_name(POBJECT_ATTRIBUTES a) {
    if(!a || !a->ObjectName || (a->ObjectName->Length&1)) return L"";
    std::wstring p(a->ObjectName->Buffer,a->ObjectName->Length/2);
    if(device(p)) return p;
    if(a->RootDirectory && !p.empty() && p[0]!=L'\\') p=handle_name(a->RootDirectory)+L"\\"+p;
    return normalize(p);
}
static void record(const wchar_t* op,const std::wstring& p,bool allow) {
    wchar_t line[32768]; int n=swprintf_s(line,L"%s %s %s\r\n",allow?L"ALLOW":L"DENY",op,p.c_str());
    IO_STATUS_BLOCK io={}; if(n>0) real_write(log_handle,NULL,NULL,NULL,&io,line,n*2,NULL,NULL);
}
static NTSTATUS refuse(PIO_STATUS_BLOCK io) {
    NTSTATUS s=(NTSTATUS)0xc0000022; if(io){io->Status=s;io->Information=0;} return s;
}
static bool modifying(ACCESS_MASK a,ULONG disposition,ULONG options) {
    return (a&(GENERIC_WRITE|GENERIC_ALL|FILE_WRITE_DATA|FILE_APPEND_DATA|FILE_WRITE_EA|
        FILE_WRITE_ATTRIBUTES|DELETE|WRITE_DAC|WRITE_OWNER)) || disposition!=1 || (options&0x1000);
}
static NTSTATUS NTAPI create_file(PHANDLE h,ACCESS_MASK a,POBJECT_ATTRIBUTES o,PIO_STATUS_BLOCK io,PLARGE_INTEGER z,ULONG attrs,ULONG share,ULONG d,ULONG options,PVOID ea,ULONG len) {
    if(nested || !modifying(a,d,options)) return real_create(h,a,o,io,z,attrs,share,d,options,ea,len);
    nested=true; auto p=object_name(o); bool allow=device(p)||owned(p);
    // Outside the install root, a read-only OPEN_IF may open an existing
    // host profile, but must never create it. Force FILE_OPEN at this test
    // boundary; missing files retain a real failure, not fabricated data.
    if(!allow && d==3 && !modifying(a,1,options)) {
        record(L"readonly-open-no-create",p,true);
        nested=false;
        return real_create(h,a,o,io,z,attrs,share,1,options,ea,len);
    }
    record(L"create",p,allow); nested=false;
    return allow?real_create(h,a,o,io,z,attrs,share,d,options,ea,len):refuse(io);
}
static NTSTATUS NTAPI open_file(PHANDLE h,ACCESS_MASK a,POBJECT_ATTRIBUTES o,PIO_STATUS_BLOCK io,ULONG share,ULONG options) {
    if(nested || !modifying(a,1,options)) return real_open(h,a,o,io,share,options);
    nested=true; auto p=object_name(o); bool allow=device(p)||owned(p); record(L"open",p,allow); nested=false;
    return allow?real_open(h,a,o,io,share,options):refuse(io);
}
static NTSTATUS NTAPI write_file(HANDLE h,HANDLE e,PVOID apc,PVOID c,PIO_STATUS_BLOCK io,PVOID b,ULONG n,PLARGE_INTEGER off,PULONG key) {
    if(nested || GetFileType(h)!=FILE_TYPE_DISK) return real_write(h,e,apc,c,io,b,n,off,key);
    nested=true; auto p=normalize(handle_name(h)); bool allow=owned(p); record(L"write",p,allow); nested=false;
    return allow?real_write(h,e,apc,c,io,b,n,off,key):refuse(io);
}
static NTSTATUS NTAPI set_file(HANDLE h,PIO_STATUS_BLOCK io,PVOID b,ULONG n,FILE_INFORMATION_CLASS cls) {
    int k=(int)cls;
    if(nested || GetFileType(h)!=FILE_TYPE_DISK ||
        !(k==4||k==10||k==11||k==13||k==19||k==20||k==40||k==64||k==65||k==72)) return real_set(h,io,b,n,cls);
    nested=true; auto p=normalize(handle_name(h)); bool allow=owned(p);
    if(allow && (k==10||k==11||k==65||k==72)) {
        struct Rename { ULONG flags; HANDLE parent; ULONG length; WCHAR name[1]; };
        auto r=(Rename*)b;
        if(n<12 || r->length>n-12 || (r->length&1)) allow=false;
        else {
            std::wstring dest(r->name,r->length/2);
            if(dest.empty()) allow=false;
            else {
                if(dest[0]!=L'\\' && !(dest.size()>1 && dest[1]==L':'))
                    dest=(r->parent?handle_name(r->parent):p.substr(0,p.find_last_of(L'\\')))+L"\\"+dest;
                p=normalize(dest); allow=owned(p);
            }
        }
    }
    record(L"set-info",p,allow); nested=false;
    return allow?real_set(h,io,b,n,cls):refuse(io);
}
static void __cdecl initialize_guard() {
    wchar_t env[32768]; DWORD n=GetEnvironmentVariableW(L"WIN101_GUARD_ROOT",env,32768);
    if(!n || n>=32768) ExitProcess(127);
    root=normalize(std::wstring(env,n));
    while(root.size()>3 && root.back()==L'\\')root.pop_back();
    if(root.empty() || (root.find(L"\\build\\")==std::wstring::npos &&
        _wcsicmp(root.c_str(),normalize(L"O:\\win101-setup").c_str()))) ExitProcess(127);
    n=GetEnvironmentVariableW(L"WIN101_GUARD_TARGET",env,32768);
    if(n) {
        if(n>=32768)ExitProcess(127);
        install_target=normalize(std::wstring(env,n));
        // Owner's S3 installation destination only, not a generic write grant.
        if(_wcsicmp(install_target.c_str(),normalize(L"O:\\win101").c_str()))ExitProcess(127);
    }
    HMODULE nt=GetModuleHandleW(L"ntdll.dll");
    real_create=(CreateFn)GetProcAddress(nt,"NtCreateFile"); real_open=(OpenFn)GetProcAddress(nt,"NtOpenFile");
    real_write=(WriteFn)GetProcAddress(nt,"NtWriteFile"); real_set=(SetFn)GetProcAddress(nt,"NtSetInformationFile");
    if(!real_create || !real_open || !real_write || !real_set) ExitProcess(127);
    log_handle=CreateFileW((root+L"\\install-writes.txt").c_str(),FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);
    if(log_handle==INVALID_HANDLE_VALUE) ExitProcess(127);
    if(DetourTransactionBegin()!=NO_ERROR || DetourUpdateThread(GetCurrentThread())!=NO_ERROR ||
        DetourAttach(&(PVOID&)real_create,create_file)!=NO_ERROR || DetourAttach(&(PVOID&)real_open,open_file)!=NO_ERROR ||
        DetourAttach(&(PVOID&)real_write,write_file)!=NO_ERROR || DetourAttach(&(PVOID&)real_set,set_file)!=NO_ERROR ||
        DetourTransactionCommit()!=NO_ERROR) ExitProcess(127);
}
#pragma section(".CRT$XCU",read)
extern "C" __declspec(allocate(".CRT$XCU")) void (__cdecl *win101_guard_entry)()=initialize_guard;
