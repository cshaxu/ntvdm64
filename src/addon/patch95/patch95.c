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
#define RAW_IOS_RUNTIME "DD800EF43421051BEE8CC29277B777180A4BA9170B79CE7EBB69558FB85EC6F2"
#define KERNEL_ORIGINAL "8DC06DD276B72A5C69558C649CFEB342398A4B05966D45BBB5CA33D1B8BE8A73"
#define KERNEL_PATCHED "716E9F4E5A2C33F7221F550750FCF46190C8D6C6A6766C3D791EBC897D569C77"
#define SETVER_ORIGINAL "7D8B0765C302358117186D93283AB70D2D0DC72852757AFB4CFEECA69FD74653"
#define SETVER_PATCHED "84DD1839E2A08BC9958411560052DFFD10B99C8BA2580062A564D90EE3A9E145"
static WORD word(const BYTE *p){return (WORD)(p[0]|p[1]<<8);}
static DWORD dword(const BYTE *p){return (DWORD)p[0]|(DWORD)p[1]<<8|(DWORD)p[2]<<16|(DWORD)p[3]<<24;}
static void put32(BYTE *p,DWORD n){unsigned i;for(i=0;i<4;i++){p[i]=(BYTE)n;n>>=8;}}
static BOOL digest(BYTE *p,DWORD size,char out[65]){BCRYPT_ALG_HANDLE a=0;BYTE h[32];unsigned i;BOOL ok=FALSE;if(BCryptOpenAlgorithmProvider(&a,BCRYPT_SHA256_ALGORITHM,0,0)>=0){if(BCryptHash(a,0,0,p,size,h,32)>=0){for(i=0;i<32;i++)sprintf_s(out+i*2,65-i*2,"%02X",h[i]);ok=TRUE;}BCryptCloseAlgorithmProvider(a,0);}return ok;}
typedef struct {const BYTE *p;DWORD bytes,bit;BOOL failed;} Bits;
static DWORD bits(Bits *b,unsigned n){DWORD v=0;unsigned i;if(n>16||b->bit+n>b->bytes*8){b->failed=TRUE;return 0;}for(i=0;i<n;i++,b->bit++)v|=((b->p[b->bit/8]>>(b->bit%8))&1u)<<i;return v;}
static BOOL decode(const BYTE *p,DWORD bytes,BYTE *out,DWORD count){Bits b={p,bytes,0,FALSE};DWORD used=0,tag,back,z,n,i;while(used<count&&!b.failed){tag=bits(&b,2);if(tag==1||tag==2){out[used++]=(BYTE)(bits(&b,7)|(tag==1?128:0));continue;}if(!tag){back=bits(&b,6);if(!back)break;}else if(!bits(&b,1))back=bits(&b,8)+64;else{back=bits(&b,12)+320;if(back==4415)continue;}z=0;while(!b.failed&&!bits(&b,1)){if(++z>=9)return FALSE;}n=(1u<<z)+bits(&b,z)+1;if(!back||back>used||n>count-used)return FALSE;for(i=0;i<n;i++){out[used]=out[used-back];used++;}}return !b.failed&&used==count;}
/* Bounded DS encoder for modified pages; unchanged input blocks stay intact. */
typedef struct {BYTE *p;DWORD size,bit;BOOL failed;} Writer;
static void putbits(Writer *w,DWORD v,unsigned n){unsigned i;for(i=0;i<n;i++,w->bit++){if(w->bit>=w->size*8){w->failed=TRUE;return;}if(v&(1u<<i))w->p[w->bit/8]|=(BYTE)(1u<<(w->bit%8));}}
static DWORD encode(const BYTE *in,BYTE *out,DWORD capacity){Writer w={out,capacity,0,FALSE};DWORD pos=0,back,len,best,dist,limit,z,x,cost,seen;BYTE check[8192];ZeroMemory(out,capacity);
 while(pos<8192&&!w.failed){limit=512-(pos%512);best=dist=seen=0;for(back=1;back<=pos&&back<=4414&&seen<64;back++){if(in[pos]!=in[pos-back]||limit<2||in[pos+1]!=in[pos-back+1])continue;seen++;len=2;while(len<limit&&in[pos+len]==in[pos-back+len])len++;x=len-1;z=0;while(x>>=1)z++;cost=(back<64?8:back<320?11:15)+2*z+1;if(len>best&&cost<len*9){best=len;dist=back;}}
 if(best){if(dist<64){putbits(&w,0,2);putbits(&w,dist,6);}else if(dist<320){putbits(&w,3,3);putbits(&w,dist-64,8);}else{putbits(&w,7,3);putbits(&w,dist-320,12);}x=best-1;z=0;while(x>>=1)z++;putbits(&w,0,z);putbits(&w,1,1);putbits(&w,best-1-(1u<<z),z);pos+=best;}else{putbits(&w,(in[pos]&128)?1:2,2);putbits(&w,in[pos]&127,7);pos++;}if(pos%512==0){putbits(&w,7,3);putbits(&w,4095,12);}}
 putbits(&w,0,8);len=(w.bit+7)/8;if(w.failed||len>=8192||!decode(out,len,check,8192)||memcmp(check,in,8192))return 0;return len;}
