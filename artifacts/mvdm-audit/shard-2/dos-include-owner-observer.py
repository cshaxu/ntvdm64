"""Read-only DOS guest include identities and documented source edges."""
import csv
import hashlib
import json
import re
from pathlib import Path

A = Path('artifacts/mvdm-audit')
M = Path('src/mvdm')
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
NAMES = ('dos/v86/h/bios.inc', 'dos/v86/h/process.h', 'dos/v86/h/stddef.h',
         'dos/v86/inc/msdata2.asm', 'dos/v86/inc/oemnum.inc', 'dos/v86/inc/pathmac.inc',
         'dos/v86/inc/pcgequ.inc', 'dos/v86/inc/pcinput.inc')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    sources = []
    for name in NAMES:
        f = frozen[name]; value = sha(M / name)
        assert value == sha(ORIGINALS[0] / name) == sha(ORIGINALS[1] / name)
        assert value == f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
        assert f['dos_catalog_rows'] == '1' and f['wow_catalog_rows'] == '2'
        sources.append(dict(relative_path=name, sha256=value, original_path=str(ORIGINALS[0] / name),
                            dos_catalog_rows=1, wow_catalog_rows=2, wow_source_ids=f['wow_source_ids']))
    oem = (M / 'dos/v86/inc/oemnum.inc').read_bytes()
    assert b'documentation purposes only' in oem.lower()
    assert all(not line.strip() or line.lstrip().startswith(b';') for line in oem.splitlines())
    assert len(oem.splitlines()) == 17
    context = [M / name for name in ('dos/v86/cmd/command/buildmsg.c',
        'dos/v86/cmd/command/makefile', 'dos/v86/doskrnl/dos/makefile')]
    assert all(sha(p) == sha(ORIGINALS[0] / p.relative_to(M)) for p in context)
    buildmsg, command_recipe, dos_recipe = [p.read_text(encoding='latin1').lower() for p in context]
    assert re.search(r'(?m)^\s*#\s*include\s+"process\.h"', buildmsg)
    assert 'buildmsg.exe' in command_recipe
    assert re.search(r'(?m)^\s*msdata\.obj\s*:\s*msdata\.asm', dos_recipe)
    assert re.search(r'(?m)^\s*include\s+msdata\.asm', (M / 'dos/v86/inc/msdata2.asm').read_text(encoding='latin1').lower())
    return dict(result='pass', sources=sources, oemnum_all_nonblank_lines_are_comments=True,
        context=[dict(path=str(p), sha256=sha(p)) for p in context],
        scope='Original identities, documentation-only lines and bounded historical source edges; no guest behavior or general include absence claim')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
