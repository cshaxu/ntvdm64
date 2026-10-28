"""Compare immutable DOS kernel images, build intermediate, and package bytes."""
import csv
import hashlib
import json
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit'); M = Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
PATHS = ('dos/v86/doskrnl/bios/NTIO.SYS', 'dos/v86/doskrnl/dos/NTDOS.BIN', 'dos/v86/doskrnl/dos/NTDOS.SYS')
def digest(data): return hashlib.sha256(data).hexdigest().upper()
def sha(path): return digest(Path(path).read_bytes())
def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    entries = []
    for rel in PATHS:
        row = frozen[rel]; hashes = [sha(root / rel) for root in (M, *O)]
        assert hashes[0] == hashes[1] == hashes[2] == row['current_sha256'] == row['opennt_sha256'] == row['opennt45_sha256']
        entries.append(dict(path=rel, sha256=hashes[0], wow_source_ids=row['wow_source_ids']))
    make = M / 'dos/v86/doskrnl/dos/makefile'; bios = M / 'dos/v86/doskrnl/bios/makefile'
    assert all(sha(p) == sha(O[0] / p.relative_to(M)) for p in (make, bios))
    text = make.read_text(encoding='latin1').lower()
    assert 'reloc ntdos.exe ntdos.bin' in text and 'stripz ntdos.bin $(dest)' in text
    assert 'dest\t=ntdos.sys' in text and 'dest\t=ntio.sys' in bios.read_text(encoding='latin1').lower()
    archive = Path('assets/winnt.zip'); images = []
    with zipfile.ZipFile(archive) as z:
        for rel in (PATHS[0], PATHS[2]):
            name = Path(rel).name; member = 'WINNT/SYSTEM32/' + name
            data = z.read(member); installed = Path('O:/winnt/system32') / name
            assert sha(M / rel) == digest(data) == sha(installed)
            images.append(dict(path=rel, member=member, sha256=digest(data), installed=str(installed)))
        assert not any(n.lower().endswith('/ntdos.bin') for n in z.namelist())
    return dict(result='pass', sources=entries, images=images, archive_sha256=sha(archive),
                makefile_sha256=sha(make), bios_makefile_sha256=sha(bios),
                limits='Exact image identity and historical intermediate role only; no source-to-image or guest execution proof.')
if __name__ == '__main__': print(json.dumps(inspect(), indent=2))
