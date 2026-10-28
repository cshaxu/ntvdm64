"""Run isolated x86 null/invalid-handle observations against local WINMM."""
import hashlib
import json
import pathlib
import subprocess
import sys

sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
build = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else 'build/t473-s10/winmm-real-null')
fixture = build / 'fixture.exe'
obj = build / 'fixture.obj'
wrapper = build / 'wrapper.exe'
wrapper_obj = build / 'wrapper.obj'
wrapper_map = build / 'wrapper.map'
selected_obj = pathlib.Path('build/.ninja/msvc-x86-release/wow32/obj/wmmedia.obj')
source = pathlib.Path('artifacts/mvdm-audit/host-ch-scope/wow-winmm-real-null-fixture.c')
recipe = pathlib.Path('artifacts/mvdm-audit/host-ch-scope/wow-winmm-real-null-build.cmd')
winmm = pathlib.Path('C:/Windows/SysWOW64/winmm.dll')
before = sha(winmm)
architecture = subprocess.check_output(['objdump', '-f', str(fixture)]).decode('utf-8', errors='replace')
assert 'pei-i386' in architecture
wrapper_architecture = subprocess.check_output(['objdump', '-f', str(wrapper)]).decode('utf-8', errors='replace')
assert 'pei-i386' in wrapper_architecture
results = []
for mode, executable in (('provider', fixture), ('wrapper', wrapper)):
    for case in ('zero16', 'invalid16', 'zero32', 'invalid32'):
        result = subprocess.run([str(executable), case], capture_output=True)
        output = result.stdout.decode('utf-8', errors='replace').strip()
        results.append({'mode': mode, 'case': case, 'exit_code': result.returncode, 'stdout': output,
                        'stderr': result.stderr.decode('utf-8', errors='replace').strip()})
        assert result.returncode == 0 and 'stack_equal=1' in output
        assert 'mode=' + mode in output and 'syswow64\\winmm.dll' in output.lower()
after = sha(winmm)
assert before == after
map_lines = [line.strip() for line in wrapper_map.read_text(encoding='utf-8', errors='replace').splitlines()
             if '_WOW32ResolveHandle@24' in line]
assert map_lines and any('wmmedia.obj' in line for line in map_lines)
print(json.dumps({'winmm_path': str(winmm), 'winmm_sha256_before_after': [before, after],
                  'fixture_sha256': sha(fixture), 'fixture_obj_sha256': sha(obj),
                  'wrapper_sha256': sha(wrapper), 'wrapper_obj_sha256': sha(wrapper_obj),
                  'wrapper_map_sha256': sha(wrapper_map), 'wrapper_map_lines': map_lines,
                  'selected_wmmedia_obj_sha256': sha(selected_obj),
                  'source_sha256': sha(source), 'build_recipe_sha256': sha(recipe),
                  'fixture_architecture': 'pei-i386', 'wrapper_architecture': 'pei-i386', 'results': results,
                  'limits': 'Independent processes and test-owned null/small-invalid handles; wrapper links cached unmodified wmmedia.obj with explicitly preloaded WINMM and fail-fast unrelated seams. Private ABI assumption; no real guest/device, valid handles, callback or other host versions.'}, indent=2))
