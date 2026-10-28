"""Observe original DOSX x86 selected historical guest build inputs."""
import csv
import hashlib
import json
import re
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
M = Path('src/mvdm')
ROOTS = (M, Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ASM = ('486/dxfunc.asm', '486/dxint31.asm', '486/dxmain.asm', '486/dxstrt.asm',
       'dxdma.asm', 'dxemm.asm', 'dxemm2.asm', 'dxfind.asm', 'dxmsg.asm')
INCLUDES = ('cmacros.inc', 'interupt.inc', 'pmdefs.inc')


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    ledger = {r['relative_path']: r for r in csv.DictReader((A / 'combined-initial-status.csv').open(encoding='utf-8-sig'))}
    recipe = (M / 'dpmi/makefile').read_text(encoding='latin1').lower()
    link = (M / 'dpmi/dosxi.lnk').read_text(encoding='latin1').lower()
    for name in ('dpmi/makefile', 'dpmi/dosxi.lnk'):
        assert len({sha((root / name).read_bytes()) for root in ROOTS}) == 1
    assert 'lnkfile = dosxi.lnk' in recipe and 'country=usa' in recipe
    rows = []
    for name in ASM + INCLUDES:
        path = 'dpmi/' + name
        hashes = [sha((root / path).read_bytes()) for root in ROOTS]
        f = frozen[path]
        assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
        assert ledger[path]['shard'] == '0' and ledger[path]['status'] in ('pending', 'excluded')
        if name in ASM:
            stem = Path(name).stem.lower()
            actual = name.replace('/', '\\')[:-4].lower()
            assert re.search(r'(?m)^\s*\$\(asm486\)\s+' + re.escape(actual) + r'\s*,', recipe), name
            assert re.search(r'(?i)\b' + re.escape(stem) + r'\.obj\b', link), name
        else:
            assert name.lower() in recipe
        rows.append(dict(path=path, hashes=hashes))
    assert 'copy $(country)\\dxmsg.asm .' in recipe
    assert (M / 'dpmi/dxmsg.asm').read_bytes() == (M / 'dpmi/usa/dxmsg.asm').read_bytes()
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        member = next(x for x in z.namelist() if x.lower() == 'winnt/system32/dosx.exe')
        image = z.read(member)
    assert image == (M / 'dpmi/486/dosx.exe').read_bytes() == Path('O:/winnt/system32/DOSX.EXE').read_bytes()
    return dict(result='pass', files=rows, recipe_sha256=sha((M / 'dpmi/makefile').read_bytes()),
                link_sha256=sha((M / 'dpmi/dosxi.lnk').read_bytes()), dosx_sha256=sha(image),
                scope='Original x86 guest recipe and image identity only; no source-to-image lineage or DOSX behavior')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
