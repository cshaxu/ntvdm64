"""Compare five original guest binaries with archive and exact installed paths."""
import csv
import hashlib
import json
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
ROOTS = (Path('src/mvdm'), Path('O:/repos.external/OpenNT/base/mvdm'),
         Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
FILES = ('dos/v86/cmd/fastopen/FASTOPEN.EXE', 'wow16/mmsystem/MMSYSTEM.DLL',
         'wow16/rasapi16/RASAPI16.DLL', 'wow16/test/shell/WOWEXEC.EXE', 'wow16/winsock/WINSOCK.DLL')


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    archive = Path('assets/winnt.zip')
    rows = []
    with zipfile.ZipFile(archive) as z:
        for path in FILES:
            f = frozen[path]
            hashes = [sha((root / path).read_bytes()) for root in ROOTS]
            assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
            member = 'WINNT/SYSTEM32/' + Path(path).name.upper()
            candidates = [x for x in z.namelist() if x.lower() == member.lower()]
            assert len(candidates) == 1 and sha(z.read(candidates[0])) == hashes[0]
            installed = Path('O:/winnt/system32') / Path(path).name
            installed_hash = sha(installed.read_bytes()) if installed.exists() else None
            assert installed_hash is None or installed_hash == hashes[0]
            rows.append(dict(path=path, hashes=hashes, member=candidates[0], installed_path=str(installed),
                             installed_sha256=installed_hash))
    return dict(result='pass', archive_sha256=sha(archive.read_bytes()), files=rows,
                scope='Exact original, archive and bounded installed-path identity; no guest load, source lineage or behavior')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
