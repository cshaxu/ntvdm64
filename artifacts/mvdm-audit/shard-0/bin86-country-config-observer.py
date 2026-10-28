"""Read-only package and installed identity comparison for two guest inputs."""
import csv
import hashlib
import json
import zipfile
from pathlib import Path


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def inspect():
    frozen_path = Path('artifacts/mvdm-audit/frozen-file-reconciliation.csv')
    with frozen_path.open(encoding='utf-8-sig', newline='') as stream:
        frozen = {r['relative_path']: r for r in csv.DictReader(stream)}
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        entries = [(i.filename, sha(z.read(i))) for i in z.infolist() if not i.is_dir()]
    results = []
    for name in ('country.sys', 'system.ini'):
        relative = 'bin86/' + name
        source = Path('src/mvdm') / relative
        digest = sha(source.read_bytes())
        row = frozen[relative]
        assert digest == row['current_sha256']
        assert digest in (row['opennt_sha256'], row['opennt45_sha256'])
        installed = []
        for path in (Path('O:/winnt') / name, Path('O:/winnt/system32') / name):
            installed.append(dict(path=str(path), sha256=sha(path.read_bytes()) if path.is_file() else None))
        results.append(dict(relative_path=relative, sha256=digest,
            original_hashes=[row['opennt_sha256'], row['opennt45_sha256']],
            zip_matching_members=[name for name, value in entries if value == digest], installed=installed))
    return dict(result='pass', archive_sha256=sha(archive.read_bytes()), sources=results,
        scope='Point-in-time identities only; no guest search path or behavior claim')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
