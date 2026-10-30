"""Read the selected PE and map; no guest code is run or modified."""
from pathlib import Path
import hashlib
import re
import struct

root = Path(__file__).resolve().parents[4]
image = root / 'build/.ninja/msvc-x86-release/worker/ntvdm.exe'
mapping = image.with_name('ntvdm.exe.map')
data = image.read_bytes()
maptext = mapping.read_text(errors='replace')
def symbol(name):
    match = re.search(r'(?m)^\s*[0-9a-f]+:[0-9a-f]+\s+_' + name + r'\s+([0-9a-f]+)\s', maptext, re.I)
    if not match:
        raise ValueError('Missing symbol ' + name)
    return int(match.group(1), 16)
pe = struct.unpack_from('<I', data, 0x3c)[0]
count = struct.unpack_from('<H', data, pe + 6)[0]
optional = pe + 24
base = struct.unpack_from('<I', data, optional + 28)[0]
sections = optional + struct.unpack_from('<H', data, pe + 20)[0]
rva = symbol('BIOS') - base + 0xb1 * 4
for i in range(count):
    at = sections + 40 * i
    virtual_size, virtual_address, raw_size, raw_pointer = struct.unpack_from('<IIII', data, at + 8)
    if virtual_address <= rva < virtual_address + min(virtual_size, raw_size):
        offset = raw_pointer + rva - virtual_address
        break
else:
    raise ValueError('BOP slot is outside the mapped PE data')
actual = struct.unpack_from('<I', data, offset)[0]
expected = symbol('virtual_device_trap')
if actual != expected:
    raise ValueError('Selected BOP B1 does not target virtual_device_trap')
print('BIOS_BOP_B1_SELECTED table=%08X slot=%08X target=%08X image_sha256=%s' % (
    symbol('BIOS'), actual, expected, hashlib.sha256(data).hexdigest().upper()))
