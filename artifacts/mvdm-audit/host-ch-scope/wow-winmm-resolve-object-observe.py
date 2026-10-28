"""Execute six isolated scenarios on selected wmmedia object with loader seams."""
import hashlib
import json
import pathlib
import subprocess
import sys

sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
original = pathlib.Path('build/.ninja/msvc-x86-release/wow32/obj/wmmedia.obj')
build = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else 'build/t473-s10/winmm-resolve-object')
seam = build / 'wmmedia-seam.obj'
fixture = build / 'fixture.exe'
inputs = [original, seam, fixture, build / 'fixture.obj', build / 'fixture.map',
          pathlib.Path('src/mvdm/wow32/wmmedia.c'),
          pathlib.Path('O:/repos.external/OpenNT/base/mvdm/wow32/wmmedia.c'),
          pathlib.Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow32/wmmedia.c'),
          pathlib.Path('src/mvdm/wow32/wmmedia.h'),
          pathlib.Path('artifacts/mvdm-audit/host-ch-scope/wow-winmm-resolve-object-fixture.c'),
          pathlib.Path('artifacts/mvdm-audit/host-ch-scope/wow-winmm-resolve-object-build.cmd'),
          pathlib.Path('build/.ninja/msvc-x86-release/wow32/build.ninja'),
          pathlib.Path('build/.ninja/msvc-x86-release/wow32/wow32.dll.map')]
disassembly = []
relocations = []
for obj in (original, seam):
    text = subprocess.check_output(['objdump', '-d', str(obj)]).decode('utf-8', errors='replace')
    disassembly.append('\n'.join(x for x in text[text.find('Disassembly of section'):].splitlines()
                                if x.strip() != '...'))
    text = subprocess.check_output(['objdump', '-r', str(obj)]).decode('utf-8', errors='replace')
    text = text[text.find('RELOCATION RECORDS'):]
    text = text.replace('__imp__LoadLibraryA@4', '_fake_imp_LoadLibraryA')
    text = text.replace('__imp__GetProcAddress@8', '_fake_imp_GetProcAddress')
    relocations.append(text)
assert disassembly[0] == disassembly[1] and relocations[0] == relocations[1]
expected = {
    'missingdll': 'missingdll resolve=6 callback=6 out16=aaaa out32=cccccccc calls=0',
    'missing': 'missing resolve=6 callback=6 out16=aaaa out32=cccccccc calls=0',
    'success16to32': 'success16to32 resolve=1 out16=aaaa out32=11112222 calls=1',
    'success32to16': 'success32to16 resolve=1 out16=2222 out32=cccccccc calls=1',
    'fail16to32': 'fail16to32 resolve=1 out16=aaaa out32=11112222 calls=1',
    'fail32to16': 'fail32to16 resolve=1 out16=2222 out32=cccccccc calls=1',
}
results = {}
for scenario, expected_line in expected.items():
    result = subprocess.run([str(fixture), scenario], capture_output=True, check=True)
    line = result.stdout.decode('ascii').strip()
    assert line == expected_line, (scenario, line, expected_line)
    results[scenario] = {'exit_code': result.returncode, 'stdout': line}
print(json.dumps({'inputs': [{'path': str(p).replace('\\', '/'), 'sha256': sha(p)} for p in inputs],
                  'instructions_equal_ignoring_objcopy_padding': True,
                  'relocations_equal_after_two_loader_import_renames': True,
                  'scenarios': results,
                  'limits': 'Controlled loader and WINMM provider seams; unmodified selected object instruction body, no unresolved link; not real guest/provider ABI, reachability, or current release identity.'}, indent=2))
