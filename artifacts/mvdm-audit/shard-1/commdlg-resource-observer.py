"""Read-only COMMdlg resource/bitmap provenance and retained image check."""
import csv
import hashlib
import json
import struct
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
ROOT = Path('src/mvdm/wow16/commdlg')
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm/wow16/commdlg'),
             Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow16/commdlg'))
BITMAPS = ('cddrv.bmp', 'smfldr1.bmp', 'smnet.bmp', 'smram.bmp')


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def bitmap(data):
    assert data[:2] == b'BM'
    off = struct.unpack_from('<I', data, 10)[0]
    header = struct.unpack_from('<I', data, 14)[0]
    width, height, planes, bpp = struct.unpack_from('<iiHH', data, 18)
    compression = struct.unpack_from('<I', data, 30)[0]
    assert header == 40 and width > 0 and height > 0 and planes == 1 and bpp == 4 and compression == 0
    stride = ((width * bpp + 31) // 32) * 4
    assert off >= 54 and off + stride * height <= len(data)
    rows = []
    for y in range(height):
        line = data[off + y * stride:off + (y + 1) * stride]
        rows.append([line[x // 2] >> (4 if x % 2 == 0 else 0) & 15 for x in range(width)])
    return width, height, data[54:off], rows


def ne_bitmap(image, wanted):
    assert image[:2] == b'MZ'
    ne = struct.unpack_from('<I', image, 0x3c)[0]
    assert image[ne:ne + 2] == b'NE'
    table = ne + struct.unpack_from('<H', image, ne + 0x24)[0]
    shift = struct.unpack_from('<H', image, table)[0]
    pos, found = table + 2, []
    while True:
        typ = struct.unpack_from('<H', image, pos)[0]
        pos += 2
        if not typ:
            break
        count = struct.unpack_from('<H', image, pos)[0]
        pos += 6
        for _ in range(count):
            offset, length, flags, ident, _, _ = struct.unpack_from('<6H', image, pos)
            pos += 12
            if typ == 0x8002 and ident == (0x8000 | wanted):
                start, size = offset << shift, length << shift
                assert start + size <= len(image)
                found.append((start, size, flags, image[start:start + size]))
    assert len(found) == 1
    return found[0]


def inspect():
    with (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig', newline='') as f:
        frozen = {r['relative_path'].lower(): r for r in csv.DictReader(f)}
    records = []
    for name in BITMAPS + ('COMMDLG.RES',):
        relative = 'wow16/commdlg/' + name
        record = frozen[relative.lower()]
        current = (ROOT / name).read_bytes()
        value = sha(current)
        assert value == sha((ORIGINALS[0] / name).read_bytes()) == sha((ORIGINALS[1] / name).read_bytes())
        assert value == record['current_sha256'] == record['opennt_sha256'] == record['opennt45_sha256']
        assert record['dos_catalog_rows'] == '1' and record['wow_catalog_rows'] == '2'
        records.append(dict(relative_path=relative, sha256=value, wow_source_ids=record['wow_source_ids']))
    strip_bytes = (ROOT / 'filebmps.bmp').read_bytes()
    assert all(strip_bytes == (o / 'filebmps.bmp').read_bytes() for o in ORIGINALS)
    width, height, palette, strip = bitmap(strip_bytes)
    assert (width, height) == (128, 16)
    for record in records:
        name = Path(record['relative_path']).name
        if name not in BITMAPS:
            continue
        w, h, colours, pixels = bitmap((ROOT / name).read_bytes())
        assert (w, h) == (16, 16) and colours == palette
        record['matching_strip_tiles'] = [tile for tile in range(8)
            if all(pixels[y] == strip[y][tile * 16:(tile + 1) * 16] for y in range(16))]
    assert [r['matching_strip_tiles'] for r in records[:4]] == [[5], [], [6], [7]]
    recipe = (ROOT / 'makefile').read_bytes()
    rc = (ROOT / 'messages/usa/commdlg.rc').read_bytes()
    assert recipe == (ORIGINALS[0] / 'makefile').read_bytes()
    assert rc == (ORIGINALS[0] / 'messages/usa/commdlg.rc').read_bytes()
    assert b'bmpDirDrive    BITMAP  IMPURE filebmps.bmp' in rc
    assert b'rc16 -30 -t commdlg.res commdlg.dll' in recipe
    source_dll = (ROOT / 'COMMDLG.DLL').read_bytes()
    with zipfile.ZipFile('assets/winnt.zip') as archive:
        zip_dll = archive.read('WINNT/SYSTEM32/COMMDLG.DLL')
    installed = Path('O:/winnt/system32/COMMDLG.DLL').read_bytes()
    assert source_dll == zip_dll == installed
    start, size, flags, resource = ne_bitmap(source_dll, 576)
    assert resource.startswith(strip_bytes[14:])
    return dict(result='pass', records=records,
        strip=dict(sha256=sha(strip_bytes), width=width, height=height, palette_sha256=sha(palette)),
        recipe_sha256=sha(recipe), rc_sha256=sha(rc),
        image=dict(source='src/mvdm/wow16/commdlg/COMMDLG.DLL', sha256=sha(source_dll),
                   archive='WINNT/SYSTEM32/COMMDLG.DLL', installed='O:/winnt/system32/COMMDLG.DLL',
                   bitmap_id=576, bitmap_offset=start, bitmap_size=size, bitmap_flags=flags,
                   strip_dib_sha256=sha(strip_bytes[14:])),
        scope='Exact image resource and palette-index comparison; no guest execution, authoring provenance or visual behavior claim')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
