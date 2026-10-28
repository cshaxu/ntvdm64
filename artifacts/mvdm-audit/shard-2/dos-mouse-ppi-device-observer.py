"""Read-only original bus mouse/PPI port registration and selected worker link."""
import csv, hashlib, json, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/worker').resolve()
NAMES = ('softpc.new/base/keymouse/mouse.c','softpc.new/base/keymouse/ppi.c')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def dep(obj,rel):
    out=subprocess.run(['ninja','-C',str(B),'-t','deps',obj],check=True,capture_output=True,text=True).stdout
    paths={str((B/line.strip()).resolve()).lower() for line in out.splitlines() if line.startswith('    ')}
    return str((M/rel).resolve()).lower() in paths

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows=[]
    for rel in NAMES:
        f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],wow_catalog_rows=f['wow_catalog_rows']))
    for obj,hdr in (('obj/keymouse/mouse.obj','softpc.new/base/inc/mouse.h'),('obj/keymouse/ppi.obj','softpc.new/base/inc/ppi.h')):
        assert dep(obj,hdr)
    mouse=(M/NAMES[0]).read_text(encoding='latin1').lower(); ppi=(M/NAMES[1]).read_text(encoding='latin1').lower()
    reset=(M/'softpc.new/base/bios/reset.c').read_text(encoding='latin1').lower()
    assert 'io_define_inb(mouse_adaptor, mouse_inb)' in mouse and 'io_define_outb(mouse_adaptor, mouse_outb)' in mouse
    assert 'io_define_inb(ppi_adaptor, ppi_inb)' in ppi and 'io_define_outb(ppi_adaptor, ppi_outb)' in ppi
    assert 'mouse_init();' in reset and 'ppi_init();' in reset
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    edges=[next(line for line in graph.splitlines() if line.startswith('build obj/keymouse/'+n+'.obj:')) for n in ('mouse','ppi')]
    lib=next(line for line in graph.splitlines() if line.startswith('build original-softpc-keymouse.lib:'))
    assert all('obj/keymouse/'+n+'.obj' in lib.split() for n in ('mouse','ppi'))
    mp=(B/'ntvdm.exe.map').read_text(encoding='latin1')
    tokens=('_mouse_init','_mouse_inb','_mouse_outb','_ppi_init','_ppi_inb','_ppi_outb')
    symbols=[next(line.strip() for line in mp.splitlines() if token in line.split()) for token in tokens]
    return dict(result='pass',sources=rows,normalized_deps=[dict(object='obj/keymouse/'+n+'.obj',path=str((M/('softpc.new/base/inc/'+n+'.h')).resolve())) for n in ('mouse','ppi')],
                compile_edges=edges,map_symbols=symbols,reset_source_sha256=sha(M/'softpc.new/base/bios/reset.c'),
                graph_sha256=sha(B/'build.ninja'),map_sha256=sha(B/'ntvdm.exe.map'),
                scope='Source port registration and worker link snapshot; not proof reset path executes or device I/O behavior')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
