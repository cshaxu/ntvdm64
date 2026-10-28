"""Read-only DOS command guest source, build-role and package identities."""
import csv
import hashlib
import json
import re
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
M = Path('src/mvdm')
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
PATHS = ('dos/v86/cmd/append/append.exe', 'dos/v86/cmd/command/readme.txt',
    'dos/v86/cmd/exe2bin/exe2bin.tag', 'dos/v86/cmd/keyb/kb16.exc', 'dos/v86/cmd/keyb/keyb.tag',
    'dos/v86/cmd/loadfix/LOADFIX.COM', 'dos/v86/cmd/mem/mem.exe', 'dos/v86/cmd/mem/mem.skl',
    'dos/v86/cmd/nlsfunc/nlsfunc.exe', 'dos/v86/cmd/qbasic/edit.com',
    'dos/v86/cmd/qbasic/edit.hlp', 'dos/v86/cmd/qbasic/gorilla.bas', 'dos/v86/cmd/setver/setver.tag')
IMAGES = {
    'dos/v86/cmd/loadfix/LOADFIX.COM': ('WINNT/SYSTEM32/LOADFIX.COM', 'O:/winnt/system32/LOADFIX.COM'),
    'dos/v86/cmd/mem/mem.exe': ('WINNT/SYSTEM32/MEM.EXE', 'O:/winnt/system32/MEM.EXE'),
    'dos/v86/cmd/qbasic/edit.com': ('WINNT/EDIT.COM', 'O:/winnt/EDIT.COM'),
    'dos/v86/cmd/qbasic/edit.hlp': ('WINNT/EDIT.HLP', 'O:/winnt/EDIT.HLP'),
}


def digest(blob):
    return hashlib.sha256(blob).hexdigest().upper()


def sha(path):
    return digest(Path(path).read_bytes())


def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    sources = []
    for name in PATHS:
        f = frozen[name]
        actual = [sha(root / name) if (root / name).is_file() else '' for root in (M, *ORIGINALS)]
        assert actual == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
        assert actual[0] and actual[0] in actual[1:]
        sources.append(dict(relative_path=name, current_sha256=actual[0], opennt_sha256=actual[1],
            opennt45_sha256=actual[2], dos_catalog_rows=f['dos_catalog_rows'], wow_catalog_rows=f['wow_catalog_rows'],
            wow_source_ids=f['wow_source_ids']))
    context_paths = ('dos/v86/cmd/keyb/makefile', 'dos/v86/cmd/keyb/keyb.lnk',
                     'dos/v86/cmd/mem/makefile', 'dos/v86/cmd/command/readme.txt')
    context = [M / p for p in context_paths]
    assert all(sha(p) == sha(ORIGINALS[0] / p.relative_to(M)) for p in context)
    key, lnk, mem, readme = [p.read_text(encoding='latin1').lower() for p in context]
    assert re.search(r'kb16\.exc\s*:', key) and re.search(r'\$\(dest\)\s*:\s*kb16\.exc', key)
    assert 'kb16.exc' in lnk and re.search(r'mem\.ctl\s*:\s*mem\.skl', mem)
    assert 'source files for command.com' in readme
    archive = Path('assets/winnt.zip'); identities = []
    with zipfile.ZipFile(archive) as z:
        for source, (member, installed) in IMAGES.items():
            names = [n for n in z.namelist() if n.lower() == member.lower()]
            assert len(names) == 1
            archive_hash = digest(z.read(names[0]))
            assert archive_hash == sha(M / source) == sha(installed)
            identities.append(dict(source=source, member=names[0], installed=installed, sha256=archive_hash))
        unmatched = {source: [n for n in z.namelist() if Path(n).name.lower() == Path(source).name.lower()]
                     for source in PATHS if source not in IMAGES}
    return dict(result='pass', sources=sources, context=[dict(path=str(p), sha256=sha(p)) for p in context],
        archive=dict(path=str(archive), sha256=sha(archive), exact_images=identities, other_basename_hits=unmatched),
        scope='Historical sources/recipes and captured guest identities, not build lineage or guest runtime behavior')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
