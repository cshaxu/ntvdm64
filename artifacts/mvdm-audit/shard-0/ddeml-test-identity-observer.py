"""Read-only original identity and recipe evidence for shard-0 DDEML tests."""
import csv
import hashlib
import json
from pathlib import Path

A = Path('artifacts/mvdm-audit')
M = Path('src/mvdm')
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm'),
             Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    with (A / 'combined-initial-status.csv').open(encoding='utf-8-sig', newline='') as stream:
        ledger = [r for r in csv.DictReader(stream) if r['shard'] == '0' and r['status'] == 'pending'
                  and r['relative_path'].startswith('wow16/ddeml/tests/')]
    with (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig', newline='') as stream:
        frozen = {r['relative_path']: r for r in csv.DictReader(stream)}
    assert len(ledger) == 15
    sources = []
    for entry in ledger:
        path = entry['relative_path']
        f = frozen[path]
        digest = sha(M / path)
        assert digest == sha(ORIGINALS[0] / path) == sha(ORIGINALS[1] / path)
        assert digest == f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
        assert f['dos_catalog_rows'] == '1' and f['wow_catalog_rows'] == '2'
        sources.append(dict(relative_path=path, sha256=digest, wow_source_ids=f['wow_source_ids']))
    root = M / 'wow16/ddeml/tests'
    recipes = [root / q for q in ('ddestrs/sources', 'ddestrs/makefile.dos',
        'src/client/sources', 'src/client/makefile.dos', 'src/server/makefile.dos',
        'src/ddemo/makefile.dos', 'src/client/mk4dos.cmd',
        'src/server/mk4dos.cmd', 'src/ddemo/mk4dos.cmd')]
    for path in recipes:
        assert sha(path) == sha(ORIGINALS[0] / path.relative_to(M))
    assert '-DWIN32' in recipes[0].read_text() and '-DWIN16' in recipes[1].read_text()
    assert '-DWIN32' in recipes[2].read_text() and '-DWIN16' in recipes[3].read_text()
    assert 'client.rc' in recipes[2].read_text() and not (root / 'src/client/client.rc').exists()
    assert 'ddemlcl.rc' in recipes[3].read_text() and (root / 'src/client/ddemlcl.rc').is_file()
    return dict(result='pass', sources=sources,
        recipes=[dict(path=str(p), sha256=sha(p)) for p in recipes],
        scope='Historical test input identities and recipes; not DDEML behavior acceptance')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
