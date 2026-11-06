/* Exact original thunk bodies with native GDI and explicit guest-copy seams. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef void *PVDMFRAME;
typedef DWORD VPVOID, VPSTR;
typedef WORD HDC16;
#include "font_types.h"
static HDC dc;
static unsigned checks, failures, allocations;
static int fail_kerning;
static DWORD native_pairs(HDC h,DWORD n,LPKERNINGPAIR p){return fail_kerning?0:GetKerningPairsA(h,n,p);}
static DWORD copied, pairs;
static void check(int ok, const char *name) { ++checks; if(!ok){++failures;printf("FAIL %s\n",name);} }
static void *allocate(size_t n){void *p=malloc(n);if(p){memset(p,0xcd,n);++allocations;}return p;}
static void release(void *p){if(p){--allocations;free(p);}}
static void copy_buffer(DWORD p,DWORD n,const void *b){copied=n;memcpy((void*)p,b,n);}
static void copy_pairs(DWORD p,DWORD n,const KERNINGPAIR *b){pairs=n;memcpy((void*)p,b,n*sizeof(*b));}
#define FASTCALL
#define GETARGPTR(f,s,p) ((p)=(void*)(f))
#define FREEARGPTR(p) ((void)0)
#define RETURN(v) return(v)
#define DWORD32(v) (v)
#define WORD32(v) (v)
#define FETCHDWORD(v) (v)
#define GETDWORD16(v) (v)
#define HDC32(v) dc
#define malloc_w allocate
#define free_w release
#define PUTBUFFER16 copy_buffer
#define putkerningpairs16 copy_pairs
#define GETMAT2(p,m) memcpy(m,(void*)(p),sizeof(MAT2))
#define PUTGLYPHMETRICS16(p,m) memcpy((void*)(p),m,sizeof(GLYPHMETRICS))
#define GetGlyphOutlineWow GetGlyphOutlineA
#undef GetKerningPairs
#define GetKerningPairs native_pairs
#define LOGDEBUG(a,b) ((void)0)
#include "font_exact.h"
int main(void){
 HFONT font,old;MAT2 m={{0,1},{0,0},{0,0},{0,1}};GLYPHMETRICS gm,nativegm;
 BYTE guest[65536],native[65536];DWORD r,expected,n,i;
 GETGLYPHOUTLINE16 g={0};GETFONTDATA16 f={0};GETKERNINGPAIRS16 k={0};
 dc=CreateCompatibleDC(NULL);font=CreateFontA(24,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,0,0,0,"Arial");old=SelectObject(dc,font);
 check(dc&&font&&old,"native font/DC");
 g.f7=(DWORD)&m;g.f6=(DWORD)guest;g.f5=sizeof(guest);g.f4=(DWORD)&gm;g.f2='A';g.f3=GGO_METRICS;
 memset(native,0xa5,sizeof(native));expected=GetGlyphOutlineA(dc,'A',GGO_METRICS,&nativegm,sizeof(native),native,&m);
 check(expected!=GDI_ERROR,"metrics native success");printf("NATIVE metrics return=%lu first=%02x last=%02x\n",expected,native[0],native[65535]);
 memset(guest,0xa5,sizeof(guest));copied=0;r=WG32GetGlyphOutline(&g);
 check(r==expected,"metrics return");check(!memcmp(&gm,&nativegm,sizeof(gm)),"metrics result");check(!memcmp(guest,native,sizeof(guest)),"metrics preserves selected native output");
 g.f3=GGO_BITMAP;memset(native,0xa5,sizeof(native));expected=GetGlyphOutlineA(dc,'A',GGO_BITMAP,&nativegm,sizeof(native),native,&m);
 printf("NATIVE bitmap return=%lu tail=%02x last=%02x\n",expected,native[expected],native[65535]);check(expected>0&&expected<sizeof(native),"bitmap native bounded success");memset(guest,0xa5,sizeof(guest));copied=0;r=WG32GetGlyphOutline(&g);
 check(r==expected,"bitmap return");check(!memcmp(guest,native,expected),"bitmap bytes");check(!memcmp(guest,native,sizeof(guest)),"bitmap preserves selected native output");
 n=GetKerningPairsA(dc,0,NULL);check(n>0&&n+3<65536/sizeof(KERNINGPAIR),"native kerning fixture count");
 if(n>0&&n+3<65536/sizeof(KERNINGPAIR)){
 k.f2=(WORD)(n+3);k.f3=(DWORD)guest;memset(native,0xa5,sizeof(native));expected=GetKerningPairsA(dc,n+3,(KERNINGPAIR*)native);
 printf("NATIVE kerning return=%lu tail=%02x last=%02x\n",expected,native[expected*sizeof(KERNINGPAIR)],native[65535]);memset(guest,0xa5,sizeof(guest));pairs=0;r=WG32GetKerningPairs(&k);
 check(r==expected&&r==n,"kerning short return");check(!memcmp(guest,native,n*sizeof(KERNINGPAIR)),"kerning values");check(pairs==n&&guest[n*sizeof(KERNINGPAIR)]==0xa5,"kerning tail untouched");}
 fail_kerning=1;k.f3=(DWORD)guest;k.f2=3;memset(guest,0xa5,sizeof(guest));pairs=0;check(WG32GetKerningPairs(&k)==0,"controlled kerning failure return");check(pairs==0&&guest[0]==0xa5,"no unavailable kerning output");fail_kerning=0;
 f.f4=(DWORD)guest;f.f5=32;memset(native,0xa5,sizeof(native));expected=GetFontData(dc,0,0,native,32);memset(guest,0xa5,sizeof(guest));r=WG32GetFontData(&f);
 check(expected==32&&r==expected&&!memcmp(guest,native,32),"font data success");
 f.f4=0;f.f5=0;r=WG32GetFontData(&f);check(r==GetFontData(dc,0,0,NULL,0)&&r!=GDI_ERROR,"font size query");
 g.f5=0;g.f6=0;r=WG32GetGlyphOutline(&g);check(r==GetGlyphOutlineA(dc,'A',GGO_BITMAP,&nativegm,0,NULL,&m),"glyph size query");
 k.f3=0;k.f2=0;check(WG32GetKerningPairs(&k)==n,"kerning size query");
 for(i=0;i<1;++i)check(allocations==0,"temporary allocations retired");
 SelectObject(dc,old);DeleteObject(font);DeleteDC(dc);
 printf("FONT_OUTPUT checks=%u failures=%u\n",checks,failures);return failures?1:0;
}
