"""Observe original registry-editor / debugger guest inputs and packages."""
import csv
import hashlib
import json
import zipfile
from pathlib import Path

A=Path('artifacts/mvdm-audit')
NAMES=('wow16/regedit/debug/REGEDIT.RES','wow16/regedit/regedit.ico',
       'wow16/regedit/sdkreged.h','wow16/regedit/sdkreged.ico',
       'wow16/sherlock/DRWATSON.EXE','wow16/sherlock/watson.ico')


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    archive=Path('assets/winnt.zip')
    manifest=Path('assets/winnt-guest-manifest.json')
    assert sha(archive.read_bytes()).lower()==json.loads(manifest.read_text(encoding='utf-8-sig'))['archive_sha256'].lower()
    with zipfile.ZipFile(archive) as z:
        archive_names=[i.filename for i in z.infolist() if not i.is_dir()]
        identities={}
        for name in archive_names:
            identities.setdefault(sha(z.read(name)),[]).append(name)
    rows=[]
    for relative in NAMES:
        record=frozen[relative];source=Path('src/mvdm')/relative;digest=sha(source.read_bytes())
        assert digest==record['current_sha256']
        origins=[]
        for base,key in ((Path('O:/repos.external/OpenNT/base/mvdm'),'opennt_sha256'),
                         (Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'),'opennt45_sha256')):
            original=base/relative
            if record[key]:
                assert original.is_file() and sha(original.read_bytes())==record[key]==digest
                origins.append(str(original))
            else:
                assert not original.exists()
        rows.append(dict(relative_path=relative,sha256=digest,origin=record['upstream_origin'],
            dos_catalog_rows=record['dos_catalog_rows'],wow_catalog_rows=record['wow_catalog_rows'],
            wow_source_ids=record['wow_source_ids'],originals=origins,zip_members=identities.get(digest,[])))
    root=Path('src/mvdm/wow16')
    recipes=[]
    for relative in ('regedit/makefile','regedit/messages/usa/regedit.rc',
                     'sherlock/makefile','sherlock/messages/usa/drwatson.rc'):
        source=root/relative;original=Path('O:/repos.external/OpenNT/base/mvdm/wow16')/relative
        assert source.read_bytes()==original.read_bytes()
        recipes.append(dict(relative_path=relative,sha256=sha(source.read_bytes())))
    lower={r['relative_path']:(root/r['relative_path']).read_bytes().lower() for r in recipes}
    assert br'$(dstdir)\regedit.res:' in lower['regedit/makefile']
    assert b'sdkreged.obj: sdkreged.c sdkreged.h' in lower['regedit/makefile']
    assert b'icon regedit.ico' in lower['regedit/messages/usa/regedit.rc']
    assert b'drwatson.exe:' in lower['sherlock/makefile'] and b'icon doctor.ico' in lower['sherlock/messages/usa/drwatson.rc']
    candidates=('O:/winnt/DRWATSON.EXE','O:/winnt/system32/DRWATSON.EXE',
                'O:/winnt/REGEDT16.EXE','O:/winnt/system32/REGEDT16.EXE')
    installed=[dict(path=name,exists=Path(name).is_file(),sha256=sha(Path(name).read_bytes()) if Path(name).is_file() else None) for name in candidates]
    return dict(result='pass',sources=rows,recipes=recipes,archive_sha256=sha(archive.read_bytes()),
        archive_matching_names=[name for name in archive_names if any(x in name.lower() for x in ('drwatson','regedt16','regload'))],
        direct_installation=installed,scope='Historical source/recipe and current package presence only, not loader behavior or guest semantics')


if __name__=='__main__':
    print(json.dumps(inspect(),indent=2))
