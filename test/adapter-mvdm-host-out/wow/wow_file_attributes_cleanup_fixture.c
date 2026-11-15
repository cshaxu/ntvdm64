/* Exact selected thunk; guest addressing and pathname/API providers are bounded
 * test substitutes. This proves cleanup ownership, not DOS path integration. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
typedef struct {WORD pszPathSegment,pszPathOffset,wAttributes;} ARGS,*PWOWFILESETATTRIBUTES16,*PVDMFRAME;
typedef ARGS WOWFILESETATTRIBUTES16;
static char *input;static BYTE drive;static struct {ULONG_PTR lpCurDrv;} DosWowData;
static int normalizes,sets,frees,pipe_case,normalize_failure,set_failure,checks,failures;
static DWORD seen_attributes;
#define FASTCALL
#define GETARGPTR(frame,size,p) ((p)=(frame))
#define FREEARGPTR(p) ((void)0)
#define FETCHWORD(x) (x)
#define SEGPTR(s,o) input
#define WOW32ASSERT(x) ((void)0)
#define IS_ASCII_PATH_SEPARATOR(c) ((c)=='\\'||(c)=='/')
#define DOS_ATTR_MASK 0x003f
#define CHECK(x) do{++checks;if(!(x)){++failures;printf("FAIL line=%d %s\n",__LINE__,#x);}}while(0)
static PSTR NormalizeDosPath(PSTR path,WORD current,PBOOL pipe) {
 (void)current;++normalizes;*pipe=pipe_case;
 if(normalize_failure)return NULL;
 return pipe_case?(PSTR)LocalAlloc(LMEM_FIXED,8):path;
}
static BOOL SetFileAttributesOem(PSTR path,DWORD attributes){CHECK(path!=NULL);++sets;seen_attributes=attributes;SetLastError(ERROR_ACCESS_DENIED);return !set_failure;}
static HLOCAL free_owned(HLOCAL p){++frees;return LocalFree(p);}
#define LocalFree free_owned
#include THUNK_SOURCE
#undef LocalFree
static void run(char *path,int pipe,int nf,int sf,WORD attributes,ULONG expected,int n,int s,int f) {
 ARGS args={0,0,attributes};ULONG result;
 input=path;pipe_case=pipe;normalize_failure=nf;set_failure=sf;normalizes=sets=frees=0;seen_attributes=0;
 result=WK32FileSetAttributes(&args);
 CHECK(result==expected);CHECK(normalizes==n);CHECK(sets==s);CHECK(frees==f);
 if(s)CHECK(seen_attributes==((attributes?attributes:FILE_ATTRIBUTE_NORMAL)&DOS_ATTR_MASK));
}
int main(void){
 DosWowData.lpCurDrv=(ULONG_PTR)&drive;
 run("C:\\dir\\",0,0,0,0,0xffff0003,0,0,0);
 run("C:\\dir/",0,0,0,0,0xffff0003,0,0,0);
 run("C:\\",0,0,0,0,0,1,1,0);
 run("\\",0,0,0,0,0,1,1,0);
 run("file",0,0,0,0xffff,0,1,1,0);
 run("file",0,0,1,1,0xffff0005,1,1,0);
 run("file",0,1,0,0,0xffff0003,1,0,0);
 run("pipe",1,0,0,1,0,1,1,1);
 run("pipe",1,0,1,1,0xffff0005,1,1,1);
 run("pipe",1,1,0,0,0xffff0003,1,0,1);
 printf("WOW_FILE_ATTRIBUTES checks=%d failures=%d\n",checks,failures);return failures?1:0;
}
