"""Recheck DOS guest device identities and retained recipe evidence."""
import csv
import hashlib
import zipfile
from pathlib import Path

ROOT=Path('artifacts/mvdm-audit')
PREFIX='dos/v86/dev/'
NAMES=('ansi/ansi.tag','country/country.exe','himem/HIMEM.SYS',
       'himem/himem1.asm','himem/usa-ms/messages.asm','keyboard/kdf.asm',
       'keyboard/kdfbl097.asm','keyboard/kdfbl098.asm','keyboard/kdffr.asm',
       'keyboard/kdfru.asm','keyboard/kdfsu.asm','keyboard/kdfuk168.asm',
       'keyboard/kdfur094.asm','keyboard/KEYBOARD.SYS')


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def main():
    frozen={r['relative_path']:r for r in csv.DictReader((ROOT/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    report=(ROOT/'shard-3/dos-device-locale-role-review.md').read_text(encoding='utf-8')
    for tail in NAMES:
        relative=PREFIX+tail
        row=frozen[relative]
        src=Path('src/mvdm')/relative
        digest=sha(src.read_bytes())
        assert digest==row['current_sha256'] and relative in report and digest in report
        for base,key in ((Path('O:/repos.external/OpenNT/base/mvdm'),'opennt_sha256'),
                         (Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'),'opennt45_sha256')):
            original=base/relative
            assert (sha(original.read_bytes())==row[key]) if row[key] else not original.exists()
    base=Path('src/mvdm/dos/v86/dev/keyboard')
    original=Path('O:/repos.external/OpenNT/base/mvdm/dos/v86/dev/keyboard')
    for name in ('makefile','keyboard.lnk','kdfnow.asm'):
        assert (base/name).read_bytes()==(original/name).read_bytes()
    link=(base/'keyboard.lnk').read_text(encoding='latin1').lower()
    assert 'kdfnow.obj' in link and 'kdfsv.obj' in link
    assert not any(name in link for name in ('kdfsu.obj','kdffr.obj','kdfru.obj','kdfuk168.obj'))
    assert 'finland' in (base/'kdfnow.asm').read_text(encoding='latin1').lower()
    with zipfile.ZipFile('assets/winnt.zip') as archive:
        for filename,relative in (('HIMEM.SYS','himem/HIMEM.SYS'),('KEYBOARD.SYS','keyboard/KEYBOARD.SYS')):
            digest=sha((Path('src/mvdm')/PREFIX/relative).read_bytes())
            assert sha(archive.read('WINNT/SYSTEM32/'+filename))==digest
            assert sha((Path('O:/winnt/system32')/filename).read_bytes())==digest
    print('PASS 14 identities, original keyboard link recipe and two archived/installed binary matches; no layout behavior claim')


if __name__=='__main__':
    main()
