"""Observe absent historical build tools and parent-tool references; never execute them."""
import csv
import hashlib
import json
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
CURRENT = Path('src/mvdm')
ALT = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
NAMES = ('buildidx.exe', 'c1xx3216.exe', 'c23216.exe', 'c23pcd.exe',
         'implib.exe', 'link_60.exe', 'mkpublic.exe', 'rcpp.exe', 'reloc.exe')


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


with (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig', newline='') as f:
    frozen = {r['relative_path']: r for r in csv.DictReader(f)}
with (A / 'combined-initial-status.csv').open(encoding='utf-8-sig', newline='') as f:
    ledger = {r['relative_path']: r for r in csv.DictReader(f)}
with zipfile.ZipFile('assets/winnt.zip') as archive:
    archive_names = {Path(n.replace('\\', '/')).name.casefold() for n in archive.namelist()}
entries = []
for name in NAMES:
    rel = 'tools16/' + name
    original, current = ALT / rel, CURRENT / rel
    retained = Path('tools/historical/opennt-4.5') / name
    assert not current.exists() and original.is_file()
    assert sha(original) == frozen[rel]['opennt45_sha256']
    assert retained.is_file() and sha(retained) == sha(original)
    entries.append(dict(path=rel, status=ledger[rel]['status'], original_sha256=sha(original),
                        retained_path=str(retained), retained_sha256=sha(retained),
                        installed=(Path('O:/winnt/system32') / name).exists(),
                        in_archive=name.casefold() in archive_names))
compiler = (ALT / 'tools16/cl16.exe').read_bytes().lower()
resource_compiler = (ALT / 'tools16/rc16.exe').read_bytes().lower()
assert all(name.encode() in compiler for name in ('c1xx3216.exe', 'c23216.exe', 'c23pcd.exe'))
assert b'rcpp.exe' in resource_compiler
anchors = {
    'dos/v86/messages/makefile': 'tools16\\buildidx',
    'dos/v86/doskrnl/bios/makefile': 'reloc ntio.exe',
    'wow16/ole/client/makefile': 'mkpublic olecli.def',
    'wow16/lib/makefile': 'implib kernel.lib',
}
for path, needle in anchors.items():
    assert needle in (CURRENT / path).read_text(encoding='latin1').lower()
print(json.dumps(dict(result='pass', entries=entries,
    parent_images=[dict(path=str(ALT / ('tools16/' + n)), sha256=sha(ALT / ('tools16/' + n)))
                   for n in ('cl16.exe', 'rc16.exe')],
    recipe_inputs=[dict(path=path, sha256=sha(CURRENT / path)) for path in anchors],
    limits='Strings and recipes are static evidence only; exact historical tool selection and runtime behavior are not proved.'), indent=2))
