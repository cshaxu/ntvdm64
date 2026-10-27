from pathlib import Path
import hashlib, re, subprocess, tempfile
root = Path(__file__).resolve().parents[3]
sources = {'original': Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/base/bios/tape_io.c'), 'current': root/'src/mvdm/softpc.new/base/bios/tape_io.c'}
expected = {'original':'3AA86F1FAF07F68937629F1D6E410EB41F498E17DC6D747647C7D6E5697C80AA','current':'9073B54069F74950CB4B14D2B01E7AEAC0EDD2A482930AEF5DE6ABFF5CFCD5F3'}
prefix = r'''
#include <stdio.h>
#define PROD 1
#define always_trace0(s) ((void)0)
#define INT15_DEVICE_OPEN 0x80
#define INT15_DEVICE_CLOSE 0x81
#define INT15_PROGRAM_TERMINATION 0x82
#define INT15_REQUEST_KEY 0x85
#define INT15_DEVICE_BUSY 0x90
#define INT15_EMS_DETERMINE 0x88
#define INT15_MOVE_BLOCK 0x87
#define INT15_VIRTUAL_MODE 0x89
#define INT15_INTERRUPT_COMPLETE 0x91
#define INT15_CONFIGURATION 0xc0
#define INT15_JOYSTICK 0x84
#define INT15_INVALID 0x86
#define CONF_TABLE_OFFSET 0xe6f5
typedef unsigned long sys_addr;
typedef unsigned char half_word;
static unsigned ah=0x91, al=0x25, cf=7, reads, reports;
static unsigned getAH(void) {return ah;}
static unsigned getAL(void) {return al;}
static unsigned getAX(void) {return (ah<<8)|al;}
static unsigned getSS(void) {return 0x1000;}
static unsigned getSP(void) {return 0x10;}
static unsigned getCS(void) {return 0x2000;}
static sys_addr effective_addr(unsigned seg,unsigned off) {return (seg<<4)+off;}
static unsigned sas_w_at(sys_addr address) {reads++;return address&0xffff;}
static void setAH(unsigned n) {ah=n;}
static void setAX(unsigned n) {ah=n>>8;al=n&255;}
static void setCF(unsigned n) {cf=n;}
static void setES(unsigned n) {(void)n;}
static void setBX(unsigned n) {(void)n;}
static void mvdm_softpc_report_setup_hardware_probe(const char *s,unsigned long a,unsigned long b) {(void)s;(void)a;(void)b;reports++;}
'''
suffix = r'''
int main(void) {cassette_io();printf("reads=%u ah=%02x al=%02x cf=%u report_calls=%u\n",reads,ah,al,cf,reports);return ah!=0x91 || al!=0x25 || cf!=7;}
'''
def extract(text):
    m=re.search(r'void\s+cassette_io\s*\(\s*\)\s*\{', text)
    if not m: raise RuntimeError('missing cassette_io')
    i=m.end(); depth=1
    while depth and i<len(text):
        if text[i]=='{': depth+=1
        elif text[i]=='}': depth-=1
        i+=1
    if depth: raise RuntimeError('unbalanced body')
    return text[m.start():i]
with tempfile.TemporaryDirectory(prefix='int15-',dir=root/'build/t473-s10') as td:
    for name, source in sources.items():
        raw=source.read_bytes()
        if hashlib.sha256(raw).hexdigest().upper()!=expected[name]: raise RuntimeError(name+' source differs from frozen identity')
        c=Path(td)/(name+'.c'); exe=Path(td)/(name+'.exe')
        c.write_text(prefix+extract(raw.decode('latin1'))+suffix,encoding='utf-8')
        subprocess.run(['gcc','-std=c11','-O0',str(c),'-o',str(exe)],check=True)
        result=subprocess.run([str(exe)],check=True,text=True,capture_output=True).stdout.strip()
        print(name,'sha256='+hashlib.sha256(raw).hexdigest().upper(),result)
        if ('reads=0' if name=='original' else 'reads=2') not in result: raise SystemExit('unexpected count')
