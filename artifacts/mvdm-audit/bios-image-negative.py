"""Reject a wrong BIOS pointer in a disposable project-host image, never run it."""
import json
import re
import runpy
import struct
from pathlib import Path

helpers = runpy.run_path('artifacts/mvdm-audit/bios-image-observer.py')
original = helpers['IMAGE']
observed = json.loads(Path('artifacts/mvdm-audit/bios-image-observations.json').read_text())
map_text = (helpers['BUILD'] / 'ntvdm.exe.map').read_text()
match = re.search(r'(?m)^\s+\w+:\w+\s+_BIOS\s+([0-9a-fA-F]+)\s+original-softpc-bios:bios.obj\s*$', map_text)
assert match
address = int(match[1], 16) + 255 * 4
data = bytearray(original.read_bytes())
pe = struct.unpack_from('<I', data, 60)[0]
base = struct.unpack_from('<I', data, pe + 52)[0]
sections = struct.unpack_from('<H', data, pe + 6)[0]
optional = struct.unpack_from('<H', data, pe + 20)[0]
positions = []
for index in range(sections):
    header = pe + 24 + optional + 40 * index
    rva, size, raw = struct.unpack_from('<III', data, header + 12)
    delta = address - base - rva
    if 0 <= delta and delta + 4 <= size:
        positions.append(raw + delta)
assert len(positions) == 1
offset = positions[0]
old = struct.unpack_from('<I', data, offset)[0]
wrong = observed['bios_slots'][0xf0]['va']
assert old == observed['bios_slots'][0xff]['va'] and old != wrong
struct.pack_into('<I', data, offset, wrong)
target = Path('build/t473-s10/bios-image-negative/changed-dispatch.exe')
assert target.parent.is_dir() and not target.exists()
target.write_bytes(data)
try:
    helpers['inspect'](target)
except AssertionError as error:
    assert error.args == ((255, '_control_bop'),), error.args
else:
    raise AssertionError('Wrong BIOS pointer was accepted')
result = dict(result='pass', expected_rejection='BIOS slot 255 relocation target mismatch',
              original_image_sha256=helpers['sha'](original.read_bytes()),
              changed_image_sha256=helpers['sha'](target.read_bytes()), file_offset=offset,
              original_pointer=old, wrong_pointer=wrong, executed=False, guest_changed=False)
Path('artifacts/mvdm-audit/bios-image-negative-result.json').write_text(json.dumps(result, indent=2) + '\n')
print(json.dumps(result))
