"""Observe six original guest package candidates without modifying media."""
import csv
import hashlib
import json
import zipfile
from pathlib import Path

A=Path('artifacts/mvdm-audit')
PATHS=('bin86/config.nt','bin86/i386/win87em.dll','bin86/lanman.drv',
       'bin86/olecli.dll','bin86/ver.dll','bin86/write.hlp')


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    zip_path=Path('assets/winnt.zip')
    archive_sha=sha(zip_path.read_bytes())
    manifest_path=Path('assets/winnt-guest-manifest.json')
    manifest=json.loads(manifest_path.read_text(encoding='utf-8-sig'))
    assert archive_sha.lower()==manifest['archive_sha256'].lower()
    with zipfile.ZipFile(zip_path) as archive:
        members={}
        for info in archive.infolist():
            if not info.is_dir(): members.setdefault(sha(archive.read(info)),[]).append(info.filename)
    rows=[]
    for relative in PATHS:
        record=frozen[relative]
        source=Path('src/mvdm')/relative
        digest=sha(source.read_bytes())
        assert digest==record['current_sha256']
        originals=[]
        for root,key in ((Path('O:/repos.external/OpenNT/base/mvdm'),'opennt_sha256'),
                         (Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'),'opennt45_sha256')):
            original=root/relative
            if record[key]:
                assert original.is_file() and sha(original.read_bytes())==record[key]==digest
                originals.append(str(original))
            else:
                assert not original.exists()
        installed=Path('O:/winnt/system32')/source.name
        installed_sha=sha(installed.read_bytes()) if installed.is_file() else None
        rows.append(dict(relative_path=relative,source_sha256=digest,origin=record['upstream_origin'],
            dos_catalog_rows=record['dos_catalog_rows'],wow_catalog_rows=record['wow_catalog_rows'],
            wow_source_ids=record['wow_source_ids'],originals=originals,zip_members=members.get(digest,[]),
            direct_installed_path=str(installed),direct_installed_sha256=installed_sha,
            direct_installed_matches=installed_sha==digest))
    return dict(result='pass',archive_sha256=archive_sha,manifest_sha256=sha(manifest_path.read_bytes()),
                sources=rows,scope='Archive and direct installed identity only; loader search and module behavior unproven')


if __name__=='__main__':
    print(json.dumps(inspect(),indent=2))
