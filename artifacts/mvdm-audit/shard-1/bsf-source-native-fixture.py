from pathlib import Path
import hashlib,re,subprocess,tempfile
root=Path(__file__).resolve().parents[3]
source=root/'src/mvdm/softpc.new/base/ccpu386/bsf.c'
expected='229E244BDB8D904EE6E2E390C36A8F5C7BCCEF6AA8FB2BC832B7D91F723B1FD0'
raw=source.read_bytes()
if hashlib.sha256(raw).hexdigest().upper()!=expected:raise RuntimeError('source differs from frozen identity')
t=raw.decode('latin1');m=re.search(r'GLOBAL\s+VOID\s+BSF\s+IFN2\(',t)
if not m:raise RuntimeError('missing BSF')
b=t.index('{',m.start());i=b+1;depth=1
while depth and i<len(t):
 if t[i]=='{':depth+=1
 if t[i]=='}':depth-=1
 i+=1
if depth:raise RuntimeError('unbalanced body')
source_body=t[b:i]
prefix=r'''
#include <stdio.h>
#include <stdint.h>
typedef uint32_t IU32;
#define BIT0_MASK 1u
static int zf;
#define SET_ZF(v) (zf=(v))
static void original_bsf(IU32 *pop1,IU32 op2)
'''
suffix=r'''
int main(void) {
 uint32_t x,dst,native,seed=0x63fa8732; unsigned char native_z; unsigned long long checks=0;
 for(x=0;x<=65535;x++) {
   dst=seed;zf=-1;original_bsf(&dst,x);
   __asm__ volatile("bsfl %2,%0; setz %1" : "=r"(native),"=qm"(native_z) : "r"(x) : "cc");
   if(zf!=(x==0) || (x && (native_z || dst!=native)) || (!x && (native_z!=1 || dst!=seed))) return 1;
   checks++;
 }
 for(unsigned i=0;i<32;i++) {
   x=1u<<i;dst=seed;zf=-1;original_bsf(&dst,x);
   __asm__ volatile("bsfl %2,%0; setz %1" : "=r"(native),"=qm"(native_z) : "r"(x) : "cc");
   if(zf||native_z||dst!=native)return 2;checks++;
 }
 printf("cases=%llu zero_dst=unchanged nonzero=x86 result and zf match\n",checks);
 return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='bsf-',dir=root/'build') as td:
 c=Path(td)/'test.c'; exe=Path(td)/'test.exe';c.write_text(prefix+source_body+suffix,encoding='utf-8')
 subprocess.run(['gcc','-std=c11','-O2',str(c),'-o',str(exe)],check=True)
 print('source_sha256='+expected)
 print(subprocess.run([str(exe)],capture_output=True,text=True,check=True).stdout,end='')
