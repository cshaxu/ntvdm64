"""Observe two original guest inputs and the WOWEXEC archive identity."""
import csv
import hashlib
import json
import zipfile
from pathlib import Path

A=Path('artifacts/mvdm-audit')
PATHS=('wow16/test/shell/wowexec.ico','wow16/ole/server/olever.h')


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows=[]
    for relative in PATHS:
        record=frozen[relative];source=Path('src/mvdm')/relative
        one=Path('O:/repos.external/OpenNT/base/mvdm')/relative
        two=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/relative
        digest=sha(source.read_bytes())
        assert digest==sha(one.read_bytes())==sha(two.read_bytes())==record['current_sha256']==record['opennt_sha256']==record['opennt45_sha256']
        assert record['dos_catalog_rows']=='1' and record['wow_catalog_rows']=='2'
        rows.append(dict(relative_path=relative,sha256=digest,wow_source_ids=record['wow_source_ids']))
    root=Path('src/mvdm/wow16/test/shell')
    original=Path('O:/repos.external/OpenNT/base/mvdm/wow16/test/shell')
    for name in ('wowexec.rc','makefile'):
        assert (root/name).read_bytes()==(original/name).read_bytes()
    assert b'icon wowexec.ico' in (root/'wowexec.rc').read_bytes().lower()
    make=(root/'makefile').read_bytes().lower()
    assert b'rc16 -r' in make and b'rc16 -t wowexec.res wowexec.exe' in make
    archive=Path('assets/winnt.zip');manifest=Path('assets/winnt-guest-manifest.json')
    assert sha(archive.read_bytes()).lower()==json.loads(manifest.read_text(encoding='utf-8-sig'))['archive_sha256'].lower()
    with zipfile.ZipFile(archive) as z: archive_exe=sha(z.read('WINNT/SYSTEM32/WOWEXEC.EXE'))
    mirror=sha((root/'WOWEXEC.EXE').read_bytes());installed=sha(Path('O:/winnt/system32/WOWEXEC.EXE').read_bytes())
    assert mirror==archive_exe==installed
    return dict(result='pass',sources=rows,recipe_sha256=sha((root/'makefile').read_bytes()),
        resource_sha256=sha((root/'wowexec.rc').read_bytes()),wowexec_sha256=mirror,
        zip_sha256=sha(archive.read_bytes()),scope='Original resource input and immutable image identities; no image build lineage, behavior or OLE consumer proof')


if __name__=='__main__':
    print(json.dumps(inspect(),indent=2))
