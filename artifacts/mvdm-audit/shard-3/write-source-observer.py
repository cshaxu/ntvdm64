"""Observe shard-3 WRITE historical inputs and exact shipped-image boundary."""
import csv
import hashlib
import json
import re
import zipfile
from pathlib import Path

A=Path('artifacts/mvdm-audit')
ROOT=Path('src/mvdm/wow16/write')
NAMES=('chlook.c','diachgpr.c','diaprint.c','double.bms','filespec.wri',
       'heapinit.c','indent.bms','left.bms','readme.txt','rmargin.bms','write.def','write.itl')


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    originals=Path('O:/repos.external/OpenNT/base/mvdm/wow16/write')
    originals45=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow16/write')
    rows=[]
    for name in NAMES:
        relative='wow16/write/'+name; record=frozen[relative]
        source=ROOT/name; digest=sha(source.read_bytes())
        assert digest==sha((originals/name).read_bytes())==sha((originals45/name).read_bytes())
        assert digest==record['current_sha256']==record['opennt_sha256']==record['opennt45_sha256']
        assert record['dos_catalog_rows']=='1' and record['wow_catalog_rows']=='2'
        rows.append(dict(relative_path=relative,sha256=digest,wow_source_ids=record['wow_source_ids']))
    makefile=(ROOT/'makefile').read_bytes()
    lnk=(ROOT/'write.lnk').read_bytes()
    assert makefile==(originals/'makefile').read_bytes() and lnk==(originals/'write.lnk').read_bytes()
    assert b'write.def' in lnk.lower() and b'link16 @$(app).lnk' in makefile.lower()
    assert b'chlook.obj:' in makefile.lower() and b'heapinit.obj:' in makefile.lower()
    assert not re.search(rb'(?i)\b(?:chlook|heapinit|diachgpr|diaprint)\.obj\b',lnk)
    itl=(ROOT/'write.itl').read_bytes().lower(); assert b'intl' in itl and b'rbutton.bmp' in itl and b'rmark.bmp' in itl
    spec=(ROOT/'filespec.wri').read_bytes(); assert b'Binary File Format' in spec
    archive=Path('assets/winnt.zip')
    manifest=Path('assets/winnt-guest-manifest.json')
    manifest_json=json.loads(manifest.read_text(encoding='utf-8-sig'))
    assert sha(archive.read_bytes()).lower()==manifest_json['archive_sha256'].lower()
    with zipfile.ZipFile(archive) as z:
        published=sha(z.read('WINNT/WRITE.EXE'))
    mirror=sha((ROOT/'WRITE.EXE').read_bytes())
    installed=sha(Path('O:/winnt/WRITE.EXE').read_bytes())
    assert published==installed and mirror!=published
    return dict(result='pass',sources=rows,recipe_sha256=sha(makefile),link_response_sha256=sha(lnk),
        mirror_write_exe_sha256=mirror,archive_write_exe_sha256=published,installed_write_exe_sha256=installed,
        zip_sha256=sha(archive.read_bytes()),manifest_sha256=sha(manifest.read_bytes()),
        scope='Historical WRITE recipe and binary identity; no source-to-installed image or behavior assertion')


if __name__=='__main__':
    print(json.dumps(inspect(),indent=2))
