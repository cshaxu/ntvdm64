"""Compile the selected original mouse callback bodies against a stack fixture."""
from pathlib import Path
import subprocess
import sys
import argparse

root = Path(__file__).resolve().parents[2]
source_path = 'src/mvdm-host/softpc.new/base/keymouse/mouse_io.c'
source = (root / source_path).read_text(encoding='latin1')
out = root / 'build/t481-s9/mouse-callback-regression'
out.mkdir(parents=True, exist_ok=True)
def function(signature):
    start = source.index(signature)
    brace = source.index('{', start)
    depth = 0
    for i in range(brace, len(source)):
        if source[i] == '{': depth += 1
        elif source[i] == '}':
            depth -= 1
            if depth == 0: return source[start:i+1]
    raise ValueError(signature)
candidate_functions = '\n'.join(function(x) for x in (
    'LOCAL void mouse_callback_push IFN1', 'LOCAL word mouse_callback_pop IFN0',
    'void mouse_int2()', 'LOCAL void jump_to_user_subroutine IFN3'))
parser=argparse.ArgumentParser()
parser.add_argument('--baseline', type=Path, help='optional pre-fix mouse_io.c for a negative control')
args=parser.parse_args()
baseline_functions=None
if args.baseline:
    source=args.baseline.read_text(encoding='latin1')
    baseline_functions='\n'.join(function(x) for x in (
        'void mouse_int2()', 'LOCAL void jump_to_user_subroutine IFN3'))
preamble = r'''
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uint16_t word; typedef word MOUSE_CALL_MASK;
#define LOCAL static
#define IFN0() (void)
#define IFN1(a,b) (a b)
#define IFN3(a,b,c,d,e,f) (a b,c d,e f)
#define NTVDM 1
#define CPU_40_STYLE 1
/* Original host/inc/host_def.h callback adjustment for the negative control. */
#define HOST_BOP_IP_FUDGE (-2)
#define MOUSE_VERBOSE 0
#define note_trace0(a,b) ((void)0)
#define MOUSE_INT2_SEGMENT 0xf000
#define MOUSE_INT2_OFFSET 0x100
#define MOUSE_CALL_MASK_POSITION_BIT 1
#define MOUSE_CURSOR_DISPLAYED 1
static word reg[12]; enum { AX,BX,CX,DX,SI,DI,ES,BP,DS,SS,SP,CS };
#define REG(n) static word get##n(void){return reg[n];} static void set##n(word v){reg[n]=v;}
REG(AX) REG(BX) REG(CX) REG(DX) REG(SI) REG(DI) REG(ES) REG(BP) REG(DS) REG(SP)
static word getSS(void){return reg[SS];} static void setCS(word v){reg[CS]=v;}
static void setIP(word v){(void)v;} static void setIF(word v){(void)v;}
static unsigned char mem[65536];
static unsigned effective_addr(word seg,word off){(void)seg;return off;}
static void sas_storew(unsigned a,word v){mem[a]=v;mem[(a+1)&65535]=v>>8;}
static void sas_loadw(unsigned a,word *v){*v=(word)(mem[a]|mem[(a+1)&65535]<<8);}
static struct { struct {word x,y;} position; word button_status;} cursor_status;
static struct {word x,y;} mouse_motion;
static int cursor_flag,draws,flushes,resumes;
static word saved_AX,saved_BX,saved_CX,saved_DX,saved_SI,saved_DI,saved_ES,saved_BP,saved_DS,last_condition_mask;
static void cursor_display(void){draws++;} static void host_flush_screen(void){flushes++;}
static void ResumeMouseInterrupts(void){resumes++;}
'''
main = r'''
static void die(const char *msg){puts(msg);exit(1);}
#include <stdlib.h>
static void verify(const word *expected,const char *where){
 int i; for(i=AX;i<=DS;i++)if(reg[i]!=expected[i]){printf("%s: reg %d expected %04x got %04x\n",where,i,expected[i],reg[i]);exit(1);}
}
int main(void){
 word outer[9],inner[9]; int i; memset(mem,0,sizeof(mem));
#ifdef BASELINE
 const word frame_size=4;
#else
 const word frame_size=24;
#endif
 for(i=AX;i<=DS;i++)reg[i]=(word)(0x1100+i*0x111); reg[SI]=0x1f8f;reg[BP]=0x8f90;reg[SS]=0x3456;reg[SP]=0x1000;
 memcpy(outer,reg,sizeof outer);cursor_flag=MOUSE_CURSOR_DISPLAYED;
 mouse_motion.x=0x780;mouse_motion.y=1080;cursor_status.position.x=639;cursor_status.position.y=479;
 jump_to_user_subroutine(1,0x1234,0x4321);
 if(reg[SP]!=(word)(0x1000-frame_size)||reg[CS]!=0x1234)die("outer frame placement");
 memcpy(inner,reg,sizeof inner);jump_to_user_subroutine(0,0x5678,0x8765);
 if(reg[SP]!=(word)(0x1000-2*frame_size))die("nested frame placement");
 reg[SP]+=4;mouse_int2();verify(inner,"inner");if(reg[SP]!=(word)(0x1000-frame_size))die("inner stack");
 reg[SP]+=4;mouse_int2();verify(outer,"outer");
 if(reg[SP]!=0x1000||draws!=1||flushes!=2||resumes!=2)die("return effects");
#ifndef BASELINE
 /* A normal callback and 16-bit stack wrapping retain the same contract. */
 reg[SP]=8;memcpy(outer,reg,sizeof outer);jump_to_user_subroutine(0,0x1234,0x4321);
 if(reg[SP]!=(word)(8-24))die("wrapped frame placement");
 reg[SP]+=4;mouse_int2();verify(outer,"wrapped single");
 if(reg[SP]!=8||draws!=1||flushes!=3||resumes!=3)die("wrapped return effects");
#endif
 puts("PASS: original callback bodies preserve independent nested guest frames");return 0;
}
'''
# Build the same assertions against the tracked pre-fix and candidate bodies.
def build_and_run(name, functions):
    text=preamble+'\n'+functions+'\n#include <stdlib.h>\n'+main
    fixture=out/(name+'.c');fixture.write_text(text,encoding='utf-8')
    exe=out/(name+'.exe')
    subprocess.run(['gcc','-std=c11','-w']+(['-DBASELINE'] if name=='baseline' else [])+['-o',str(exe),str(fixture)],check=True)
    return subprocess.run([str(exe)],capture_output=True,text=True)
if baseline_functions is not None:
    baseline=build_and_run('baseline',baseline_functions)
    if baseline.returncode == 0 or 'outer:' not in baseline.stdout:
        raise RuntimeError('baseline did not demonstrate the nested restore failure: '+baseline.stdout)
    print('BASELINE expected failure:',baseline.stdout.strip())
candidate=build_and_run('candidate',candidate_functions)
if candidate.returncode:
    raise RuntimeError('candidate failed: '+candidate.stdout+candidate.stderr)
print(candidate.stdout.strip())
