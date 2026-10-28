"""Compare cached, release and installed VDMREDIR PE file identities."""
import hashlib
import json
import struct
from pathlib import Path

PATHS = (Path('build/.ninja/msvc-x86-release/worker/VDMREDIR.dll'),
         Path('assets/release/VDMREDIR.DLL'), Path('O:/winnt/system32/VDMREDIR.DLL'))
def sha(b): return hashlib.sha256(b).hexdigest().upper()
def inspect_file(path):
    b = path.read_bytes(); pe = struct.unpack_from('<I', b, 0x3c)[0]
    assert b[:2] == b'MZ' and b[pe:pe+4] == b'PE\0\0'
    count = struct.unpack_from('<H', b, pe+6)[0]; optional = struct.unpack_from('<H', b, pe+20)[0]
    at = pe + 24 + optional; sections = []
    for i in range(count):
        entry = at + 40*i; name = b[entry:entry+8].rstrip(b'\0').decode('ascii')
        size, offset = struct.unpack_from('<II', b, entry+16)
        assert offset + size <= len(b)
        sections.append(dict(name=name, size=size, sha256=sha(b[offset:offset+size])))
    return dict(path=str(path), size=len(b), sha256=sha(b), timestamp=struct.unpack_from('<I', b, pe+8)[0], sections=sections)
def inspect():
    files = [inspect_file(p) for p in PATHS]
    assert files[1]['sha256'] == files[2]['sha256']
    return dict(result='pass', files=files,
                limits='PE section byte comparison only; no source provenance, data interpretation, runtime or semantic-equivalence claim')
if __name__ == '__main__': print(json.dumps(inspect(), indent=2))
