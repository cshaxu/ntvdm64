/* path95: standalone bounded CREG/INI relocation.
 * CREG layout: libyal/libcreg documentation a9232da; independently authored,
 * no libcreg implementation. Source and checksum findings in README.md.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../common/file-helpers.h"
typedef struct {BYTE *p;DWORD n,capacity;} Buffer;
static WORD u16(const BYTE *p){return (WORD)(p[0]|p[1]<<8);}
static DWORD u32(const BYTE *p){return (DWORD)p[0]|(DWORD)p[1]<<8|(DWORD)p[2]<<16|(DWORD)p[3]<<24;}
static void p16(BYTE *p,DWORD n){p[0]=(BYTE)n;p[1]=(BYTE)(n>>8);}
static void p32(BYTE *p,DWORD n){unsigned i;for(i=0;i<4;i++){p[i]=(BYTE)n;n>>=8;}}
static BOOL append(Buffer *b,const void *p,DWORD n){BYTE *q;DWORD size;if(n>16777216-b->n)return FALSE;if(b->n+n>b->capacity){size=b->n+n+65536;q=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,size);if(!q)return FALSE;if(b->p){memcpy(q,b->p,b->n);HeapFree(GetProcessHeap(),0,b->p);}b->p=q;b->capacity=size;}if(n)memcpy(b->p+b->n,p,n);b->n+=n;return TRUE;}
static void drop(Buffer *b){if(b->p)HeapFree(GetProcessHeap(),0,b->p);b->p=0;b->n=b->capacity=0;}
static BOOL check(const BYTE *p,DWORD n,DWORD flags){DWORD sum=0,i;if(!(u32(p+flags)&4))return TRUE;if(n%4)return FALSE;for(i=0;i<n;i+=4)sum+=u32(p+i);return sum==0;}
static void checksum(BYTE *p,DWORD n,DWORD flags,DWORD field){DWORD sum=0,i;if(!(u32(p+flags)&4))return;p32(p+field,0);for(i=0;i<n;i+=4)sum+=u32(p+i);p32(p+field,0-sum);}
static BOOL boundary(BYTE c){return !c||c=='\\'||c=='/'||c==' '||c=='\t'||c=='"'||c=='\''||c==','||c==';'||c==')';}
static BOOL substitute(const BYTE *data,DWORD n,const char *from,const char *to,Buffer *out,DWORD *changed){DWORD i=0,f=(DWORD)strlen(from),t=(DWORD)strlen(to);if(!f)return FALSE;while(i<n){if(n-i>=f&&!_strnicmp((const char*)data+i,from,f)&&(i+f==n||boundary(data[i+f]))){if(!append(out,to,t))return FALSE;i+=f;(*changed)++;}else{if(!append(out,data+i,1))return FALSE;i++;}}return TRUE;}
static char foundRoot[128];
static BOOL registry(const BYTE *data,DWORD size,const char *from,const char *to,Buffer *out,DWORD *changed){DWORD at,nav,count,block,sizeBlock,entry,entrySize,used,keyStart,value,valueEnd,k,nameBytes,valueName,valueBytes,type,oldAt,newSize,pad;Buffer b={0},key={0},str={0};BOOL ok=FALSE,modified;BYTE zeros[4]={0};
 if(size<64||memcmp(data,"CREG",4)||u16(data+4)||u16(data+6)!=1||memcmp(data+32,"RGKN",4))return FALSE;
 nav=u32(data+36);at=u32(data+8);count=u16(data+16);if(nav<32||nav>size-32||at!=32+nav||!check(data,32,18)||!check(data+32,nav,16)||!append(out,data,at))return FALSE;
 for(block=0;block<count;block++){
  if(at>size-32||memcmp(data+at,"RGDB",4))goto done;sizeBlock=u32(data+at+4);if(sizeBlock<32||sizeBlock>size-at||u16(data+at+14)!=block||!check(data+at,sizeBlock,12))goto done;
  drop(&b);if(!append(&b,data+at,32))goto done;entry=at+32;
  while(entry<at+sizeBlock){
   if(at+sizeBlock-entry<4)goto done;entrySize=u32(data+entry);if(!entrySize){for(k=entry;k<at+sizeBlock;k++)if(data[k])goto done;break;}
   if(entrySize<8||entrySize>at+sizeBlock-entry)goto done;
   if(u16(data+entry+4)==65535){if(!append(&b,data+entry,entrySize))goto done;entry+=entrySize;continue;}
   if(entrySize<20)goto done;nameBytes=u16(data+entry+12);if(nameBytes>entrySize-20)goto done;
   drop(&key);if(!append(&key,data+entry,20+nameBytes))goto done;value=entry+20+nameBytes;modified=FALSE;
   for(k=0;k<u16(data+entry+14);k++){
    if(value>entry+entrySize-12)goto done;type=u32(data+value);valueName=u16(data+value+8);valueBytes=u16(data+value+10);valueEnd=value+12+valueName+valueBytes;if(valueEnd>entry+entrySize)goto done;
    if(type==1&&valueName==10&&!_strnicmp((const char*)data+value+12,"SystemRoot",10)){
     if(!valueBytes||valueBytes>=sizeof(foundRoot))goto done;
     {char root[128];memcpy(root,data+value+22,valueBytes);root[valueBytes]=0;if(foundRoot[0]&&_stricmp(foundRoot,root))goto done;strcpy_s(foundRoot,sizeof(foundRoot),root);}
    }
    drop(&str);oldAt=*changed;
    if(type==1&&from){if(!substitute(data+value+12+valueName,valueBytes,from,to,&str,changed)||str.n>65535)goto done;}
    else if(!append(&str,data+value+12+valueName,valueBytes))goto done;
    keyStart=key.n;if(!append(&key,data+value,12+valueName)||!append(&key,str.p,str.n))goto done;p16(key.p+keyStart+10,str.n);if(*changed!=oldAt)modified=TRUE;value=valueEnd;
   }
   if(!modified){if(!append(&b,data+entry,entrySize))goto done;}
   else{used=key.n;pad=(4-key.n%4)%4;if(!append(&key,zeros,pad))goto done;p32(key.p,key.n);p32(key.p+8,used);if(!append(&b,key.p,key.n))goto done;}
   entry+=entrySize;
  }
  used=b.n;newSize=used>sizeBlock?(used+4095)&~4095u:sizeBlock;if(newSize>65536)goto done;
  while(b.n<newSize)if(!append(&b,zeros,newSize-b.n>4?4:newSize-b.n))goto done;
  p32(b.p+4,newSize);p32(b.p+8,newSize-used);if(u32(b.p+16)!=0xffffffff)p32(b.p+16,used);
  checksum(b.p,newSize,12,28);if(!append(out,b.p,newSize))goto done;at+=sizeBlock;
 }
 if(at!=size)goto done;ok=TRUE;
done:drop(&b);drop(&key);drop(&str);return ok;
}
int wmain(int argc,wchar_t **argv){BYTE *data=0;DWORD size,changed=0,written;Buffer out={0};HANDLE file=INVALID_HANDLE_VALUE;char from[128],to[128];int result=1;BOOL hive;
 if(argc!=3&&argc!=6){fwprintf(stderr,L"path95.EXE root SYSTEM.DAT\npath95.EXE replace INPUT OUTPUT FROM TO\n");return 64;}
 if(!read_all(argv[2],&data,&size))goto done;hive=size>=4&&!memcmp(data,"CREG",4);
 if(argc==3&&!wcscmp(argv[1],L"root")){if(hive&&registry(data,size,0,0,&out,&changed)&&foundRoot[0]){printf("OLD_ROOT=%s\n",foundRoot);result=0;}goto done;}
 if(argc!=6||wcscmp(argv[1],L"replace")||!WideCharToMultiByte(20127,WC_NO_BEST_FIT_CHARS,argv[4],-1,from,sizeof(from),0,0)||!WideCharToMultiByte(20127,WC_NO_BEST_FIT_CHARS,argv[5],-1,to,sizeof(to),0,0))goto done;
 if(hive){if(!registry(data,size,from,to,&out,&changed))goto done;}else if(!substitute(data,size,from,to,&out,&changed))goto done;
 /* A no-op must preserve every byte, including original free-list metadata. */
 if(!changed){drop(&out);if(!append(&out,data,size))goto done;}
 file=CreateFileW(argv[3],GENERIC_WRITE,0,0,CREATE_NEW,0,0);if(file==INVALID_HANDLE_VALUE)goto done;if(!WriteFile(file,out.p,out.n,&written,0)||written!=out.n||!FlushFileBuffers(file)){CloseHandle(file);file=INVALID_HANDLE_VALUE;DeleteFileW(argv[3]);goto done;}
 printf("CHANGED=%lu\n",(unsigned long)changed);result=0;
done:if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);drop(&out);if(data)HeapFree(GetProcessHeap(),0,data);if(result)fwprintf(stderr,L"path95: invalid/unsupported input, ambiguous root or existing output; original preserved\n");return result;
}
