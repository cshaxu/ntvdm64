"""Compare one selected COFF function with the published PE, masking relocations."""
import hashlib
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
obj = ROOT / 'build/.ninja/msvc-x86-release/worker/obj/xms/xmsblock.obj'
exe = ROOT / 'build/package-uuid/package-c/system32/ntvdm.exe'
blob, image = obj.read_bytes(), exe.read_bytes()
assert hashlib.sha256(blob).hexdigest().upper() == '76D1E79FDE8F824EA36891E55816BAF461F9E25964CF1295E7826EDE3BCC63EF'
assert hashlib.sha256(image).hexdigest().upper() == '01A5B459072CA23F49AA43285580BD9017395C936F9DA8B2F18EE7223A52D1F6'
machine, nsec, _, _, _, opt, _ = struct.unpack_from('<HHIIIHH', blob, 0)
assert machine == 0x14c
sections = []
for i in range(nsec):
    off = 20 + opt + i * 40
    size, raw, reloc, _, nreloc, _ = struct.unpack_from('<IIIIHH', blob, off + 16)
    body = blob[raw:raw + size]
    if body[:1] == b'\x51' and body[6:16] == bytes.fromhex('0f b7 c0 c1 e0 0a 85 c0 74 30'):
        mask = bytearray(b'\x01' * len(body))
        for j in range(nreloc):
            address, _, kind = struct.unpack_from('<IIH', blob, reloc + j * 10)
            assert kind in (6, 20) and address + 4 <= len(body)
            mask[address:address + 4] = b'\0' * 4
        sections.append((body, mask, nreloc))
assert len(sections) == 1
body, mask, nreloc = sections[0]
assert len(body) == 83 and nreloc == 7
peoff = struct.unpack_from('<I', image, 0x3c)[0]
assert image[peoff:peoff + 4] == b'PE\0\0'
_, count, _, _, _, optsize, _ = struct.unpack_from('<HHIIIHH', image, peoff + 4)
executable = []
for i in range(count):
    off = peoff + 24 + optsize + 40 * i
    rawsize, rawptr = struct.unpack_from('<II', image, off + 16)
    characteristics = struct.unpack_from('<I', image, off + 36)[0]
    if characteristics & 0x20000000:
        executable.append((rawptr, rawptr + rawsize))
hits = []
for begin, end in executable:
    pos = begin
    while True:
        pos = image.find(body[:1], pos, end)
        if pos < 0 or pos + len(body) > end:
            break
        if all(not mask[j] or image[pos + j] == body[j] for j in range(len(body))):
            hits.append(pos)
        pos += 1
assert hits == [0x1540b0], hits
print('83-byte selected function matches published executable at file offset 0x1540b0; 7 relocation fields masked')
