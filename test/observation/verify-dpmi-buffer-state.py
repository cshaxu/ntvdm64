"""Bounded x86 fixture of original DPMI buffer/save bodies; not guest proof."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('--build-root', required=True)
args = parser.parse_args()
repo = Path(__file__).resolve().parents[2]
out = (repo / args.build_root).resolve()
if not out.is_relative_to(repo / 'build') or out.exists():
    raise SystemExit('A fresh repository build directory is required')
out.mkdir(parents=True)
root = repo / 'src/mvdm-host/dpmi32'
buffer = (root / 'buffer.c').read_text(encoding='utf-8')
state = (root / 'savestat.c').read_text(encoding='utf-8')
origins = [Path('O:/repos.external/OpenNT/base/mvdm/dpmi32'),
           Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/dpmi32')]

def normalize(text):
    text = re.sub(r'/\*.*?\*/|//[^\n]*', '', text, flags=re.S)
    text = re.sub(r'^\s*#line[^\n]*', '', text, flags=re.M)
    text = re.sub(r'^\s*#if 0\b.*?^\s*#endif[^\n]*', '', text,
                  flags=re.M | re.S)
    return re.sub(r'\s+', '', text)

identities = {}
for name in ('buffer.c', 'savestat.c'):
    paths = [root / name] + [p / name for p in origins]
    identities[name] = [dict(path=str(p), sha256=hashlib.sha256(p.read_bytes()).hexdigest())
                        for p in paths]
    for original in paths[1:]:
        assert normalize(paths[0].read_text(encoding='utf-8')) == normalize(
            original.read_text(encoding='utf-8')), name

# SegmentToSelector invokes the actual guest executor and is deliberately not
# replaced with a fake successful execution in this controlled-body fixture.
buffer, removed = re.subn(r'USHORT\s+DpmiSegmentToSelector\(.*?(?=PUCHAR\s+DpmiAllocateBuffer\()',
                          '', buffer, flags=re.S)
assert removed == 1
body = re.sub(r'^\s*#include[^\n]*|^\s*#pragma hdrstop[^\n]*', '',
              buffer + '\n' + state, flags=re.M)
prefix = r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define ASSERT assert
#define SMALL_XLAT_BUFFER_SIZE 128
#define LARGE_XLAT_BUFFER_SIZE 8192
#define MAX_V86_ADDRESS 0x100000
typedef USHORT *PWORD16;
typedef struct { UCHAR Misc[28]; } VSAVEDSTATE;
static PUCHAR IntelBase, SmallXlatBuffer, LargeXlatBuffer, mapped;
static ULONG LargeBufferInUseCount, selectorLimit;
static BOOL SmallBufferInUse;
static USHORT regs[5];
static ULONG esp;
#define getSS() regs[0]
#define getDS() regs[1]
#define getES() regs[2]
#define getFS() regs[3]
#define getGS() regs[4]
#define getESP() esp
#define setSS(v) (regs[0]=(v))
#define setDS(v) (regs[1]=(v))
#define setES(v) (regs[2]=(v))
#define setFS(v) (regs[3]=(v))
#define setGS(v) (regs[4]=(v))
#define setESP(v) (esp=(v))
#define GET_SELECTOR_LIMIT(seg, limit) ((limit)=selectorLimit)
static PUCHAR Sim32GetVDMPointer(ULONG p, ULONG n, BOOL pm)
{ (void)p; (void)n; (void)pm; return mapped; }
PUCHAR DpmiAllocateBuffer(USHORT);
VOID DpmiFreeBuffer(PUCHAR, USHORT);
'''
# Exhaustion is an original deliberate assertion/sentinel; the fixture records
# the sentinel without dereferencing it, and does not call this a safe failure.
prefix = prefix.replace('#define ASSERT assert', '#define ASSERT(x) ((void)0)')
main = r'''
static unsigned checks;
#define CHECK(x) do { ++checks; if(!(x)) { printf("FAIL line=%d\n",__LINE__); return 1; } } while(0)
int main(void) {
    PUCHAR a,b,c,high;
    USHORT length;
    unsigned i,j;
    union { ULONG align; VSAVEDSTATE value; } saved[16];
    CHECK(sizeof(void*)==4);
    CHECK(sizeof(SAVEDCONTEXT)<sizeof(VSAVEDSTATE));
    IntelBase=calloc(1,0x200000);
    CHECK(IntelBase!=NULL);
    SmallXlatBuffer=IntelBase+0x1000; LargeXlatBuffer=IntelBase+0x2000;
    high=IntelBase+0x110000;
    for(i=0;i<8192;i++) high[i]=(UCHAR)(i*17);
    a=DpmiMapAndCopyBuffer(IntelBase+0x9000,128);
    CHECK(a==IntelBase+0x9000 && !SmallBufferInUse && !LargeBufferInUseCount);
    DpmiUnmapAndCopyBuffer(a,a,128);
    a=DpmiMapAndCopyBuffer(high,128);
    CHECK(a==SmallXlatBuffer && SmallBufferInUse && !memcmp(a,high,128));
    b=DpmiAllocateBuffer(128); CHECK(b==LargeXlatBuffer && LargeBufferInUseCount==128);
    c=DpmiAllocateBuffer(4096); CHECK(c==LargeXlatBuffer+128 && LargeBufferInUseCount==4224);
    DpmiFreeBuffer(c,4096); DpmiFreeBuffer(b,128);
    CHECK(LargeBufferInUseCount==0);
    a[7]=0x81; DpmiUnmapAndCopyBuffer(high,a,128);
    CHECK(high[7]==0x81 && !SmallBufferInUse);
    a=DpmiMapAndCopyBuffer(high,4096);
    CHECK(a==LargeXlatBuffer && !memcmp(a,high,4096));
    DpmiUnmapBuffer(a,4096); CHECK(!LargeBufferInUseCount);
    a=DpmiAllocateBuffer(8192); CHECK(a==LargeXlatBuffer && LargeBufferInUseCount==8192);
    b=DpmiAllocateBuffer(128); CHECK(b==SmallXlatBuffer);
    c=DpmiAllocateBuffer(1); CHECK(c==(PUCHAR)0xf00df00d);
    DpmiFreeAllBuffers(); CHECK(!SmallBufferInUse && !LargeBufferInUseCount);
    CHECK(DpmiCalcFcbLength((PUCHAR)"x")==0x25);
    CHECK(DpmiCalcFcbLength((PUCHAR)"\xff")==0x2c);
    mapped=high; selectorLimit=255; memset(high,0,256); memcpy(high,"ABC",3);
    a=DpmiMapString(8,0,&length);
    CHECK(length==8 && !memcmp(a,high,length));
    DpmiUnmapString(a,length); CHECK(!SmallBufferInUse);
    a=DpmiMapAndCopyBuffer(IntelBase+MAX_V86_ADDRESS-128,128);
    CHECK(a==SmallXlatBuffer); DpmiUnmapBuffer(a,128);
    for(i=0;i<16;i++) {
        for(j=0;j<5;j++) regs[j]=(USHORT)(i*8+j);
        esp=0x12340000+i;
        DpmiSaveSegmentsAndStack(&saved[i].value);
    }
    for(i=16;i--;) {
        memset(regs,0xff,sizeof regs); esp=0;
        CHECK(DpmiRestoreSegmentsAndStack()==&saved[i].value);
        CHECK(esp==0x12340000+i);
        for(j=0;j<5;j++) CHECK(regs[j]==i*8+j);
    }
    CHECK(StateStack==NULL);
    free(IntelBase);
    printf("PASS %u controlled original buffer/state checks; exhaustion sentinel retained\n", checks);
    return 0;
}
'''
(out / 'fixture.c').write_text(prefix + body + main, encoding='utf-8')
driver = repo / 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
command = [str(driver), 'cl', '/nologo', '/W3', '/MT', '/TC', '/Od',
           str(out / 'fixture.c'), '/Fo' + str(out / 'fixture.obj'),
           '/Fe' + str(out / 'fixture.exe')]
with (out / 'build.txt').open('w', encoding='utf-8') as log:
    subprocess.run(command, cwd=repo, stdout=log, stderr=subprocess.STDOUT,
                   timeout=90, check=True)
result = subprocess.run([str(out / 'fixture.exe')], capture_output=True,
                        text=True, timeout=15)
(out / 'result.txt').write_text(result.stdout + result.stderr, encoding='utf-8')
result.check_returncode()
(out / 'identity.json').write_text(json.dumps(identities, indent=2), encoding='utf-8')
print(result.stdout, end='')
