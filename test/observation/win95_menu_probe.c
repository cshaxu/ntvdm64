/* Independent original-Win95 shell link/menu witness; no input synthesis.
 * Own canary processes are opened through shell links and closed normally.
 */
#define WIN32_LEAN_AND_MEAN
#define COBJMACROS
#define CINTERFACE
#include <windows.h>
#include <shellapi.h>
#include <objbase.h>
#include <shlobj.h>
static HRESULT (WINAPI *shell_factory)(REFCLSID,REFIID,LPVOID*);
static LONG (WINAPI *reg_open)(HKEY,LPCSTR,DWORD,REGSAM,PHKEY);
static LONG (WINAPI *reg_query)(HKEY,LPCSTR,LPDWORD,LPDWORD,LPBYTE,LPDWORD);
static LONG (WINAPI *reg_close)(HKEY);
static BOOL (WINAPI *shell_execute)(SHELLEXECUTEINFOA*);
#define RegOpenKeyExA reg_open
#define RegQueryValueExA reg_query
#define RegCloseKey reg_close
#define ShellExecuteExA shell_execute
static HANDLE output;
static char line[2048],module[MAX_PATH],programs[MAX_PATH];
static unsigned links,missing;
static BOOL launchCanaries=TRUE;
static BOOL appsOnly=FALSE;
static unsigned add(unsigned n,const char *s){while(*s&&n<2000)line[n++]=*s++;return n;}
static unsigned hex(unsigned n,DWORD x){int i;static const char d[]="0123456789ABCDEF";for(i=7;i>=0;i--)line[n++]=d[(x>>(i*4))&15];return n;}
static void write(unsigned n){DWORD done;line[n++]='\r';line[n++]='\n';WriteFile(output,line,n,&done,0);}
static void value(const char *name,DWORD x){unsigned n=add(0,name);n=add(n,"=");write(hex(n,x));}
static void text(const char *name,const char *x){unsigned n=add(0,name);n=add(n,"=");write(add(n,x));}
static FARPROC bind(const char *dll,const char *name){HMODULE m;FARPROC p;text("BIND_BEGIN",name);m=LoadLibraryA(dll);value("BIND_MODULE",(DWORD)m);if(!m){value("BIND_ERROR",GetLastError());return 0;}p=GetProcAddress(m,name);value("BIND_RESULT",(DWORD)p);return p;}
static char lastTarget[MAX_PATH];
static BOOL inspect(const char *file){
 IShellLinkA *link=0;IPersistFile *persist=0;IClassFactory *factory=0;HRESULT hr;WCHAR wide[MAX_PATH];char target[MAX_PATH];BOOL ok=FALSE;
 text("LINK",file);value("LINK_ATTR",GetFileAttributesA(file));if(!MultiByteToWideChar(CP_ACP,0,file,-1,wide,MAX_PATH))return FALSE;
 /* Use the original shell's in-process object factory, with no marshaling
  * or substituted parser. This does not require a separate OLE32 service. */
 hr=shell_factory(&CLSID_ShellLink,&IID_IClassFactory,(void**)&factory);
 if(SUCCEEDED(hr)){hr=IClassFactory_CreateInstance(factory,0,&IID_IShellLinkA,(void**)&link);IClassFactory_Release(factory);}
 value("CREATE",hr);if(FAILED(hr))return FALSE;
 hr=IShellLinkA_QueryInterface(link,&IID_IPersistFile,(void**)&persist);
 if(SUCCEEDED(hr)){
  hr=IPersistFile_Load(persist,wide,STGM_READ);value("LOAD",hr);
  if(SUCCEEDED(hr)){
   target[0]=0;hr=IShellLinkA_GetPath(link,target,MAX_PATH,0,0);value("GETPATH",hr);
   if(!target[0]){hr=IShellLinkA_Resolve(link,0,SLR_NO_UI);value("RESOLVE",hr);hr=IShellLinkA_GetPath(link,target,MAX_PATH,0,0);value("RESOLVED_GETPATH",hr);}
   text("TARGET",target);lstrcpyA(lastTarget,target);
   value("ATTR",GetFileAttributesA(target));ok=SUCCEEDED(hr)&&target[0]&&GetFileAttributesA(target)!=INVALID_FILE_ATTRIBUTES;
  }
  IPersistFile_Release(persist);
 }
 IShellLinkA_Release(link);return ok;
}
static void enumerate(const char *root,unsigned depth){
 char pattern[MAX_PATH],path[MAX_PATH];WIN32_FIND_DATAA data;HANDLE search;unsigned n;
 if(depth>5)return;lstrcpyA(pattern,root);lstrcatA(pattern,"\\*.*");
 search=FindFirstFileA(pattern,&data);if(search==INVALID_HANDLE_VALUE){value("ENUM_ERROR",GetLastError());return;}
 do {
  if(data.cFileName[0]=='.')continue;lstrcpyA(path,root);lstrcatA(path,"\\");lstrcatA(path,data.cFileName);
  text("ENTRY",path);
  if(data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY){enumerate(path,depth+1);continue;}
  n=(unsigned)lstrlenA(path);if(n>=4&&!lstrcmpiA(path+n-4,".lnk")){links++;if(!inspect(path))missing++;}
 }while(FindNextFileA(search,&data));FindClose(search);
}
static BOOL CALLBACK childText(HWND window,LPARAM unused){char label[256];(void)unused;GetClassNameA(window,label,sizeof(label));text("CHILD_CLASS",label);GetWindowTextA(window,label,sizeof(label));text("CHILD_TEXT",label);return TRUE;}
static BOOL CALLBACK processWindow(HWND window,LPARAM processId){DWORD owner;char label[256];GetWindowThreadProcessId(window,&owner);if(owner==(DWORD)processId){GetClassNameA(window,label,sizeof(label));text("PROCESS_WINDOW_CLASS",label);GetWindowTextA(window,label,sizeof(label));text("PROCESS_WINDOW_TEXT",label);EnumChildWindows(window,childText,0);}return TRUE;}
static void canary(const char *name,const char *file,const char *className){
 static STARTUPINFOA startup;static PROCESS_INFORMATION process;char path[MAX_PATH];DWORD i,code=0;HWND window=0;BOOL loaded;
 lstrcpyA(path,module);lstrcatA(path,file);text("CANARY",name);loaded=inspect(path);value("CANARY_TARGET_OK",loaded);
 if(!loaded||!launchCanaries)return;
 startup.cb=sizeof(startup);process.hProcess=0;process.hThread=0;
 value("CREATEPROCESS",CreateProcessA(lastTarget,0,0,0,FALSE,0,0,0,&startup,&process));value("CREATE_ERROR",GetLastError());
 for(i=0;i<60;i++){window=FindWindowA(className,0);if(window)break;if(i==3&&process.hProcess)EnumWindows(processWindow,(LPARAM)process.dwProcessId);Sleep(10);}
 text("CANARY_CLASS",className);value("CANARY_WINDOW",!!window);
 if(!window&&process.hProcess)EnumWindows(processWindow,(LPARAM)process.dwProcessId);
 if(process.hProcess){GetExitCodeProcess(process.hProcess,&code);value("CANARY_PROCESS",code);}
 if(window)PostMessageA(window,WM_CLOSE,0,0);
 if(process.hThread)CloseHandle(process.hThread);
 if(process.hProcess){value("CANARY_CLOSE_WAIT",WaitForSingleObject(process.hProcess,5000));CloseHandle(process.hProcess);}
}
__declspec(noreturn) void entry(void){
 HRESULT hr;HKEY key=0;DWORD size=MAX_PATH,type=0,i,version=GetVersion();MEMORYSTATUS memory;
 output=CreateFileA("O:\\WINDOWS\\PATCH\\TESTS\\MENU95.LOG",GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,0,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,0);
 if(output==INVALID_HANDLE_VALUE)ExitProcess(91);
 value("GUEST_VERSION",version);value("ACP",GetACP());GetModuleFileNameA(0,module,MAX_PATH);
 memory.dwLength=sizeof(memory);GlobalMemoryStatus(&memory);value("TOTAL_PHYS",memory.dwTotalPhys);value("AVAIL_PHYS",memory.dwAvailPhys);value("AVAIL_VIRTUAL",memory.dwAvailVirtual);
 value("LONG_PROGRAM_FILES_ATTR",GetFileAttributesA("O:\\Program Files\\ACCESS~1\\MSPAINT.EXE"));
 value("SHORT_PROGRAM_FILES_ATTR",GetFileAttributesA("O:\\PROGRA~1\\ACCESS~1\\MSPAINT.EXE"));
 if(!(version&0x80000000u)||(version&0xffffu)!=4){CloseHandle(output);ExitProcess(90);}
 reg_open=(void*)bind("ADVAPI32.DLL","RegOpenKeyExA");reg_query=(void*)bind("ADVAPI32.DLL","RegQueryValueExA");reg_close=(void*)bind("ADVAPI32.DLL","RegCloseKey");
 shell_execute=(void*)bind("SHELL32.DLL","ShellExecuteExA");
 shell_factory=(void*)bind("SHELL32.DLL","DllGetClassObject");
 if(!shell_factory||!reg_open||!reg_query||!reg_close||!shell_execute){CloseHandle(output);ExitProcess(92);}
 for(i=(DWORD)lstrlenA(module);i;i--)if(module[i-1]=='\\'){module[i]=0;break;}
 {char path[MAX_PATH];lstrcpyA(path,module);lstrcatA(path,"INSPECT.TAG");if(GetFileAttributesA(path)!=INVALID_FILE_ATTRIBUTES)launchCanaries=FALSE;value("LAUNCH_ENABLED",launchCanaries);}
 {char path[MAX_PATH];lstrcpyA(path,module);lstrcatA(path,"APPS.TAG");appsOnly=GetFileAttributesA(path)!=INVALID_FILE_ATTRIBUTES;value("APPS_ONLY",appsOnly);}
 hr=S_OK;
 if(SUCCEEDED(hr)){
  value("REGOPEN",RegOpenKeyExA(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",0,KEY_QUERY_VALUE,&key));
  if(key){value("REGQUERY",RegQueryValueExA(key,"Programs",0,&type,(BYTE*)programs,&size));RegCloseKey(key);}
  text("PROGRAMS",programs);if(programs[0]&&!appsOnly)enumerate(programs,0);
  value("MENU_LINKS",links);value("MENU_MISSING",missing);
  {char path[MAX_PATH];lstrcpyA(path,module);lstrcatA(path,"\xb2\xe2\xca\xd4.LNK");value("CHINESE_FILENAME",inspect(path));}
  canary("NOTEPAD","NOTE.LNK","Notepad");
  canary("PAINT","PAINT.LNK","MSPaintApp");
  canary("WORDPAD","WORD.LNK","WordPadClass");
 }
 write(add(0,"MENU_INSPECTION_COMPLETE"));CloseHandle(output);ExitProcess(0);
}
