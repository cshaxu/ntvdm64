"""One isolated open/close per mode; audio buffers are never submitted."""
import hashlib
import json
import pathlib
import re
import subprocess
import sys

build = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else 'build/t473-s10/winmm-wave-handle')
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
source = pathlib.Path('artifacts/mvdm-audit/host-ch-scope/wow-winmm-wave-handle-fixture.c')
recipe = pathlib.Path('artifacts/mvdm-audit/host-ch-scope/wow-winmm-wave-handle-build.cmd')
winmm = pathlib.Path('C:/Windows/SysWOW64/winmm.dll')
obj = pathlib.Path('build/.ninja/msvc-x86-release/wow32/obj/wmmedia.obj')
mapfile = build / 'wrapper.map'
maps = [line.strip() for line in mapfile.read_text(encoding='utf-8', errors='replace').splitlines()
        if '_WOW32ResolveHandle@24' in line]
assert maps and any('wmmedia.obj' in line for line in maps)
before = sha(winmm)
runs = []
for mode, name in (('provider', 'fixture.exe'), ('wrapper', 'wrapper.exe')):
    binary = build / name
    assert 'pei-i386' in subprocess.check_output(['objdump', '-f', str(binary)]).decode(errors='replace')
    result = subprocess.run([str(binary)], capture_output=True)
    output = result.stdout.decode('utf-8', errors='replace').strip()
    fields = dict(re.findall(r'(\w+)=([^\s]+)', output))
    runs.append({'mode': mode, 'executable_sha256': sha(binary), 'exit_code': result.returncode,
                 'stdout': output, 'stderr': result.stderr.decode('utf-8', errors='replace').strip(),
                 'fields': fields})
    if result.returncode == 0:
        assert fields.get('mode') == mode and fields.get('open') == '0' and fields.get('close') == '0'
        assert fields.get('handle_nonzero') == '1' and fields.get('stack_equal') == '1'
        assert 'syswow64\\winmm.dll' in output.lower()
after = sha(winmm)
assert before == after
print(json.dumps({'winmm_path': str(winmm), 'winmm_sha256_before_after': [before, after],
                  'source_sha256': sha(source), 'recipe_sha256': sha(recipe),
                  'selected_wmmedia_object_sha256': sha(obj), 'wrapper_map_sha256': sha(mapfile),
                  'wrapper_map_lines': maps, 'runs': runs,
                  'limits': 'Each process independently opens and closes one host wave output handle, no sound buffers; ordinary Win32 handle is not proof of a valid Win16 alias. A device-open failure is recorded, not fabricated success.'}, indent=2))
