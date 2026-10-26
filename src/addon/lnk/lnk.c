/* LNK.EXE: offline classic Shell Link reader/writer. No COM resolution.
 * Format: Microsoft MS-SHLLINK, sections 2.1--2.5. Project-authored code.
 * Writers emit ANSI LinkInfo/StringData for Win95, discarding stale PIDLs.
 * Unknown extra blocks are readable but editing them is deliberately refused.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <io.h>
#include <fcntl.h>

#define LIMIT (1024u*1024u)
static const BYTE clsid[16]={1,0x14,2,0,0,0,0,0,0xc0,0,0,0,0,0,0,0x46};
typedef struct { BYTE header[76]; wchar_t *target,*s[5]; DWORD extra; BOOL writable; } LINK;
typedef struct { BYTE bytes[LIMIT]; DWORD used; } OUTPUT;
static DWORD u32(const BYTE *p){return (DWORD)p[0]|((DWORD)p[1]<<8)|((DWORD)p[2]<<16)|((DWORD)p[3]<<24);}
static WORD u16(const BYTE *p){return (WORD)(p[0]|((WORD)p[1]<<8));}
static void put32(BYTE *p,DWORD v){p[0]=(BYTE)v;p[1]=(BYTE)(v>>8);p[2]=(BYTE)(v>>16);p[3]=(BYTE)(v>>24);}
static void put16(BYTE *p,WORD v){p[0]=(BYTE)v;p[1]=(BYTE)(v>>8);}
static void *alloc(size_t n){return HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,n);}
static void release(void *p){if(p)HeapFree(GetProcessHeap(),0,p);}
static wchar_t *copy(const wchar_t *s){size_t n=(wcslen(s)+1)*sizeof(wchar_t);wchar_t *p=alloc(n);if(p)memcpy(p,s,n);return p;}
static void dispose(LINK *v){unsigned i;release(v->target);for(i=0;i<5;i++)release(v->s[i]);}
static BOOL take(DWORD *at,DWORD size,DWORD count){if(*at>size||count>size-*at)return FALSE;*at+=count;return TRUE;}
static wchar_t *decode(const BYTE *b,DWORD n,BOOL unicode,UINT cp){
 wchar_t *s;int count;
 if(unicode){if(n&1)return NULL;s=alloc((size_t)n+2);if(s)memcpy(s,b,n);return s;}
 if(!n)return copy(L"");
 count=MultiByteToWideChar(cp,MB_ERR_INVALID_CHARS,(const char*)b,(int)n,NULL,0);
 if(!count)return NULL;s=alloc(((size_t)count+1)*2);if(s&&!MultiByteToWideChar(cp,MB_ERR_INVALID_CHARS,(const char*)b,(int)n,s,count)){release(s);return NULL;}return s;
}
static wchar_t *cstring(const BYTE *b,DWORD n,DWORD at,UINT cp){DWORD end=at;if(at>=n)return NULL;while(end<n&&b[end])end++;return end<n?decode(b+at,end-at,FALSE,cp):NULL;}
static wchar_t *wstring(const BYTE *b,DWORD n,DWORD at,UINT cp){DWORD end=at;if(at>=n)return NULL;while(end+1<n&&u16(b+end))end+=2;return end+1<n?decode(b+at,end-at,TRUE,cp):NULL;}
static BOOL load(const wchar_t *path,LINK *v,UINT cp){
 HANDLE f;BYTE *b=NULL;DWORD n,got,at=76,flags,i;BOOL ok=FALSE;
 f=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);if(f==INVALID_HANDLE_VALUE)return FALSE;
 n=GetFileSize(f,NULL);if(n<76||n>LIMIT)goto done;b=alloc(n);if(!b||!ReadFile(f,b,n,&got,NULL)||got!=n)goto done;
 if(u32(b)!=76||memcmp(b+4,clsid,16))goto done;memcpy(v->header,b,76);flags=u32(b+20);v->writable=!(flags&~0xffu);
 if(flags&1){DWORD end,j;WORD size;if(!take(&at,n,2))goto done;end=at+u16(b+at-2);if(end>n)goto done;j=at;
  for(;;){if(j+2>end)goto done;size=u16(b+j);if(!size){if(j+2!=end)goto done;break;}if(size<2||size>end-j)goto done;j+=size;}at=end;}
 if(flags&2){DWORD len,hdr,local,suffix,vol;wchar_t *tail;
  if(n-at<28)goto done;len=u32(b+at);hdr=u32(b+at+4);if(len<28||len>n-at||(hdr!=28&&hdr<36)||hdr>len)goto done;
  if(u32(b+at+8)!=1)goto done; /* local volume only; UNC is not edited */
  local=u32(b+at+16);suffix=u32(b+at+24);vol=u32(b+at+12);
  /* Win95 leaves the inactive network offset uninitialized. Ignore it when
   * LinkInfoFlags has no network bit; never dereference inactive offsets. */
  if(local<hdr||suffix<hdr||vol<hdr||vol>len-16)goto done;
  if(u32(b+at+vol)<17||u32(b+at+vol)>len-vol||u32(b+at+vol+12)>=u32(b+at+vol))goto done;
  v->target=cstring(b+at,len,local,cp);tail=cstring(b+at,len,suffix,cp);if(!v->target||!tail){release(tail);goto done;}
  if(hdr>=36){DWORD wide=u32(b+at+28),wideTail=u32(b+at+32);if(wide){wchar_t *p;if(wide<hdr){release(tail);goto done;}p=wstring(b+at,len,wide,cp);if(!p){release(tail);goto done;}release(v->target);v->target=p;}if(wideTail){wchar_t *p;if(wideTail<hdr){release(tail);goto done;}p=wstring(b+at,len,wideTail,cp);if(!p){release(tail);goto done;}release(tail);tail=p;}}
  if(*tail){size_t a=wcslen(v->target),z=wcslen(tail);if(a<z||_wcsicmp(v->target+a-z,tail)){
    wchar_t *joined=alloc((a+z+2)*2);if(!joined){release(tail);goto done;}wcscpy_s(joined,a+z+2,v->target);if(a&&v->target[a-1]!=L'\\'&&tail[0]!=L'\\')wcscat_s(joined,a+z+2,L"\\");wcscat_s(joined,a+z+2,tail);release(v->target);v->target=joined;}}
  release(tail);at+=len;
 }
 for(i=0;i<5;i++)if(flags&(4u<<i)){DWORD count,start;if(!take(&at,n,2))goto done;count=u16(b+at-2);if(flags&128)count*=2;start=at;if(!take(&at,n,count))goto done;v->s[i]=decode(b+start,count,!!(flags&128),cp);if(!v->s[i])goto done;}
 v->extra=n-at;if(v->extra){DWORD j=at;BOOL terminal=FALSE;while(j<n){DWORD size;if(n-j<4)goto done;size=u32(b+j);if(!size){if(j+4!=n)goto done;terminal=TRUE;break;}if(size<8||size>n-j)goto done;j+=size;}if(!terminal)goto done;if(v->extra!=4)v->writable=FALSE;}
 ok=TRUE;
