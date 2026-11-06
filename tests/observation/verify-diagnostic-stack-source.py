from pathlib import Path
import hashlib
import argparse

root = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('--build-root', default='build/diagnostic-stack-source')
args = parser.parse_args()
output = (root / args.build_root).resolve()
if not output.is_relative_to((root / 'build').resolve()) or output.exists():
    raise SystemExit('fresh repository build directory required')
output.mkdir(parents=True)
items = [
    ('artifacts/mvdm-audit/shard-1/bop9-source-fixture.py',
     'src/mvdm-host/softpc.new/host/src/nt_bop.c',
     'C10228895446D6193AE6FD0215F5B93299359858DA1F7DD0626AC7CBB7E70153',
     "else 'reads=8'", "else 'reads=0'"),
    ('artifacts/mvdm-audit/shard-1/int15-source-fixture.py',
     'src/mvdm-host/softpc.new/base/bios/tape_io.c',
     '9073B54069F74950CB4B14D2B01E7AEAC0EDD2A482930AEF5DE6ABFF5CFCD5F3',
     "else 'reads=2'", "else 'reads=0'"),
    ('artifacts/mvdm-audit/shard-0/illegal-op-source-fixture.py',
     'src/mvdm-host/softpc.new/base/system/illegalp.c',
     'C3214520B0944064CF396D3E22814836A8DC193B0F6594EFAFA627D7C038FF94',
     'else "reads=8"', 'else "reads=4"'),
]
for script, source, old_hash, old_count, new_count in items:
    path = root / script
    text = path.read_text(encoding='utf8')
    digest = hashlib.sha256((root/source).read_bytes()).hexdigest().upper()
    assert old_hash in text and old_count in text
    text = text.replace(old_hash, digest).replace(old_count, new_count)
    text = text.replace('build/t473-s10', output.relative_to(root).as_posix())
    text = text.replace('src/mvdm/', 'src/mvdm-host/')
    text = text.replace('unsigned e,unsigned f,unsigned g,unsigned h,unsigned i){(void)a;(void)b;(void)c;(void)d;(void)e;(void)f;(void)g;(void)h;(void)i;reports++;}',
                        'unsigned e){(void)a;(void)b;(void)c;(void)d;(void)e;reports++;}')
    if 'int15-' in script:
        # Current product selects PM/NTVDM. Other INT15 services abort if reached;
        # this historical witness exercises only AH=91, not those providers.
        extra = r"""
#include <stdlib.h>
#define PM 1
#define NTVDM 1
#define CPU_40_STYLE 1
#define IMPORT extern
#define IPT4(a,b,c,d,e,f,g,h) (a b,c d,e f,g h)
typedef unsigned short word;
static unsigned unexpected(void){abort();return 0;}
#define CMOS_PORT 0x70
#define CMOS_DATA 0x71
#define CMOS_U_M_S_LO 0x30
#define CMOS_U_M_S_HI 0x31
#define ICA0_PORT_0 0x20
#define ICA0_PORT_1 0x21
#define ICA1_PORT_0 0xa0
#define ICA1_PORT_1 0xa1
#define outb(...) unexpected()
#define inb(...) unexpected()
#define getES() unexpected()
#define getSI() unexpected()
#define getCX() unexpected()
#define getBH() unexpected()
#define getBL() unexpected()
#define setAL(...) unexpected()
#define setDS(...) unexpected()
#define setZF(...) unexpected()
#define setIF(...) unexpected()
#define assert1(...) ((void)0)
#define assert0(...) ((void)0)
#define sas_twenty_bit_wrapping_enabled() unexpected()
#define xmsDisableA20Wrapping() unexpected()
#define xmsEnableA20Wrapping() unexpected()
#define sas_move_words_forward(...) unexpected()
#define sas_disable_20_bit_wrapping() unexpected()
static void retrieve_descr_fields(unsigned char*a,unsigned long*b,word*c,unsigned long d){abort();}
"""
        text = text.replace('#define PROD 1', '#define PROD 1' + extra)
    if 'illegal-op-' in script:
        # Match the selected historical define() typo's false branch, including
        # its operational opcode read. Both original and current must read five.
        text = text.replace('#define define(x) 1', '#define define(x) 0')
        text = text.replace('#define EG_BAD_OP 1', '#define EG_BAD_OP 1\n#define EG_BAD_OP386 3')
        text = text.replace('reads=4', 'reads=5')
    print(script, flush=True)
    exec(compile(text, str(path), 'exec'), {'__file__':str(path), '__name__':'__main__'})
