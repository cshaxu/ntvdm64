"""Read-only multimedia guest build, resource and package observations."""
import csv, hashlib, json, zipfile
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'wow16/mmsystem/'
NAMES = ('messages/usa/mci.rc', 'messages/usa/mmtask.rc', 'mmsystem.rc', 'mmtask/mmtask.asm', 'timea.asm')

def digest(data):
    return hashlib.sha256(data).hexdigest().upper()

def sha(p):
    return digest(p.read_bytes()) if p.is_file() else ''

def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows = []
    for name in NAMES:
        rel = ROOT + name; f = frozen[rel]
        hashes = [sha(root / rel) for root in (M, *O)]
        assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
        assert hashes[0] and hashes[0] in hashes[1:]
        rows.append(dict(path=rel, hashes=hashes, dos_catalog_rows=f['dos_catalog_rows'], wow_catalog_rows=f['wow_catalog_rows']))
    ctx = [M / (ROOT + n) for n in ('makefile', 'mmtask/makefile', 'task.c')]
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in ctx)
    make, task_make, task = [p.read_text(encoding='latin1').lower() for p in ctx]
    rc = (M / (ROOT + 'mmsystem.rc')).read_text(encoding='latin1').lower()
    assert 'res_dir =.\\messages\\usa' in make and 'mci.rc: $(res_dir)' in make
    assert '#include "mci.rc"' in rc and 'mmtask.tsk' in rc
    assert 'mmtask.tsk: mmtask\\mmtask.asm' in make and 'obj\t=$(name).obj' in task_make
    assert 'res_dir\t=..\\messages\\usa' in task_make and '$(name).rc:\t$(res_dir)' in task_make
    archive = Path('assets/winnt.zip'); images = []
    with zipfile.ZipFile(archive) as z:
        for name in ('MMSYSTEM.DLL', 'MMTASK.TSK'):
            found = [n for n in z.namelist() if n.lower() == ('winnt/system32/' + name).lower()]
            assert len(found) <= 1
            member_hash = digest(z.read(found[0])) if found else ''
            source = M / (ROOT + name); installed = Path('O:/winnt/system32') / name
            images.append(dict(name=name, member=found[0] if found else '', archive_sha256=member_hash,
                               source_sha256=sha(source), installed_sha256=sha(installed)))
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                archive_sha256=sha(archive), images=images,
                scope='Historical USA resource/build edges and archive identities, not active language, module load or behavior')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