done:release(b);CloseHandle(f);return ok;
}
static BOOL append(OUTPUT *o,const void *p,DWORD n){if(n>LIMIT-o->used)return FALSE;memcpy(o->bytes+o->used,p,n);o->used+=n;return TRUE;}
static char *encode(const wchar_t *s,UINT cp,DWORD *count){
 BOOL fallback=FALSE;int n;char *b;DWORD flags=cp==CP_UTF8?WC_ERR_INVALID_CHARS:WC_NO_BEST_FIT_CHARS;
 n=WideCharToMultiByte(cp,flags,s,-1,NULL,0,NULL,cp==CP_UTF8?NULL:&fallback);if(!n||fallback)return NULL;
 b=alloc((size_t)n);if(!b)return NULL;
 if(!WideCharToMultiByte(cp,flags,s,-1,b,n,NULL,cp==CP_UTF8?NULL:&fallback)||fallback){release(b);return NULL;}*count=(DWORD)n;return b;
}
static BOOL pidl(LINK *v,UINT cp,OUTPUT *o){
 /* Classic shell filesystem items observed in the supplied Win95 links:
  * Desktop/My Computer root, 25-byte drive item, 14-byte file-item header,
  * NUL-terminated 8.3 name and empty secondary name. No modern extensions. */
 BYTE root[20]={20,0,0x1f,0,0xe0,0x4f,0xd0,0x20,0xea,0x3a,0x69,0x10,0xa2,0xd8,8,0,0x2b,0x30,0x30,0x9d};
 BYTE drive[25]={25,0,0x23},size[2]={0},item[16]={0};DWORD at=o->used,i=3,start,n;char *path=encode(v->target,cp,&n);
 if(!path)return FALSE;drive[3]=(BYTE)path[0];drive[4]=':';drive[5]='\\';
 if(!append(o,size,2)||!append(o,root,20)||!append(o,drive,25))goto fail;
 while(path[i]){DWORD count,base=0,ext=0;BOOL dot=FALSE,last;
  start=i;while(path[i]&&path[i]!='\\')i++;count=i-start;last=!path[i];
  if(!count||count>12)goto fail;
  for(n=0;n<count;n++){BYTE c=(BYTE)path[start+n];if(c=='.'){if(dot)goto fail;dot=TRUE;continue;}if(c<33||c>=127||strchr("\"*+,/:;<=>?[\\]|",c))goto fail;if(dot)ext++;else base++;}
  if(!base||base>8||ext>3)goto fail;
  put16(item,(WORD)(14+count+2));item[2]=(BYTE)((last&&!(u32(v->header+24)&FILE_ATTRIBUTE_DIRECTORY))?0x32:0x31);
  put16(item+12,(WORD)(0x8000|(item[2]==0x31?FILE_ATTRIBUTE_DIRECTORY:FILE_ATTRIBUTE_ARCHIVE)));
  if(!append(o,item,14)||!append(o,path+start,count)||!append(o,size,2))goto fail;
  if(!last)i++;
 }
 if(!append(o,size,2)||o->used-at-2>65535)goto fail;
 put16(o->bytes+at,(WORD)(o->used-at-2));release(path);return TRUE;
fail:release(path);return FALSE;
}
static BOOL serialize(LINK *v,UINT cp,OUTPUT *o){
 BYTE li[45]={0},zero[4]={0};DWORD n,flags=3,i;char *text;
 if(!v->writable||!v->target||!((v->target[0]>=L'A'&&v->target[0]<=L'Z')||(v->target[0]>=L'a'&&v->target[0]<=L'z'))||v->target[1]!=L':'||v->target[2]!=L'\\')return FALSE;
 for(i=0;i<5;i++)if(v->s[i])flags|=4u<<i;
 put32(v->header+20,flags);if(!append(o,v->header,76))return FALSE;
 if(!pidl(v,cp,o))return FALSE;
 text=encode(v->target,cp,&n);if(!text)return FALSE;
 put32(li,45+n+1);put32(li+4,28);put32(li+8,1);put32(li+12,28);put32(li+16,45);put32(li+24,45+n);
 put32(li+28,17);put32(li+32,DRIVE_FIXED);put32(li+40,16);
 if(!append(o,li,45)||!append(o,text,n)||!append(o,zero,1)){release(text);return FALSE;}release(text);
 for(i=0;i<5;i++)if(v->s[i]){BYTE count[2];text=encode(v->s[i],cp,&n);if(!text)return FALSE;if(n-1>65535){release(text);return FALSE;}put16(count,(WORD)(n-1));if(!append(o,count,2)||!append(o,text,n-1)){release(text);return FALSE;}release(text);}
 return append(o,zero,4);
}
static BOOL save(const wchar_t *path,OUTPUT *o,BOOL replace){
 wchar_t full[MAX_PATH],dir[MAX_PATH],tmp[MAX_PATH],*leaf;HANDLE f;DWORD written;BOOL ok;
 written=GetFullPathNameW(path,MAX_PATH,full,&leaf);if(!written||written>=MAX_PATH||!leaf)return FALSE;wcscpy_s(dir,MAX_PATH,full);dir[leaf-full]=0;
 if(!GetTempFileNameW(dir,L"lnk",0,tmp))return FALSE;
 f=CreateFileW(tmp,GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);if(f==INVALID_HANDLE_VALUE){DeleteFileW(tmp);return FALSE;}
 ok=WriteFile(f,o->bytes,o->used,&written,NULL)&&written==o->used&&FlushFileBuffers(f);CloseHandle(f);
 if(ok)ok=MoveFileExW(tmp,full,MOVEFILE_WRITE_THROUGH|(replace?MOVEFILE_REPLACE_EXISTING:0));if(!ok)DeleteFileW(tmp);return ok;
}
static BOOL set(wchar_t **p,const wchar_t *s){wchar_t *n=copy(s);if(!n)return FALSE;release(*p);*p=n;return TRUE;}
static BOOL number(const wchar_t *s,DWORD max,DWORD *n){wchar_t *end;unsigned long v;if(!*s||*s==L'-')return FALSE;v=wcstoul(s,&end,0);if(*end||v>max)return FALSE;*n=(DWORD)v;return TRUE;}
static void show(LINK *v,UINT cp){unsigned i;static const wchar_t *names[]={L"DESCRIPTION",L"RELATIVE",L"DIRECTORY",L"ARGUMENTS",L"ICON"};
 wprintf(L"CODEPAGE=%u\nTARGET=%ls\n",cp,v->target?v->target:L"");for(i=0;i<5;i++)wprintf(L"%ls=%ls\n",names[i],v->s[i]?v->s[i]:L"");
 wprintf(L"ICON_INDEX=%ld\nSHOW=%lu\nHOTKEY=%u\nPIDL=%u\nEXTRA_BYTES=%lu\nWRITABLE=%u\n",(LONG)u32(v->header+56),(unsigned long)u32(v->header+60),u16(v->header+64),!!(u32(v->header+20)&1),(unsigned long)v->extra,v->writable);}
