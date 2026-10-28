"""Observe retained CTL3D debug/reference inputs and archived guest image."""
import csv
import hashlib
import json
import zipfile
from pathlib import Path

A=Path('artifacts/mvdm-audit')
ROOT=Path('src/mvdm/wow16/ctl3dv2')
NAMES=('3dcheck.bmp','new3d.doc','scomp.dif','version.h')


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    one=Path('O:/repos.external/OpenNT/base/mvdm/wow16/ctl3dv2')
    two=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow16/ctl3dv2')
    rows=[]
    for name in NAMES:
        relative='wow16/ctl3dv2/'+name;record=frozen[relative]
        digest=sha((ROOT/name).read_bytes())
        assert digest==sha((one/name).read_bytes())==sha((two/name).read_bytes())
        assert digest==record['current_sha256']==record['opennt_sha256']==record['opennt45_sha256']
        assert record['dos_catalog_rows']=='1' and record['wow_catalog_rows']=='2'
        rows.append(dict(relative_path=relative,sha256=digest,wow_source_ids=record['wow_source_ids']))
    context={}
    for name in ('readme.txt','ctl3d.rc','ctl3d.c','common.ver'):
        current=(ROOT/name).read_bytes();assert current==(one/name).read_bytes()
        context[name]=sha(current)
    readme=(ROOT/'readme.txt').read_text(encoding='latin1').lower()
    resource=(ROOT/'ctl3d.rc').read_text(encoding='latin1').lower()
    assert 'debugging purposes only' in readme and 'binplaced from' in readme
    assert '#include "version.h"' in resource and 'bitmap "3dcheck.bmp"' in resource
    source=Path('src/mvdm/bin86/ctl3dv2.dll')
    archive=Path('assets/winnt.zip');manifest=Path('assets/winnt-guest-manifest.json')
    assert sha(archive.read_bytes()).lower()==json.loads(manifest.read_text(encoding='utf-8-sig'))['archive_sha256'].lower()
    with zipfile.ZipFile(archive) as z:
        archive_hash=sha(z.read('WINNT/SYSTEM32/CTL3DV2.DLL'))
    assert archive_hash==sha(source.read_bytes())
    direct=Path('O:/winnt/system32/CTL3DV2.DLL')
    return dict(result='pass',sources=rows,context=context,archive_sha256=sha(archive.read_bytes()),
        source_dll_sha256=archive_hash,direct_installed_exists=direct.is_file(),
        direct_installed_sha256=sha(direct.read_bytes()) if direct.is_file() else None,
        scope='Original reference inputs and archived identity, not actual loader resolution or control behavior')


if __name__=='__main__':
    print(json.dumps(inspect(),indent=2))
