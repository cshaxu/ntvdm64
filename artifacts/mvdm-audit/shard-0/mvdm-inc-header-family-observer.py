"""Observe original shared header identities and current x86 object dependencies."""
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = Path('src/mvdm/inc')
REFS = (Path('O:/repos.external/OpenNT/base/mvdm/inc'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/inc'))
ROOT = Path('build/.ninja/msvc-x86-release')
NAMES = ('apistruc.h', 'bop.h', 'dbgsvc.h', 'doswow.h', 'exterr.h', 'netb.h', 'sharewow.h',
         'vrdlctab.h', 'vrmisc.h', 'vrmslot.h', 'vrnetapi.h', 'wowclip.h', 'wowcmdlg.h',
         'wowddeml.h', 'wowinfo.h', 'wowint.h', 'wowth.h')


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    want = {str((BASE / n).resolve()).lower(): n for n in NAMES}
    dependencies = {n: [] for n in NAMES}
    graphs = []
    for graph in ('worker', 'wow32'):
        root = ROOT / graph
        output = subprocess.check_output(['ninja', '-C', str(root), '-t', 'deps'], text=True, encoding='utf-8', errors='replace')
        graphs.append(dict(name=graph, sha256=sha(root / 'build.ninja')))
        owner, valid = None, False
        for line in output.splitlines():
            if not line.startswith(' '):
                owner = line.split(': #deps ', 1)[0] if ': #deps ' in line else None
                valid = bool(owner and '(VALID)' in line and '(STALE)' not in line)
            elif valid:
                path = Path(line.strip())
                key = str((path if path.is_absolute() else root / path).resolve()).lower()
                if key in want:
                    dependencies[want[key]].append(graph + '/' + owner)
    includes = {n: [] for n in NAMES}
    pattern = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.I)
    for root in (Path('src/mvdm'), Path('src/wow32-dll')):
        for path in root.rglob('*'):
            if not path.is_file() or path.suffix.lower() not in ('.c', '.h', '.inc', '.cpp'):
                continue
            for number, line in enumerate(path.read_text(encoding='latin1').splitlines(), 1):
                match = pattern.search(line)
                if match:
                    name = match.group(1).replace('\\', '/').split('/')[-1].lower()
                    if name in includes:
                        includes[name].append(f'{path}:{number}')
    rows = []
    for name in NAMES:
        path = 'inc/' + name
        hashes = [sha(root / name) for root in (BASE, *REFS)]
        old = frozen[path]
        assert hashes == [old['current_sha256'], old['opennt_sha256'], old['opennt45_sha256']]
        rows.append(dict(path=path, hashes=hashes, direct_includes=includes[name],
                         valid_objects=sorted(set(dependencies[name]))))
    return dict(result='pass', graphs=graphs, files=rows,
                scope='Exact current VALID x86 object paths and bounded direct includes; no runtime or alternate-profile proof')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
