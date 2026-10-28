"""Audit original tools16 paths, relocated copies and current Ninja inputs."""
import csv
import hashlib
import json
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
ROOT = Path('src/mvdm')
ORIGINAL = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
RETAINED = Path('tools/historical/opennt-4.5')
BUILDS = [Path('build/.ninja/msvc-x86-release') / n for n in ('worker', 'wow32')]
BUILDS += [Path('build/.ninja/msvc-x64-release') / n for n in ('frontend', 'service')]


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def rows(path):
    with path.open(encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


def norm(path):
    return str(path.resolve()).casefold()


def main():
    frozen = [r for r in rows(A / 'frozen-file-reconciliation.csv') if r['relative_path'].startswith('tools16/')]
    ledger = {r['relative_path']: r for r in rows(A / 'combined-initial-status.csv')}
    assert len(frozen) == 33
    targets = {norm(ROOT / r['relative_path']): r['relative_path'] for r in frozen}
    entries = []
    for r in frozen:
        rel = r['relative_path']
        original, retained = ORIGINAL / rel, RETAINED / Path(rel).name
        assert not (ROOT / rel).exists()
        assert sha(original) == sha(retained) == r['opennt45_sha256'].upper()
        assert rel in ledger
        entries.append(dict(path=rel, original_sha256=sha(original),
                            retained_path=str(retained), retained_sha256=sha(retained), dependencies=[]))
    lookup = {r['path']: r for r in entries}
    graphs = []
    for build in BUILDS:
        graph = (build / 'build.ninja').read_text(encoding='utf-8').replace('$:', ':').casefold()
        assert all(p.replace('\\', '/') not in graph.replace('\\', '/') for p in targets)
        output = subprocess.check_output(['ninja', '-C', str(build), '-t', 'deps'],
                                         text=True, encoding='utf-8', errors='replace')
        owner = None
        valid = False
        for line in output.splitlines():
            if not line.startswith(' '):
                owner = line.split(': #deps ', 1)[0] if ': #deps ' in line else None
                valid = bool(owner and line.rstrip().endswith('(VALID)'))
            elif valid:
                value = Path(line.strip())
                path = norm(value if value.is_absolute() else build / value)
                if path in targets:
                    lookup[targets[path]]['dependencies'].append(dict(build=str(build), object=owner))
        graphs.append(dict(path=str(build / 'build.ninja'), sha256=sha(build / 'build.ninja')))
    assert all(not entry['dependencies'] for entry in entries)
    return dict(result='pass', entries=entries, graphs=graphs,
        scope='Retained original artifacts and exact mirror-path references only; no runtime or guest build claim')


if __name__ == '__main__':
    print(json.dumps(main(), indent=2))
