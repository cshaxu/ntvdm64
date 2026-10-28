"""Observe MSCDEX guest/provider package paths without loading either image."""
import hashlib
import json
import zipfile
from pathlib import Path


def digest(data):
    return hashlib.sha256(data).hexdigest().upper()


root = Path('src/mvdm/vdd/samples/mscdex')
source = (root / 'tsr/mscdexnt.asm').read_text(encoding='latin1')
assert 'DllName DB      "VCDEX.DLL",0' in source
assert 'call    RegisterVDD' in source and 'DispatchCall' in source
autoexec = Path('O:/winnt/system32/autoexec.nt')
assert 'mscdexnt.exe' in autoexec.read_text(encoding='latin1').lower()
paths = [root / 'tsr/mscdexnt.exe', Path('O:/winnt/system32/mscdexnt.exe')]
files = [dict(path=str(path), sha256=digest(path.read_bytes())) for path in paths]
assert files[0]['sha256'] == files[1]['sha256']
provider_paths = [Path('assets/release/VCDEX.DLL'), Path('O:/winnt/system32/VCDEX.DLL')]
with zipfile.ZipFile('assets/winnt.zip') as archive:
    entries = [name for name in archive.namelist() if name.lower().endswith(('/mscdexnt.exe', '/vcdex.dll'))]
    contents = {name: digest(archive.read(name)) for name in entries}
assert any(name.lower().endswith('/mscdexnt.exe') for name in entries)
print(json.dumps(dict(result='pass', files=files, zip_entries=contents,
                      provider_paths=[dict(path=str(p), exists=p.exists()) for p in provider_paths],
                      tsr_source_sha256=digest((root / 'tsr/mscdexnt.asm').read_bytes()),
                      autoexec_sha256=digest(autoexec.read_bytes()),
                      scope='Selected filesystem and guest-source observations; no actual DLL search, guest load or optical-device test'), indent=2))
