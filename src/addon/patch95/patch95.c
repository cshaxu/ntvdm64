/* patch95: copied PATCH386 file/hash shell, adapted to the owner-approved
 * Win95 VMM32 consumer bindings. No guest binary is bundled.
 * DS decoder ported from the already attributed read-only W4 reader,
 * JHRobotics/patcher9x pe/ds_decompress.c b6e30d4 (MIT); DS-LICENSE.txt.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <stdio.h>
#include <string.h>
#pragma comment(lib,"bcrypt.lib")
#include "../common/file-helpers.h"
#include "deltas.h"
#define RAW_ORIGINAL "BAE0A8C385DA1E1E5F0DF920E54ED0FCB72EF1255234763B7AA8A0A643AF1C0C"
#define RAW_PATCHED "A1A483BA0E01A11D981801C4D370F676085281ACC49FA8C5706E7FCEE9D165A5"
static WORD word(const BYTE *p){return (WORD)(p[0]|p[1]<<8);}
static DWORD dword(const BYTE *p){return (DWORD)p[0]|(DWORD)p[1]<<8|(DWORD)p[2]<<16|(DWORD)p[3]<<24;}
static void put32(BYTE *p,DWORD n){unsigned i;for(i=0;i<4;i++){p[i]=(BYTE)n;n>>=8;}}
static BOOL digest(BYTE *p,DWORD size,char out[65]){BCRYPT_ALG_HANDLE a=0;BYTE h[32];unsigned i;BOOL ok=FALSE;if(BCryptOpenAlgorithmProvider(&a,BCRYPT_SHA256_ALGORITHM,0,0)>=0){if(BCryptHash(a,0,0,p,size,h,32)>=0){for(i=0;i<32;i++)sprintf_s(out+i*2,65-i*2,"%02X",h[i]);ok=TRUE;}BCryptCloseAlgorithmProvider(a,0);}return ok;}
typedef struct {const BYTE *p;DWORD bytes,bit;BOOL failed;} Bits;
static DWORD bits(Bits *b,unsigned n){DWORD v=0;unsigned i;if(n>16||b->bit+n>b->bytes*8){b->failed=TRUE;return 0;}for(i=0;i<n;i++,b->bit++)v|=((b->p[b->bit/8]>>(b->bit%8))&1u)<<i;return v;}
static BOOL decode(const BYTE *p,DWORD bytes,BYTE *out,DWORD count){Bits b={p,bytes,0,FALSE};DWORD used=0,tag,back,z,n,i;while(used<count&&!b.failed){tag=bits(&b,2);if(tag==1||tag==2){out[used++]=(BYTE)(bits(&b,7)|(tag==1?128:0));continue;}if(!tag){back=bits(&b,6);if(!back)break;}else if(!bits(&b,1))back=bits(&b,8)+64;else{back=bits(&b,12)+320;if(back==4415)continue;}z=0;while(!b.failed&&!bits(&b,1)){if(++z>=9)return FALSE;}n=(1u<<z)+bits(&b,z)+1;if(!back||back>used||n>count-used)return FALSE;for(i=0;i<n;i++){out[used]=out[used-back];used++;}}return !b.failed&&used==count;}
static BYTE *unpack(BYTE *input,DWORD size,DWORD *total,DWORD *start,DWORD *count){DWORD at,i,first,last;BYTE *raw;
 if(size<64||memcmp(input,"MZ",2))return 0;at=dword(input+60);if(at<64||at>size-16||memcmp(input+at,"W4",2)||word(input+at+4)!=8192||memcmp(input+at+8,"DS",2))return 0;
 *count=word(input+at+6);if(!*count||*count>1024||at+16+*count*4>size)return 0;*total=at+*count*8192;*start=at;raw=HeapAlloc(GetProcessHeap(),0,*total);if(!raw)return 0;memcpy(raw,input,at);
 for(i=0;i<*count;i++){first=dword(input+at+16+i*4);last=i+1<*count?dword(input+at+20+i*4):size;if(first<at+16+*count*4||last<=first||last>size)goto bad;if(last-first==8192)memcpy(raw+at+i*8192,input+first,8192);else if(!decode(input+first,last-first,raw+at+i*8192,8192))goto bad;}
 return raw;
bad:HeapFree(GetProcessHeap(),0,raw);return 0;
}
static BOOL changes(BYTE *raw,DWORD size){unsigned i,j,n,a,b;for(i=0;i<ARRAYSIZE(deltas);i++){n=(unsigned)strlen(deltas[i].before)/2;if(strlen(deltas[i].after)!=n*2||deltas[i].offset>size||n>size-deltas[i].offset)return FALSE;for(j=0;j<n;j++){if(sscanf_s(deltas[i].before+j*2,"%2x",&a)!=1||sscanf_s(deltas[i].after+j*2,"%2x",&b)!=1||raw[deltas[i].offset+j]!=a)return FALSE;raw[deltas[i].offset+j]=(BYTE)b;}}return TRUE;}
int wmain(int argc,wchar_t **argv){BYTE *input=0,*raw=0,*output=0;DWORD size,total,at,count,head,i,written;char hash[65];HANDLE file=INVALID_HANDLE_VALUE;int result=1;
 if(argc!=4||wcscmp(argv[1],L"prepare")){fwprintf(stderr,L"patch95.EXE prepare INPUT-VMM32 OUTPUT-VMM32\n");return 64;}
 if(!read_all(argv[2],&input,&size)||(raw=unpack(input,size,&total,&at,&count))==0||!digest(raw,total,hash))goto done;
 if(!_stricmp(hash,RAW_ORIGINAL)){if(!changes(raw,total)||!digest(raw,total,hash)||_stricmp(hash,RAW_PATCHED))goto done;}
 else if(_stricmp(hash,RAW_PATCHED))goto done;
 head=16+count*4;output=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,total+head);if(!output)goto done;memcpy(output,input,at+16);for(i=0;i<count;i++)put32(output+at+16+i*4,at+head+i*8192);memcpy(output+at+head,raw+at,total-at);
 file=CreateFileW(argv[3],GENERIC_WRITE,0,0,CREATE_NEW,0,0);if(file==INVALID_HANDLE_VALUE)goto done;
 if(!WriteFile(file,output,total+head,&written,0)||written!=total+head||!FlushFileBuffers(file)){CloseHandle(file);file=INVALID_HANDLE_VALUE;DeleteFileW(argv[3]);goto done;}
 wprintf(L"PATCHED=1\n");result=0;
done:if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);if(output)HeapFree(GetProcessHeap(),0,output);if(raw)HeapFree(GetProcessHeap(),0,raw);if(input)HeapFree(GetProcessHeap(),0,input);if(result)fwprintf(stderr,L"patch95: unsupported or malformed VMM32; source preserved, output must be fresh\n");return result;
}