static void usage(void){fwprintf(stderr,L"LNK.EXE show FILE [--codepage N]\nLNK.EXE create|update FILE --target P [--directory D] [--arguments A] [--description T] [--icon I] [--icon-index N] [--show N] [--hotkey N] [--codepage N]\n");}
int wmain(int argc,wchar_t **argv){
 LINK v={0};OUTPUT *o=NULL;UINT cp=GetACP();DWORD n;int i,result=1;BOOL create;
 _setmode(_fileno(stdout),_O_U8TEXT);
 if(argc<3){usage();return 64;}
 for(i=3;i<argc;i+=2){if(i+1>=argc){usage();return 64;}if(!_wcsicmp(argv[i],L"--codepage")){if(!number(argv[i+1],65535,&n)||!IsValidCodePage(n)){usage();return 64;}cp=n;}}
 create=!_wcsicmp(argv[1],L"create");
 if(!create&&_wcsicmp(argv[1],L"show")&&_wcsicmp(argv[1],L"update")){usage();return 64;}
 if(create){put32(v.header,76);memcpy(v.header+4,clsid,16);put32(v.header+24,FILE_ATTRIBUTE_ARCHIVE);put32(v.header+60,1);v.writable=TRUE;}
 else if(!load(argv[2],&v,cp)){fwprintf(stderr,L"LNK.EXE: invalid or unsupported link\n");goto done;}
 if(!_wcsicmp(argv[1],L"show")){for(i=3;i<argc;i+=2)if(_wcsicmp(argv[i],L"--codepage")){usage();result=64;goto done;}show(&v,cp);result=0;goto done;}
 for(i=3;i<argc;i+=2){wchar_t *key=argv[i],*value=argv[i+1];
  if(!_wcsicmp(key,L"--codepage"))continue;
  if(!_wcsicmp(key,L"--target")){if(!set(&v.target,value))goto done;release(v.s[1]);v.s[1]=NULL;}
  else if(!_wcsicmp(key,L"--directory")){if(!set(&v.s[2],value))goto done;}
  else if(!_wcsicmp(key,L"--arguments")){if(!set(&v.s[3],value))goto done;}
  else if(!_wcsicmp(key,L"--description")){if(!set(&v.s[0],value))goto done;}
  else if(!_wcsicmp(key,L"--icon")){if(!set(&v.s[4],value))goto done;}
  else if(!_wcsicmp(key,L"--icon-index")){wchar_t *end;__int64 index=_wcstoi64(value,&end,0);if(!*value||*end||index<(-2147483647LL-1)||index>2147483647LL)goto done;put32(v.header+56,(DWORD)(LONG)index);}
  else if(!_wcsicmp(key,L"--show")){if(!number(value,7,&n)||(n!=1&&n!=3&&n!=7))goto done;put32(v.header+60,n);}
  else if(!_wcsicmp(key,L"--hotkey")){if(!number(value,65535,&n))goto done;put16(v.header+64,(WORD)n);}
  else {usage();result=64;goto done;}
 }
 o=alloc(sizeof(*o));if(!o||!serialize(&v,cp,o)||!save(argv[2],o,!create)){fwprintf(stderr,L"LNK.EXE: cannot write link (target, encoding, extra blocks or destination)\n");goto done;}result=0;
done:release(o);dispose(&v);return result;
}
