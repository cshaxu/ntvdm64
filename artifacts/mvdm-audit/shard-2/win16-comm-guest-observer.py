"""Read-only Win16 COMM guest source and distribution identity audit."""
import csv
import hashlib
import json
import re
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
M = Path('src/mvdm')
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'wow16/drivers/comm/'
NAMES = ('bimodint.inc', 'comm.drv', 'comm.rc', 'ddkmake', 'ibmcom.inc')


def digest(blob):
    return hashlib.sha256(blob).hexdigest().upper()


def sha(path):
    return digest(Path(path).read_bytes())


def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    sources = []
    for name in NAMES:
        relative = ROOT + name; f = frozen[relative]; value = sha(M / relative)
        assert value == sha(ORIGINALS[0] / relative) == sha(ORIGINALS[1] / relative)
        assert value == f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
        assert f['dos_catalog_rows'] == '1' and f['wow_catalog_rows'] == '2'
        sources.append(dict(relative_path=relative, sha256=value, original_path=str(ORIGINALS[0] / relative),
                            dos_catalog_rows=1, wow_catalog_rows=2, wow_source_ids=f['wow_source_ids']))
    names = ('makefile', 'ccom.asm', 'ibmsetup.asm', 'ibmint.asm', 'ibmcom.asm', 'comm.def')
    context = [M / (ROOT + n) for n in names]
    assert all(sha(p) == sha(ORIGINALS[0] / p.relative_to(M)) for p in context)
    recipe, ccom, setup, intr, impl, definitions = [p.read_text(encoding='latin1').lower() for p in context]
    assert re.search(r'(?m)^comm\.drv\s*:', recipe) and 'link16 @$*.lrf' in recipe
    assert re.search(r'(?m)^\s*include\s+ibmcom\.inc\b', ccom)
    assert all(re.search(r'(?m)^\s*include\s+bimodint\.inc\b', s) for s in (setup, intr))
    assert re.search(r'(?m)^\s*include\s+ibmcom\.inc\b', impl)
    assert re.search(r'(?m)^\s*#\s*include\s+"comm\.rcv"', (M / (ROOT + 'comm.rc')).read_text(encoding='latin1').lower())
    assert re.search(r'(?m)^\s*inicom\s+@1\b', definitions)
    ddk = (M / (ROOT + 'ddkmake')).read_text(encoding='latin1').lower()
    assert re.search(r'comm\.drv\s*:', ddk) and 'iclean:' in ddk and 'del comm.drv' in ddk
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        members = [n for n in z.namelist() if n.lower() == 'winnt/system32/comm.drv']
        assert len(members) == 1
        binary = z.read(members[0])
    installed = Path('O:/winnt/system32/COMM.DRV')
    assert digest(binary) == sha(M / (ROOT + 'comm.drv')) == sha(installed)
    return dict(result='pass', sources=sources,
        context=[dict(path=str(p), sha256=sha(p)) for p in context],
        guest_image=dict(archive_path=str(archive), archive_sha256=sha(archive), member=members[0],
                         sha256=digest(binary), installed_path=str(installed)),
        scope='Original guest source/recipe edges and captured binary identity; no runtime serial or source-to-image derivation claim')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
