"""Observe original identity and selected x86 WOW32 header dependencies."""
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path

NAMES = ('isz.h', 'mapembed.h', 'waccel.h', 'wcmdgtbl.h', 'wcntl32.h', 'wddeml32.h',
         'wddetbl2.h', 'wdos.h', 'wgtbl2.h', 'wkbman.h', 'wkglobal.h', 'wmsgsbm.h',
         'wow32.h', 'wstbl.h', 'wstruc.h', 'wthman.h', 'wuansi.h', 'wucaret.h',
         'wuclass.h', 'wutext.h', 'wutmr.h')
A = Path('artifacts/mvdm-audit')
SOURCE = Path('src/mvdm/wow32')
REFS = (Path('O:/repos.external/OpenNT/base/mvdm/wow32'),
        Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow32'))
GRAPH = Path('build/.ninja/msvc-x86-release/wow32')


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def inspect():
    frozen = {row['relative_path']: row for row in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    ledger = {row['relative_path']: row for row in csv.DictReader((A / 'combined-initial-status.csv').open(encoding='utf-8-sig'))}
    targets = {str((SOURCE / name).resolve()).lower(): name for name in NAMES}
    deps, includes = {n: [] for n in NAMES}, {n: [] for n in NAMES}
    output = subprocess.check_output(['ninja', '-C', str(GRAPH), '-t', 'deps'], text=True, encoding='utf-8', errors='replace')
    owner = None
    valid = False
    for line in output.splitlines():
        if not line.startswith(' '):
            owner = line.split(': #deps ', 1)[0] if ': #deps ' in line else None
            valid = bool(owner and '(VALID)' in line and '(STALE)' not in line)
        elif valid:
            p = Path(line.strip())
            key = str((p if p.is_absolute() else GRAPH / p).resolve()).lower()
            if key in targets:
                deps[targets[key]].append(owner)
    for root in (SOURCE, Path('src/wow32-dll')):
        for p in list(root.rglob('*.c')) + list(root.rglob('*.h')):
            for number, line in enumerate(p.read_text(encoding='latin1').splitlines(), 1):
                for name in NAMES:
                    if re.search(r'^\s*#\s*include\s*[<"]' + re.escape(name) + r'[>"]', line, re.I):
                        includes[name].append(f'{p}:{number}')
    results = []
    for name in NAMES:
        path = 'wow32/' + name
        item = frozen[path]
        hashes = [sha(root / name) for root in (SOURCE, *REFS)]
        assert hashes == [item['current_sha256'], item['opennt_sha256'], item['opennt45_sha256']]
        assert ledger[path]['shard'] == '0' and ledger[path]['status'] in ('pending', 'partial')
        results.append(dict(path=path, hashes=hashes, direct_includes=includes[name],
                            valid_dependency_objects=sorted(set(deps[name]))))
    return dict(result='pass', graph_sha256=sha(GRAPH / 'build.ninja'), files=results,
                scope='Current VALID x86 WOW32 dependencies and textual includes only; no guest behavior or alternate-profile exclusion')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
