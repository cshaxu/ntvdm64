"""Read-only snapshot of WOW multimedia owners and named host PE exports."""
import hashlib
import json
import pathlib
import re
import subprocess

sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
names = ('NotifyCallbackData', 'wod32Message', 'wid32Message', 'mod32Message',
         'mid32Message', 'aux32Message', 'mci32Message', 'tid32Message',
         'joy32Message', 'WOW32DriverCallback', 'WOW32ResolveMultiMediaHandle')
winmm = pathlib.Path('C:/Windows/SysWOW64/winmm.dll')
exports = subprocess.run(['objdump', '-p', str(winmm)], capture_output=True, check=True).stdout
lines = exports.decode('utf-8', errors='replace').splitlines()
present = {n: [line.strip() for line in lines if re.search(r'\b' + re.escape(n) + r'\s*$', line)]
           for n in names}
files = ('src/mvdm/wow32/wmmalias.c', 'src/mvdm/wow32/wmmedia2.c',
         'src/mvdm/wow32/wmmstru2.c', 'src/mvdm/wow32/wmmstruc.h',
         'src/mvdm/wow32/wmmedia.c', 'src/mvdm/wow32/wmmtbl2.h',
         'src/mvdm/wow16/mmsystem/init.c', 'src/mvdm/wow16/mmsystem/stack.asm',
         'src/mvdm/inc/wowmmcb.h',
         'docs/etc/evidence/m0-t466-s3-async-multimedia-owner-audit.md')
build = pathlib.Path('build/.ninja/msvc-x86-release/wow32')
mapfile = build / 'wow32.dll.map'
maplines = [line.strip() for line in mapfile.read_text(encoding='utf-8', errors='replace').splitlines()
            if any(n in line for n in ('_WMM32CallProc32', '_wow_original_WMM32CallProc32',
                                       '_GetMMedia16', '_GetMMedia32', '_FreeMMedia16'))]
print(json.dumps({'winmm_path': str(winmm), 'winmm_sha256': sha(winmm), 'named_exports': present,
                  'input_sha256': {p: sha(pathlib.Path(p)) for p in files},
                  'map_sha256': sha(mapfile), 'map_lines': maplines,
                  'limits': 'Named exports and source/map identity only; no calling private ABI, device operation, callbacks or current guest loaded-image proof.'}, indent=2))
