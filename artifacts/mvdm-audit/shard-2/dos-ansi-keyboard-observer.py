"""Read-only source/recipe and guest archive identities for DOS devices."""
import csv
import hashlib
import json
import re
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
M = Path('src/mvdm')
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
NAMES = ('dos/v86/dev/ansi/ansi.skl', 'dos/v86/dev/ansi/ANSI.SYS', 'dos/v86/dev/ansi/ansivid.inc',
         'dos/v86/dev/keyboard/kdfbr.asm', 'dos/v86/dev/keyboard/kdfit.asm',
         'dos/v86/dev/keyboard/kdfru092.asm', 'dos/v86/dev/keyboard/kdfyc116.asm',
         'dos/v86/dev/keyboard/kdfyc117.asm')


def digest(data):
    return hashlib.sha256(data).hexdigest().upper()


def sha(path):
    return digest(Path(path).read_bytes())


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
    ansi_recipe = M / 'dos/v86/dev/ansi/makefile'
    key_recipe = M / 'dos/v86/dev/keyboard/makefile'
    assert all(sha(p) == sha(ORIGINALS[0] / p.relative_to(M)) for p in (ansi_recipe, key_recipe))
    ansi = ansi_recipe.read_text(encoding='latin1').lower()
    keyboard = key_recipe.read_text(encoding='latin1').lower()
    assert re.search(r'ansi\.ctl\s*:\s*ansi\.skl', ansi)
    assert re.search(r'ansiinit\.obj\s*:[\s\S]*?ansivid\.inc', ansi)
    assert re.search(r'(?m)^\s*#\s*kdfbr\.obj\s*:', keyboard)
    assert all(re.search(r'(?m)^\s*' + name + r'\.obj\s*:', keyboard) for name in ('kdfbr275', 'kdfit141', 'kdfru091'))
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        found = [n for n in z.namelist() if n.lower() == 'winnt/system32/keyboard.sys']
        assert len(found) == 1
        archive_keyboard = digest(z.read(found[0]))
        ansi_members = [n for n in z.namelist() if n.replace('\\', '/').lower().endswith('/ansi.sys')]
    actual_keyboard = Path('O:/winnt/system32/KEYBOARD.SYS')
    source_keyboard = M / 'dos/v86/dev/keyboard/KEYBOARD.SYS'
    assert archive_keyboard == sha(actual_keyboard) == sha(source_keyboard)
    return dict(result='pass', sources=sources, recipes=[dict(path=str(p), sha256=sha(p)) for p in (ansi_recipe, key_recipe)],
        archive=dict(path=str(archive), sha256=sha(archive), keyboard_member=found[0], keyboard_sha256=archive_keyboard,
                     ansi_named_members=ansi_members),
        keyboard_source_sha256=sha(source_keyboard), installed_keyboard_sha256=sha(actual_keyboard),
        scope='Historical source and recipe evidence plus exact package identity; no behavior or source-to-binary provenance claim')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
