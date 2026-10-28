"""Read-only TOOLHELP guest image and original class thunk dispatch."""
import csv, hashlib, json, re, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
REL = 'wow16/toolhelp/TOOLHELP.DLL'

def digest(data):
    return hashlib.sha256(data).hexdigest().upper()

def sha(p):
    return digest(p.read_bytes()) if p.is_file() else ''

def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    f = frozen[REL]; hashes = [sha(root / REL) for root in (M, *O)]
    assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
    assert hashes[0] and hashes[0] in hashes[1:]
    names = ('wow16/toolhelp/makefile', 'wow16/toolhelp/toolhelp.def', 'wow16/toolhelp/ththunks.asm', 'wow32/wthtbl2.h')
    ctx = [M / n for n in names]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make, definition, guest, table = [p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'ththunks.obj' in make and 'toolhelp.dll' in make
    for name, ordinal in (('classfirst', 69), ('classnext', 70)):
        assert re.search(r'\b' + name + r'\s+@' + str(ordinal) + r'\b', definition)
        assert re.search(r'toolhelpthunk\s+' + name + r'\b', guest)
        assert re.search(r'w32fun\(unimplementedapi,\s*"' + name + '"', table)
    assert re.search(r'taskfirst\s+@63\b', definition)
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        members = [n for n in z.namelist() if n.lower() == 'winnt/system32/toolhelp.dll']
        assert len(members) == 1
        image = digest(z.read(members[0]))
    assert image == hashes[0]
    installed = Path('O:/winnt/system32/TOOLHELP.DLL')
    return dict(result='pass', relative_path=REL, hashes=hashes, dos_catalog_rows=f['dos_catalog_rows'],
                wow_catalog_rows=f['wow_catalog_rows'], context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), archive_member=members[0], image_sha256=image,
                installed_path=str(installed), installed_sha256=sha(installed),
                scope='Source-level class dispatch and image identity, not guest execution or complete ToolHelp behavior')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
