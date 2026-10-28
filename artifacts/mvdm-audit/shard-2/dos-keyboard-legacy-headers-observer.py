"""Read-only legacy keyboard header and selected controller dependency observation."""
import csv, hashlib, json, re, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/worker').resolve()
ROOT='softpc.new/base/inc/'
NAMES=('keycaps.h','m_keybdp.h')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    out=subprocess.run(['ninja','-C',str(B),'-t','deps','obj/keymouse/keyba.obj'],check=True,capture_output=True,text=True).stdout
    deps={str((B/line.strip()).resolve()).lower() for line in out.splitlines() if line.startswith('    ')}
    rows=[]
    for name in NAMES:
        rel=ROOT+name; f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],wow_catalog_rows=f['wow_catalog_rows'],
                         keyba_normalized_dependency=str((M/rel).resolve()).lower() in deps))
    caps=(M/(ROOT+'keycaps.h')).read_text(encoding='latin1')
    xkeys=(M/(ROOT+'m_keybdp.h')).read_text(encoding='latin1')
    assert '#ifdef UIF_TEST' in caps and 'LOCAL CHAR *keycaps[]' in caps
    assert 'LOCAL KeySym KeySymTable[]' in xkeys and 'XK_BackSpace' in xkeys
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    edge=next(line for line in graph.splitlines() if line.startswith('build obj/keymouse/keyba.obj:'))
    flags=next(line for line in graph.splitlines() if line.startswith('cflags ='))
    assert not re.search(r'/DUIF_TEST(?:\s|=|$)',flags,re.I)
    mp=(B/'ntvdm.exe.map').read_text(encoding='latin1')
    symbols=[next(line.strip() for line in mp.splitlines() if token in line.split()) for token in ('_KbdEOIHook','_kbd_inb')]
    return dict(result='pass',sources=rows,normalized_deps_object='obj/keymouse/keyba.obj',compile_edge=edge,map_symbols=symbols,
                graph_sha256=sha(B/'build.ninja'),map_sha256=sha(B/'ntvdm.exe.map'),
                scope='Original alternate-keyboard header contents and one controller build snapshot; not keyboard behavior or total consumer proof')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
