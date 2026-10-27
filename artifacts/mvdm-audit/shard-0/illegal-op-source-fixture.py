"""Compare frozen source bodies with bounded CPU/panel stubs, not a guest."""
import hashlib
import re
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SOURCES = {
    "original": (Path("O:/repos.external/OpenNT/base/mvdm/softpc.new/base/system/illegalp.c"),
                 "94F049032A9298802CAB09AFC61651469DCBF1FEC80AB36444E5C2FED2F681AD"),
    "current": (ROOT / "src/mvdm/softpc.new/base/system/illegalp.c",
                "C3214520B0944064CF396D3E22814836A8DC193B0F6594EFAFA627D7C038FF94"),
}
PREFIX = r'''
#include <stdio.h>
#include <stdint.h>
#define NTVDM 1
#define PROD 1
#define define(x) 1
#define EG_BAD_OP 1
#define ERR_QU_CO_RE 2
typedef char CHAR;
typedef unsigned char UCHAR;
typedef uint16_t word;
typedef uint32_t double_word;
typedef uint32_t sys_addr;
static unsigned reads,writes,panels,dispatches,reports;
static unsigned char memory[16]={0x34,0x12,0x78,0x56};
static unsigned getSS(void){return 0;}
static unsigned getESP(void){return 0;}
static unsigned getEFLAGS(void){return 0x202;}
static sys_addr effective_addr(unsigned s,unsigned o){return s+o;}
static unsigned sas_hw_at(sys_addr a){reads++;return memory[a&15];}
static void sas_store(sys_addr a,unsigned x){writes++;memory[a&15]=(unsigned char)x;}
static void where(char *s,word cs,sys_addr ip){(void)s;(void)cs;(void)ip;}
static void host_error(int a,int b,char *s){(void)a;(void)b;(void)s;panels++;}
static void unexpected_int(void){dispatches++;}
static void mvdm_softpc_report_setup_illegal_opcode(unsigned a,unsigned b,unsigned c,unsigned d,unsigned e,unsigned f,unsigned g,unsigned h,unsigned i){(void)a;(void)b;(void)c;(void)d;(void)e;(void)f;(void)g;(void)h;(void)i;reports++;}
'''
SUFFIX = r'''
int main(void){illegal_op_int();printf("reads=%u writes=%u panel=%u dispatch=%u ip=%02x%02x reports=%u\n",reads,writes,panels,dispatches,memory[1],memory[0],reports);return writes!=2 || panels!=1 || dispatches!=1 || memory[0]!=0x35 || memory[1]!=0x12;}
'''

def extract(text):
    match = re.search(r"void\s+illegal_op_int\s*\(\s*\)\s*\{", text)
    if not match:
        raise ValueError("original entry missing")
    depth, i = 1, match.end()
    while depth and i < len(text):
        depth += (text[i] == "{") - (text[i] == "}")
        i += 1
    if depth:
        raise ValueError("unbalanced body")
    return text[match.start():i]

with tempfile.TemporaryDirectory(prefix="illegal-", dir=ROOT / "build/t473-s10") as tmp:
    for name, (path, expected) in SOURCES.items():
        data = path.read_bytes()
        if hashlib.sha256(data).hexdigest().upper() != expected:
            raise ValueError(name + " source hash changed")
        source, executable = Path(tmp) / (name + ".c"), Path(tmp) / (name + ".exe")
        source.write_text(PREFIX + extract(data.decode("latin1")) + SUFFIX, encoding="utf-8")
        subprocess.run(["gcc", "-std=c11", "-O0", str(source), "-o", str(executable)], check=True)
        result = subprocess.run([str(executable)], text=True, capture_output=True, check=True).stdout.strip()
        print(name, result)
        if ("reads=4" if name == "original" else "reads=8") not in result:
            raise ValueError(name + " unexpected guest-read count")
