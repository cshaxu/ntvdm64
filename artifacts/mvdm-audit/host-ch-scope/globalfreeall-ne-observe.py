"""Read-only NE entry and near-call observation; no guest extraction or build."""
import hashlib
import json
import pathlib
import struct
import zipfile

root = pathlib.Path(__file__).resolve().parents[3]
with zipfile.ZipFile(root / 'assets/winnt.zip') as package:
    image = package.read('WINNT/SYSTEM32/KRNL386.EXE')
u16 = lambda offset: struct.unpack_from('<H', image, offset)[0]
ne = struct.unpack_from('<I', image, 0x3c)[0]
if image[ne:ne + 2] != b'NE':
    raise ValueError('not an NE image')
cursor = ne + u16(ne + 4)
ordinal = 1
entry = None
while True:
    count, segment = image[cursor:cursor + 2]
    cursor += 2
    if not count:
        break
    for _ in range(count):
        if segment == 0:
            current = None
        elif segment == 255:
            current = (image[cursor], image[cursor + 3], u16(cursor + 4))
            cursor += 6
        else:
            current = (image[cursor], segment, u16(cursor + 1))
            cursor += 3
        if ordinal == 26:
            entry = current
        ordinal += 1
if entry is None:
    raise ValueError('ordinal 26 absent')
flags, segment, offset = entry
segment_table = ne + u16(ne + 0x22) + 8 * (segment - 1)
base = u16(segment_table) << u16(ne + 0x32)
call_offset = 0x1097
if image[base + call_offset] != 0xe8:
    raise ValueError('expected near-call opcode not present')
displacement = struct.unpack_from('<h', image, base + call_offset + 1)[0]
print(json.dumps({'zip_member':'WINNT/SYSTEM32/KRNL386.EXE', 'image_sha256':hashlib.sha256(image).hexdigest(), 'ne_offset':ne, 'ordinal':26, 'entry_flags':flags, 'segment':segment, 'entry_offset':offset, 'call_offset':call_offset, 'call_bytes':image[base+call_offset:base+call_offset+3].hex(), 'near_call_target':(call_offset+3+displacement)&0xffff, 'limits':'one packaged image; map address correspondence is not full source/image identity or runtime task-exit evidence'}, indent=2))
