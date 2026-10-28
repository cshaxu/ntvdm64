"""Read-only cross-component guest resource identity and recipe observer."""
import csv
import hashlib
import json
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
ROOT = Path('src/mvdm/wow16')
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm/wow16'),
             Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow16'))
CASES = (
    ('mmsystem', 'MMSYSTEM.RES', 'MMSYSTEM.DLL'),
    ('shell', 'SHELL.RES', 'SHELL.DLL'),
    ('system', 'SYSTEM.RES', 'SYSTEM.DRV'),
    ('winoldap', 'WINOLDAP.RES', 'WINOLDAP.MOD'),
    ('winspool', 'WINSPOOL.RES', 'WINSPOOL.EXE'),
    ('ole/server', 'SRVR.RES', 'OLESVR.DLL'),
    ('drivers/mouse', 'MOUSE.RES', 'MOUSE.DRV'),
)
RECIPE_MARKERS = {
    'mmsystem': ('name    =mmsystem', '$(name).res:', '$(rc) -t $(name).res $(name).$(ext)'),
    'shell': ('shell.dll shell.map:', 'shell.res', 'rc16 -t shell.res shell.dll'),
    'system': ('system.drv:', 'system.res', 'rc16 -t system.res system.exe'),
    'winoldap': ('winoldap.mod:', 'winoldap.res', 'rc16 -t winoldap.res winoldap.mod'),
    'winspool': ('winspool.exe winspool.map:', 'winspool.res', 'rc16 -t winspool.res winspool.exe'),
    'ole/server': ('olesvr.dll:', 'srvr.res', 'rc16 -t -30 $(include) ..\\srvr.res olesvr.dll'),
    'drivers/mouse': ('mouse.exe mouse.map:', 'mouse.res', 'copy mouse.exe mouse.drv'),
}


def digest(data):
    return hashlib.sha256(data).hexdigest().upper()


def inspect():
    with (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig', newline='') as stream:
        frozen = {r['relative_path'].lower(): r for r in csv.DictReader(stream)}
    records = []
    with zipfile.ZipFile('assets/winnt.zip') as archive:
        archive_names = {n.lower(): n for n in archive.namelist()}
        for directory, resource, image in CASES:
            relative = 'wow16/' + directory + '/' + resource
            original = frozen[relative.lower()]
            value = digest((ROOT / directory / resource).read_bytes())
            assert value == digest((ORIGINALS[0] / directory / resource).read_bytes())
            assert value == digest((ORIGINALS[1] / directory / resource).read_bytes())
            assert value == original['current_sha256'] == original['opennt_sha256'] == original['opennt45_sha256']
            assert original['dos_catalog_rows'] == '1' and original['wow_catalog_rows'] == '2'
            recipe = (ROOT / directory / 'makefile').read_bytes()
            assert all(recipe == (o / directory / 'makefile').read_bytes() for o in ORIGINALS)
            assert all(marker in recipe.decode('latin1').lower() for marker in RECIPE_MARKERS[directory])
            source = ROOT / directory / image
            installed = Path('O:/winnt/system32') / image
            zip_name = archive_names.get(('WINNT/SYSTEM32/' + image).lower())
            zip_bytes = archive.read(zip_name) if zip_name else None
            records.append(dict(relative_path=relative, resource_sha256=value,
                wow_source_ids=original['wow_source_ids'], recipe_sha256=digest(recipe),
                image=image, source_image_sha256=digest(source.read_bytes()) if source.is_file() else None,
                archive_entry=zip_name, archive_image_sha256=digest(zip_bytes) if zip_bytes is not None else None,
                installed_image_sha256=digest(installed.read_bytes()) if installed.is_file() else None))
        relative = 'wow16/winspool/WINSPOOL.EXE'
        original = frozen[relative.lower()]
        source = ROOT / 'winspool/WINSPOOL.EXE'
        value = digest(source.read_bytes())
        assert value == digest((ORIGINALS[0] / 'winspool/WINSPOOL.EXE').read_bytes())
        assert value == digest((ORIGINALS[1] / 'winspool/WINSPOOL.EXE').read_bytes())
        assert value == original['current_sha256'] == original['opennt_sha256'] == original['opennt45_sha256']
        assert original['dos_catalog_rows'] == '1' and original['wow_catalog_rows'] == '2'
        assert value == records[4]['archive_image_sha256'] == records[4]['installed_image_sha256']
    return dict(result='pass', resources=records, winspool_exe_sha256=value,
        winspool_wow_source_ids=original['wow_source_ids'],
        scope='Read-only original recipe and current image identities; no .res-to-image provenance, load, output or runtime behavior claim')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
