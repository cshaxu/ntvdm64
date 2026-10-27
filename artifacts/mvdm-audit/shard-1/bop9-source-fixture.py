from pathlib import Path
import hashlib, re, subprocess, tempfile
root = Path(__file__).resolve().parents[3]
sources = {'original': Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/host/src/nt_bop.c'), 'current': root/'src/mvdm/softpc.new/host/src/nt_bop.c'}
expected = {'original': '378A34807645277090BF4B38556EA210502F79A898855B649B39D6C4CCC47F66', 'current': 'C10228895446D6193AE6FD0215F5B93299359858DA1F7DD0626AC7CBB7E70153'}
prefix = r'''
#include <stdio.h>
typedef unsigned long ULONG;
typedef unsigned long sys_addr;
static int reads, calls, reports;
static unsigned long received;
static unsigned long getAX(void) {return 0x5a31;}
static unsigned long getSS(void) {return 0x1000;}
static unsigned long getSP(void) {return 0x10;}
static unsigned long getCS(void) {return 0x2000;}
static unsigned long getIP(void) {return 0x22;}
static unsigned long effective_addr(unsigned long seg,unsigned long off) {return (seg<<4)+off;}
static unsigned long sas_w_at(unsigned long addr) {reads++; return addr & 0xffff;}
static void host_direct_access_error(unsigned long ax) {calls++;received=ax;}
/* Disabled reporter is represented by a non-reading boundary stub. */
static void mvdm_softpc_report_setup_hardware_probe(const char *s,unsigned long a,unsigned long b) {(void)s;(void)a;(void)b;reports++;}
'''
suffix = r'''
int main(void) {MS_bop_9(); printf("reads=%d handler_calls=%d ax=%04lx report_calls=%d\n",reads,calls,received,reports); return (calls!=1 || received!=0x5a31);}
'''
def extract(text):
    m = re.search(r'void\s+MS_bop_9\s*\(\s*void\s*\)\s*\{', text)
    if not m: raise RuntimeError('function missing')
    i=m.end(); depth=1
    while depth and i<len(text):
        if text[i]=='{': depth+=1
        elif text[i]=='}': depth-=1
        i+=1
    if depth: raise RuntimeError('unbalanced function')
    return text[m.start():i]
with tempfile.TemporaryDirectory(prefix='bop9-', dir=root/'build/t473-s10') as td:
    for name, source in sources.items():
        raw=source.read_bytes(); digest=hashlib.sha256(raw).hexdigest().upper()
        if digest != expected[name]: raise RuntimeError(name+' source differs from frozen identity')
        body=extract(raw.decode('latin1'))
        c=Path(td)/(name+'.c'); exe=Path(td)/(name+'.exe')
        c.write_text(prefix+body+suffix,encoding='utf-8')
        subprocess.run(['gcc','-std=c11','-O0',str(c),'-o',str(exe)],check=True)
        p=subprocess.run([str(exe)],check=True,text=True,capture_output=True)
        print(name, 'source_sha256='+hashlib.sha256(raw).hexdigest().upper(),p.stdout.strip())
        required='reads=0' if name=='original' else 'reads=8'
        if required not in p.stdout: raise SystemExit('Unexpected read count')
