"""Read-only original public WOW extension and NT4 WINMM-provider search."""
import hashlib
import json
import pathlib
import re
import subprocess

sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
paths = [pathlib.Path(x) for x in (
    'src/mvdm/wow32/isvwow.h', 'src/mvdm/wow32/wmmedia.c',
    'O:/repos.external/OpenNT/base/mvdm/wow32/isvwow.h',
    'O:/repos.external/OpenNT/base/mvdm/wow32/wmmedia.c',
    'O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow32/isvwow.h',
    'O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow32/wmmedia.c',
    'O:/repos.external/OpenNT-4.5/nt/private/windows/media/winmm/mmwow32.c',
    'O:/repos.external/OpenNT-4.5/nt/private/windows/media/winmm/winmm.def',
    'build/.ninja/msvc-x86-release/wow32/wow32.dll')]
search_roots = ('O:/repos.external/OpenNT/base/mvdm',
                'O:/repos.external/OpenNT-4.5/nt/private/mvdm',
                'O:/repos.external/OpenNT-4.5/nt/private/windows')
args = ['rg', '-n', '-i', r'\bWOW32ResolveHandle\b', *search_roots,
        '-g', '*.c', '-g', '*.h', '-g', '*.cpp', '-g', '*.asm', '-g', '*.def']
result = subprocess.run(args, capture_output=True, check=True)
exports = subprocess.run(['objdump', '-p', str(paths[-1])], capture_output=True, check=True).stdout
selected = [line.strip() for line in exports.decode('utf-8', errors='replace').splitlines()
            if re.search(r'\b(?:WOW32ResolveHandle|WOW32DriverCallback)\s*$', line)]
print(json.dumps({'inputs': [{'path': str(p).replace('\\', '/'), 'sha256': sha(p)} for p in paths],
                  'search_roots': search_roots,
                  'search_extensions': ['c', 'h', 'cpp', 'asm', 'def'],
                  'resolve_handle_hits': sorted(result.stdout.decode('utf-8', errors='replace').splitlines()),
                  'cached_wow32_exports': selected,
                  'limits': 'Bounded source search and cached PE exports; dynamically obtained names, binaries, other roots, real guest reachability and modern WINMM ABI/behavior unproved.'}, indent=2))