static BYTE *unpack(BYTE *input,DWORD size,DWORD *total,DWORD *start,DWORD *count){DWORD at,i,first,last;BYTE *raw;
 if(size<64||memcmp(input,"MZ",2))return 0;at=dword(input+60);if(at<64||at>size-16||memcmp(input+at,"W4",2)||word(input+at+4)!=8192||memcmp(input+at+8,"DS",2))return 0;
 *count=word(input+at+6);if(!*count||*count>1024||at+16+*count*4>size)return 0;*total=at+*count*8192;*start=at;raw=HeapAlloc(GetProcessHeap(),0,*total);if(!raw)return 0;memcpy(raw,input,at);
 for(i=0;i<*count;i++){first=dword(input+at+16+i*4);last=i+1<*count?dword(input+at+20+i*4):size;if(first<at+16+*count*4||last<=first||last>size||last-first>8192)goto bad;if(last-first==8192)memcpy(raw+at+i*8192,input+first,8192);else if(!decode(input+first,last-first,raw+at+i*8192,8192))goto bad;}
 return raw;
bad:HeapFree(GetProcessHeap(),0,raw);return 0;
}
static BOOL changes(BYTE *raw,DWORD size,const struct Delta *set,unsigned count){unsigned i,j,n,a,b;for(i=0;i<count;i++){n=(unsigned)strlen(set[i].before)/2;if(strlen(set[i].after)!=n*2||set[i].offset>size||n>size-set[i].offset)return FALSE;for(j=0;j<n;j++){if(sscanf_s(set[i].before+j*2,"%2x",&a)!=1||sscanf_s(set[i].after+j*2,"%2x",&b)!=1||raw[set[i].offset+j]!=a)return FALSE;raw[set[i].offset+j]=(BYTE)b;}}return TRUE;}
static int prepare_kernel(const wchar_t *source,const wchar_t *destination){BYTE *data=0;DWORD size,written;HANDLE f=INVALID_HANDLE_VALUE;char hash[65];int result=1;
 if(!read_all(source,&data,&size)||size<64||!digest(data,size,hash))goto done;
 if(!_stricmp(hash,KERNEL_ORIGINAL)){if(memcmp(data+60,"\x00\x04\x00\x00",4))goto done;data[61]=0;if(!digest(data,size,hash)||_stricmp(hash,KERNEL_PATCHED))goto done;}
 else if(_stricmp(hash,KERNEL_PATCHED))goto done;
 f=CreateFileW(destination,GENERIC_WRITE,0,0,CREATE_NEW,0,0);if(f==INVALID_HANDLE_VALUE)goto done;
 if(!WriteFile(f,data,size,&written,0)||written!=size||!FlushFileBuffers(f)){CloseHandle(f);f=INVALID_HANDLE_VALUE;DeleteFileW(destination);goto done;}result=0;
done:if(f!=INVALID_HANDLE_VALUE)CloseHandle(f);if(data)HeapFree(GetProcessHeap(),0,data);if(result)fwprintf(stderr,L"patch95: unsupported kernel identity or destination exists\n");return result;}
static int prepare_setver(const wchar_t *source,const wchar_t *destination){BYTE *data=0;DWORD size,written;HANDLE f=INVALID_HANDLE_VALUE;char hash[65];int result=1;
 static const BYTE entries[]={7,'W','I','N','.','C','O','M',7,0,9,'V','M','M','3','2','.','V','X','D',7,0};
 if(!read_all(source,&data,&size)||size<0x29a+sizeof(entries)||!digest(data,size,hash))goto done;
 if(!_stricmp(hash,SETVER_ORIGINAL)){memcpy(data+0x29a,entries,sizeof(entries));if(!digest(data,size,hash)||_stricmp(hash,SETVER_PATCHED))goto done;}
 else if(_stricmp(hash,SETVER_PATCHED))goto done;
 f=CreateFileW(destination,GENERIC_WRITE,0,0,CREATE_NEW,0,0);if(f==INVALID_HANDLE_VALUE)goto done;
 if(!WriteFile(f,data,size,&written,0)||written!=size||!FlushFileBuffers(f)){CloseHandle(f);f=INVALID_HANDLE_VALUE;DeleteFileW(destination);goto done;}result=0;
done:if(f!=INVALID_HANDLE_VALUE)CloseHandle(f);if(data)HeapFree(GetProcessHeap(),0,data);if(result)fwprintf(stderr,L"patch95: unsupported SETVER identity or destination exists\n");return result;}
int wmain(int argc,wchar_t **argv){BYTE *input=0,*raw=0,*output=0;DWORD size,total,at,count,head,i,written,cursor,first,last,n;BYTE old[8192];char hash[65];HANDLE file=INVALID_HANDLE_VALUE;int result=1;
 if(argc==4&&!wcscmp(argv[1],L"prepare-kernel"))return prepare_kernel(argv[2],argv[3]);
 if(argc==4&&!wcscmp(argv[1],L"prepare-setver"))return prepare_setver(argv[2],argv[3]);
 if(argc!=4||wcscmp(argv[1],L"prepare")){fwprintf(stderr,L"patch95.EXE prepare INPUT-VMM32 OUTPUT-VMM32\n");return 64;}
 if(!read_all(argv[2],&input,&size)||(raw=unpack(input,size,&total,&at,&count))==0||!digest(raw,total,hash))goto done;
 if(!_stricmp(hash,RAW_ORIGINAL)){if(!changes(raw,total,deltas,ARRAYSIZE(deltas))||!digest(raw,total,hash)||_stricmp(hash,RAW_PATCHED))goto done;}
 if(!_stricmp(hash,RAW_PATCHED)){if(!changes(raw,total,ios_runtime_deltas,ARRAYSIZE(ios_runtime_deltas))||!digest(raw,total,hash)||_stricmp(hash,RAW_IOS_RUNTIME))goto done;}
 else if(_stricmp(hash,RAW_IOS_RUNTIME))goto done;
 head=16+count*4;output=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,total+head);if(!output)goto done;memcpy(output,input,at+16);cursor=at+head;
 for(i=0;i<count;i++){first=dword(input+at+16+i*4);last=i+1<count?dword(input+at+20+i*4):size;n=last-first;if(n==8192)memcpy(old,input+first,8192);else if(!decode(input+first,n,old,8192))goto done;put32(output+at+16+i*4,cursor);if(!memcmp(old,raw+at+i*8192,8192)){memcpy(output+cursor,input+first,n);}else{n=encode(raw+at+i*8192,output+cursor,8192);if(!n)goto done;}cursor+=n;}
 file=CreateFileW(argv[3],GENERIC_WRITE,0,0,CREATE_NEW,0,0);if(file==INVALID_HANDLE_VALUE)goto done;
 if(!WriteFile(file,output,cursor,&written,0)||written!=cursor||!FlushFileBuffers(file)){CloseHandle(file);file=INVALID_HANDLE_VALUE;DeleteFileW(argv[3]);goto done;}
 wprintf(L"PATCHED=1\n");result=0;
done:if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);if(output)HeapFree(GetProcessHeap(),0,output);if(raw)HeapFree(GetProcessHeap(),0,raw);if(input)HeapFree(GetProcessHeap(),0,input);if(result)fwprintf(stderr,L"patch95: unsupported or malformed VMM32; source preserved, output must be fresh\n");return result;
}
