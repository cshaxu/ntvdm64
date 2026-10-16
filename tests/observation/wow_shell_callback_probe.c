/* Modern provider witness, not Win16 feature acceptance. Only this executable
 * can be launched; the child records fallback and immediately exits. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <intrin.h>
#ifdef TEST_CDECL_CALLBACK
#define PROBE_CALLBACK_ABI __cdecl
#else
#define PROBE_CALLBACK_ABI WINAPI
#endif
#ifdef TEST_THIRD_ARGUMENT
typedef DWORD (PROBE_CALLBACK_ABI *shell_callback)(LPSTR,WORD,LPSTR);
#else
typedef DWORD (PROBE_CALLBACK_ABI *shell_callback)(LPSTR,WORD);
#endif
typedef HINSTANCE (WINAPI *wow_shell_execute_fn)(HWND,LPCSTR,LPCSTR,LPSTR,LPCSTR,INT,shell_callback);
static DWORD owner_thread, calls, wrong_thread, callback_result;
static DWORD exception_codes[16], exception_count;
static LONG CALLBACK observe_first_chance(EXCEPTION_POINTERS *info)
{
    if(exception_count<16)exception_codes[exception_count++]=info->ExceptionRecord->ExceptionCode;
    return EXCEPTION_CONTINUE_SEARCH;
}
#ifdef TEST_ADAPTER
HINSTANCE WINAPI wow_shell_execute(HWND,LPCSTR,LPCSTR,LPSTR,LPCSTR,INT,shell_callback);
shell_callback WINAPI ShellProbeCurrentCallback(void);
static LPCSTR nested_file;
static LPSTR nested_parameters;
static DWORD nested_calls;
static DWORD exception_calls, caught_exceptions;
static DWORD WINAPI throwing_callback(LPSTR command,WORD show)
{
    (void)command;(void)show;
    ++exception_calls;
    if(GetCurrentThreadId()!=owner_thread || ShellProbeCurrentCallback()!=throwing_callback)
        ++wrong_thread;
    RaiseException(0xe4470003,0,0,NULL);
    return 0;
}
static DWORD WINAPI inner_callback(LPSTR command,WORD show)
{
    (void)command;(void)show;
    ++nested_calls;
    if(GetCurrentThreadId()!=owner_thread)++wrong_thread;
    return 2;
}
#endif
static LONG capture_exception(EXCEPTION_POINTERS *info)
{
    HMODULE module=NULL;char path[MAX_PATH]="<unknown>";
    DWORD i;
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)info->ExceptionRecord->ExceptionAddress,&module);
    if(module)GetModuleFileNameA(module,path,sizeof(path));
    printf("exception=%08lx module=%s rva=%08lx access=%lu address=%08lx\n",
        info->ExceptionRecord->ExceptionCode,path,
        (DWORD)((ULONG_PTR)info->ExceptionRecord->ExceptionAddress-(ULONG_PTR)module),
        info->ExceptionRecord->NumberParameters>0?(DWORD)info->ExceptionRecord->ExceptionInformation[0]:0,
        info->ExceptionRecord->NumberParameters>1?(DWORD)info->ExceptionRecord->ExceptionInformation[1]:0);
    printf("first-chance:");
    for(i=0;i<exception_count;++i)printf(" %08lx",exception_codes[i]);
    putchar('\n');
    return EXCEPTION_EXECUTE_HANDLER;
}
#ifdef TEST_THROW_CONTROL
static wow_shell_execute_fn control_execute;
static LPCSTR control_file;
static LPSTR control_parameters;
static DWORD WINAPI control_throw(LPSTR command,WORD show,LPSTR directory)
{
    (void)command;(void)show;(void)directory;
    RaiseException(0xe4470003,0,0,NULL);
    return 0;
}
#endif
static DWORD PROBE_CALLBACK_ABI callback(LPSTR command,WORD show
#ifdef TEST_THIRD_ARGUMENT
    ,LPSTR third
#endif
    )
{
    BYTE *return_address=(BYTE *)_ReturnAddress();unsigned i;
    ++calls;
    if(GetCurrentThreadId()!=owner_thread)++wrong_thread;
    printf("callback thread=%lu show=%u command=%s result=%lu\n",
        GetCurrentThreadId(),show,command?command:"<null>",callback_result);
#ifdef TEST_THIRD_ARGUMENT
    printf("callback third=%s\n",third?third:"<null>");
#endif
    printf("callback-return bytes[-16..31]=");
    for(i=0;i!=48;++i)printf("%02x",return_address[(int)i-16]);
    putchar('\n');
#ifdef TEST_OUTER_BOUNDARY
    /* W32Dispatch catches outside WS32ShellExecute, not inside a resumed
     * outer Shell callback. Keep this distinct from the nested negative. */
    if(callback_result==43)RaiseException(0xe4470003,0,0,NULL);
#endif
#ifdef TEST_THROW_CONTROL
    if(callback_result==43){
        __try {
            (void)control_execute(NULL,"open",control_file,control_parameters,NULL,SW_HIDE,control_throw);
            ++wrong_thread;
        } __except(GetExceptionCode()==0xe4470003 ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH){
            puts("original injected exception caught by outer callback");
        }
    }
