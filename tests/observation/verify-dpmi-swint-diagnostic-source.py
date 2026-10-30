"""Source-body ordering witness; not a substitute for the selected object."""
import hashlib
from pathlib import Path
import re
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
path = root / 'src/mvdm/dpmi32/dpmiint.c'
data = path.read_bytes()
text = data.decode('latin1')
match = re.search(r'BOOL\s+DpmiSwIntHandler\s*\(\s*ULONG\s+IntNumber\s*\)\s*/\*.*?\*/\s*\{', text, re.S)
if not match:
    raise SystemExit('DPMI handler not found')
start, i, depth = match.start(), match.end(), 1
while depth and i < len(text):
    depth += (text[i] == '{') - (text[i] == '}')
    i += 1
if depth:
    raise SystemExit('Unbalanced DPMI handler')
body = text[start:i]
prefix = r'''
#include <stdint.h>
#include <stdio.h>
typedef uint32_t ULONG,DWORD,NTSTATUS; typedef uint16_t WORD,WORD16,*PWORD16;
typedef uint32_t DWORD16,*PDWORD16; typedef unsigned char *PUCHAR;
typedef struct { unsigned CsSelector; unsigned Eip; unsigned Flags; } HANDLER,*PVDM_INTERRUPTHANDLER;
static HANDLER DpmiInterruptHandlers[256];
static unsigned report,target,reflect,frames,flags=0x4321,protected_mode;
#define BOOL int
#define TRUE 1
#define FALSE 0
#define DBG 0
#define DBGTRACE(...) ((void)0)
#define SEGMENT_IS_PRESENT(s) ((s)!=0)
#define EFLAGS_TF_MASK 0x100
#define MSW_PE 1
#define NT_SUCCESS(s) ((s)==0)
#define LOWORD(s) ((s)&0xffff)
static unsigned getAX(void){return 1;} static unsigned getBX(void){return 2;}
static unsigned getCX(void){return 3;} static unsigned getCS(void){return 4;}
static unsigned getEIP(void){return 5;} static unsigned getEAX(void){return 0;}
static unsigned getEBP(void){return 0;} static unsigned getEBX(void){return 0;}
static unsigned getECX(void){return 0;} static unsigned getEDX(void){return 0;}
static unsigned getEFLAGS(void){return flags;} static unsigned getMSW(void){return protected_mode;}
static void setEFLAGS(unsigned v){flags=v;} static void setEAX(unsigned v){(void)v;}
static void setCF(unsigned v){(void)v;} static void setCS(unsigned v){(void)v;}
static void setEIP(unsigned v){(void)v;} static void setESP(unsigned v){(void)v;}
static void setSP(unsigned v){(void)v;} static int Frame32;
static int BuildStackFrame(int n,PUCHAR *p,ULONG *s){(void)n;(void)p;(void)s;frames++;return 0;}
static void ReflectV86Int(unsigned n,unsigned f){(void)n;(void)f;reflect++;}
static NTSTATUS DpmiSetWowLdtEntry(unsigned a,unsigned b,unsigned c){(void)a;(void)b;(void)c;return 0;}
static void mvdm_softpc_report_dpmi_swint(unsigned a,unsigned b,unsigned c,unsigned d,unsigned e,unsigned f){(void)a;(void)b;(void)c;(void)d;(void)e;(void)f;report++;}
static void mvdm_softpc_report_dpmi_swint_target(unsigned a,unsigned b,unsigned c,unsigned d){(void)a;(void)b;(void)c;(void)d;target++;}
'''
suffix = r'''
#define CHECK(e) do {if(!(e)){fprintf(stderr,"line %d: %s\n",__LINE__,#e);return 1;}}while(0)
int main(void){
 CHECK(!DpmiSwIntHandler(0x31)); CHECK(report==1 && target==0 && reflect==0 && frames==0 && flags==0x4321);
 DpmiInterruptHandlers[0x31].CsSelector=0x1234;
 CHECK(DpmiSwIntHandler(0x31)); CHECK(report==2 && target==1 && reflect==1 && frames==0 && flags==(0x4321 & ~0x4100));
 protected_mode=1; CHECK(!DpmiSwIntHandler(0x31)); CHECK(report==3 && target==2 && reflect==1 && frames==1);
 puts("DPMI_SWINT_DIAGNOSTIC_SOURCE_OK"); return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='dpmi-swint-', dir=root / 'build/t473-s11') as temp:
    source, executable = Path(temp)/'fixture.c', Path(temp)/'fixture.exe'
    source.write_text(prefix + body + suffix, encoding='utf8')
    subprocess.run(['gcc', '-std=c11', '-O0', str(source), '-o', str(executable)], check=True)
    subprocess.run([str(executable)], check=True)
print('selected_source_sha256=' + hashlib.sha256(data).hexdigest().upper())
