"""Compare original OLE server debug/retail images to the guest archive."""
import csv
import hashlib
import json
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
REL = ('wow16/ole/server/debug/OLESVR.DLL', 'wow16/ole/server/retail/OLESVR.DLL')
ROOTS = (Path('src/mvdm'), Path('O:/repos.external/OpenNT/base/mvdm'),
         Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    sources = []
    for path in REL:
        hashes = [sha((root / path).read_bytes()) for root in ROOTS]
        row = frozen[path]
        assert hashes == [row['current_sha256'], row['opennt_sha256'], row['opennt45_sha256']]
        sources.append(dict(path=path, hashes=hashes))
    assert sources[0]['hashes'][0] != sources[1]['hashes'][0]
    archive_path = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive_path) as z:
        paths = [x for x in z.namelist() if x.lower().endswith('/olesvr.dll')]
        assert paths == ['WINNT/SYSTEM32/OLESVR.DLL']
        packaged = sha(z.read(paths[0]))
    assert packaged == sources[1]['hashes'][0] and packaged != sources[0]['hashes'][0]
    installed = Path('O:/winnt/system32/OLESVR.DLL')
    recipe = ROOTS[0] / 'wow16/ole/server/makefile'
    data = recipe.read_text(encoding='latin1').lower()
    assert '$(bld)\\olesvr.dll:' in data and 'binplace olesvr.dll' in data
    return dict(result='pass', sources=sources, archive_sha256=sha(archive_path.read_bytes()),
                member=paths[0], packaged_sha256=packaged,
                installed_exact_path=str(installed), installed_exists=installed.exists(),
                installed_sha256=sha(installed.read_bytes()) if installed.exists() else None,
                makefile_sha256=sha(recipe.read_bytes()),
                scope='Debug and retail image identity only; no guest loading, build provenance or OLE behavior')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