#endif
#ifdef TEST_ADAPTER
    if(ShellProbeCurrentCallback()!=callback)++wrong_thread;
    if(callback_result==42 && (ULONG_PTR)wow_shell_execute(NULL,"open",nested_file,
        nested_parameters,NULL,SW_HIDE,inner_callback)!=2)++wrong_thread;
    if(callback_result==43){
        __try {
            (void)wow_shell_execute(NULL,"open",nested_file,nested_parameters,NULL,
                SW_HIDE,throwing_callback);
            ++wrong_thread; /* The original callback exception must propagate. */
        } __except(GetExceptionCode()==0xe4470003 ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH){
            ++caught_exceptions;
        }
    }
    if(ShellProbeCurrentCallback()!=callback)++wrong_thread;
#endif
    return callback_result;
}
int main(int argc,char **argv)
{
    char system[MAX_PATH],library[MAX_PATH],self[MAX_PATH],parameters[MAX_PATH+32];
    HMODULE module;wow_shell_execute_fn execute;unsigned test;
    setvbuf(stdout,NULL,_IONBF,0);
    if(argc==3 && !strcmp(argv[1],"--child")) {
        HANDLE f=CreateFileA(argv[2],GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
        if(f!=INVALID_HANDLE_VALUE){DWORD bytes;WriteFile(f,"native-fallback",15,&bytes,NULL);CloseHandle(f);}
        return 0;
    }
    if(argc!=2 || !GetSystemDirectoryA(system,sizeof(system)) ||
        !GetModuleFileNameA(NULL,self,sizeof(self)))return 2;
    if(sprintf_s(library,sizeof(library),"%s\\shell32.dll",system)<0 ||
        sprintf_s(parameters,sizeof(parameters),"--child \"%s\"",argv[1])<0)return 3;
    module=LoadLibraryA(library);if(!module)return 4;
    execute=(wow_shell_execute_fn)GetProcAddress(module,"WOWShellExecute");
#ifdef TEST_ADAPTER
    execute=wow_shell_execute;nested_file=self;nested_parameters=parameters;
#endif
    if(!execute){puts("provider export absent");return 5;}
    owner_thread=GetCurrentThreadId();
    if(!AddVectoredExceptionHandler(1,observe_first_chance))return 11;
#ifdef TEST_THROW_CONTROL
    control_execute=execute;control_file=self;control_parameters=parameters;
#endif
    GetModuleFileNameA(module,library,sizeof(library));
    printf("provider=%s caller_thread=%lu\n",library,owner_thread);
    for(test=0;test!=
#if defined(TEST_ADAPTER) || defined(TEST_THROW_CONTROL) || defined(TEST_OUTER_BOUNDARY)
        3
#else
        2
#endif
        ;++test){
        HINSTANCE result;
        /* A failure result needs no real launched instance. Keep success
         * separate: 42 is only a sentinel, not an actual Win16 HINSTANCE. */
        calls=wrong_thread=0;callback_result=test?41+test:2;
        SetLastError(0x447);
        __try { result=execute(NULL,"open",self,parameters,NULL,SW_HIDE,callback); }
        __except(capture_exception(GetExceptionInformation())) {
#ifdef TEST_OUTER_BOUNDARY
            if(test!=2 || GetExceptionCode()!=0xe4470003 || calls!=1 || wrong_thread)return 12;
#ifdef TEST_ADAPTER
            if(ShellProbeCurrentCallback()!=NULL)return 13;
#endif
            puts("PASS outer API exception boundary and restored callback TLS");
            calls=wrong_thread=0;callback_result=2;
            result=execute(NULL,"open",self,parameters,NULL,SW_HIDE,callback);
            puts("PASS provider invoked again after outer unwind");
#else
            printf("FAIL provider exception calls=%lu foreign_thread=%lu\n",calls,wrong_thread);
#ifdef TEST_ADAPTER
            printf("exception-scope thrown=%lu caught=%lu tls_clear=%u\n",
                exception_calls,caught_exceptions,ShellProbeCurrentCallback()==NULL);
#endif
            return 7;
#endif
        }
        printf("case=%u result=%lu error=%lu calls=%lu foreign_thread=%lu\n",
            test,(DWORD)(ULONG_PTR)result,GetLastError(),calls,wrong_thread);
        if(calls!=1 || wrong_thread || (ULONG_PTR)result!=callback_result)return 6;
#ifdef TEST_ADAPTER
        if(ShellProbeCurrentCallback()!=NULL)return 9;
#endif
    }
    FreeLibrary(module);
#ifdef TEST_ADAPTER
    if(nested_calls!=1)return 8;
#ifndef TEST_OUTER_BOUNDARY
    if(exception_calls!=1 || caught_exceptions!=1)return 10;
    puts("PASS actual adapter exception propagation and inner/outer TLS restoration");
#endif
    puts("PASS actual adapter old two-argument callback and nested invocation");
#endif
    puts("PASS modern WOWShellExecute synchronous callback and success/failure result");
    return 0;
}
