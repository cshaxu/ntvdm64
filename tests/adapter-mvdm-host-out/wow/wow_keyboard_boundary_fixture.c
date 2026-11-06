/* Exact original thunk bodies; controlled provider responses, not layout tests. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
typedef DWORD VPSTR, VPVOID;
#include "wowkbd.h"
typedef void *PVDMFRAME;
#define FASTCALL
static BYTE state[256], output[6];
static int expected_return, failures, flush_count;
static ULONG expected_map;
#define CHECK(c) do {if(!(c)){printf("failure line=%d\n",__LINE__);failures++;}} while(0)
#define GETARGPTR(frame,size,p) ((p)=(void *)(frame))
#define GETPSZPTR(vp,p) do {CHECK((vp)==1);(p)=(void *)state;} while(0)
#define GETMISCPTR(vp,p) do {CHECK((vp)==2);(p)=(void *)output;} while(0)
#define FREEPSZPTR(p) ((void)0)
#define FREEMISCPTR(p) ((void)0)
#define FREEARGPTR(p) ((void)0)
#define FLUSHVDMPTR(vp,cb,p) do {CHECK((vp)==2 && (cb)==4 && (p)==output);flush_count++;} while(0)
#define RETURN(v) return (v)
static int observed_ascii(WORD key, WORD scan, const void *keys, void *out, WORD flags)
{
 CHECK(key==0x41 && scan==0x1e && keys==state && out==output && flags==1);
 CHECK(state[0]==0 && state[255]==0x80);
 if(expected_return){output[0]=0x82;output[1]=0xa0;}
 return expected_return;
}
static UINT observed_map(UINT key, UINT kind)
{CHECK(key==0x41 && kind==2);return expected_map;}
#undef MapVirtualKey
#define MapVirtualKey observed_map
#define ToAscii observed_ascii
#include "selected-keyboard.inc"
int main(void)
{
 TOASCII16 a={1,2,1,0x1e,0x41};
 MAPVIRTUALKEY16 m={2,0x41};
 int i;int returns[]={-1,0,1,2};
 ULONG maps[]={0,0x41,0x80000041};
 state[255]=0x80;
 CHECK(sizeof(a)==14 && sizeof(m)==4);
 for(i=0;i<4;i++){
  memset(output,0xa5,sizeof(output));expected_return=returns[i];
  CHECK(WKB32ToAscii(&a)==(ULONG)expected_return);
  CHECK(output[4]==0xa5 && output[5]==0xa5);
  CHECK(output[0]==(expected_return ? 0x82 : 0xa5));
  CHECK(output[1]==(expected_return ? 0xa0 : 0xa5));
 }
 CHECK(flush_count==4);
 for(i=0;i<3;i++){
  expected_map=maps[i];
  CHECK(WKB32MapVirtualKey(&m)==(maps[i]|((maps[i]&0x80000000)?0x8000:0)));
 }
 printf("cases=7 failures=%d; controlled dead-key/DBCS result preservation only\n",failures);
 return failures ? 1 : 0;
}
