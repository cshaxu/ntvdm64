"""Observe MSCDEX guest/provider package paths without loading either image."""
import hashlib
import json
import re
import subprocess
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
historical_vdd = root / 'vdd/obj/i386/vcdex.dll'
installed_worker = Path('O:/winnt/system32/ntvdm.exe')
vdd_pe = subprocess.check_output(['objdump', '-p', str(historical_vdd)], text=True)
worker_pe = subprocess.check_output(['objdump', '-p', str(installed_worker)], text=True)
import_text = vdd_pe.split('DLL Name: ntvdm.exe', 1)[1].split('DLL Name:', 1)[0].split('There is an export table', 1)[0]
imports = re.findall(r'^\s*[0-9a-f]+\s+<none>\s+[0-9a-f]+\s+(\w+)\s*$', import_text, re.M)
export_text = worker_pe.split('[Ordinal/Name Pointer] Table', 1)[1]
exports = set(re.findall(r'^\s*\[\s*\d+\].*?\s+([A-Za-z_][\w@]*)\s*$', export_text, re.M))
assert imports
unresolved = sorted(set(imports) - exports)
print(json.dumps(dict(result='pass', files=files, zip_entries=contents,
                      provider_paths=[dict(path=str(p), exists=p.exists()) for p in provider_paths],
                      retained_vdd_sha256=digest(historical_vdd.read_bytes()),
                      installed_worker_sha256=digest(installed_worker.read_bytes()),
                      vdd_ntvdm_imports=imports, absent_worker_export_names=unresolved,
                      tsr_source_sha256=digest((root / 'tsr/mscdexnt.asm').read_bytes()),
                      autoexec_sha256=digest(autoexec.read_bytes()),
                      scope='Selected filesystem and guest-source observations; no actual DLL search, guest load or optical-device test'), indent=2))
