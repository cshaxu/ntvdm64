"""Check restored first-depth guards in the current mirror sources."""

import argparse
from pathlib import Path
import subprocess

repo = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('--scratch', required=True)
args = parser.parse_args()
scratch = (repo / args.scratch).resolve()
if not scratch.is_relative_to((repo / 'build').resolve()):
    raise SystemExit('scratch must be under the repository build directory')
scratch.mkdir(parents=True, exist_ok=True)

cases = [
    ('src/non-mvdm/base/win32/client/vdm.c', 1084,
     '#ifdef OLD_CFG_BASED', 'Excluded unsupported original OLD_CFG_BASED configuration',
     [([], False), (['OLD_CFG_BASED=1'], True)]),
    ('src/non-mvdm/windows/core/ntuser/kernel/input.c', 1154,
     '#ifdef SYSMODALWINDOWS', 'Excluded unsupported original SYSMODALWINDOWS branch',
     [([], False), (['SYSMODALWINDOWS=1'], True)]),
    ('src/non-mvdm/windows/core/ntuser/kernel/class.c', 1070,
     '#ifdef DBG', 'Unsupported DBG original branch',
     [([], False), (['DBG=0'], True), (['DBG=1'], True)]),
    ('src/non-mvdm/windows/core/ntuser/kernel/handtabl.c', 286,
     '#if (DBG || TRACE_OBJECT_ALLOCS)', 'Excluded unsupported original TRACE_OBJECT_ALLOCS branch',
     [([], False), (['DBG=1'], True), (['TRACE_OBJECT_ALLOCS=1'], True)]),
    ('src/non-mvdm/windows/core/ntuser/rtl/wow.c', 262,
     '#if defined(DEBUG) && !defined(_USERK_) && !defined(WOW)',
     'Unsupported DEBUG original branch',
     [([], False), (['DEBUG=1'], True), (['DEBUG=1', '_USERK_=1'], False),
      (['DEBUG=1', 'WOW=1'], False)]),
    ('src/non-mvdm/windows/core/ntuser/kernel/input.c', 1764,
     '#if (CBKEYSTATERECENTDOWN % 4) != 0',
     '"CBKEYSTATERECENTDOWN assumed to be an integral number of DWORDs"',
     [([], False), (['CBKEYSTATERECENTDOWN=32'], False),
      (['CBKEYSTATERECENTDOWN=31'], True)]),
    ('src/non-mvdm/windows/core/ntuser/inc/ntcftxt.h', 362,
     '#ifdef UNICODE', 'Excluded unsupported original UNICODE branch',
     [([], False), (['UNICODE=1'], True)]),
]
checked = 0
for path, line, guard, error, controls in cases:
    source = (repo / path).read_text(encoding='latin1').split('\n')
    frames = []
    for i in range(len(source) - 3):
        if source[i].strip() != f'#line {line}' or source[i + 1].strip() != guard:
            continue
        end = i + 2
        if source[end].strip() == f'#line {line + 1}':
            end += 1
        if (source[end].strip() == f'#error {error}'
                and source[end + 1].strip().startswith('#endif')):
            frames.append(source[i:end + 2])
    if len(frames) != 1:
        raise SystemExit(f'guard not unique: {path}:{line}')
    probe = scratch / 'additional-guard.c'
    probe.write_text('\n'.join(frames[0]) + '\n', encoding='ascii')
    for defines, reject in controls:
        command = ['cl.exe', '/nologo', '/EP', '/TC'] + [f'/D{x}' for x in defines] + [str(probe)]
        result = subprocess.run(command, capture_output=True, text=True, errors='replace')
        diagnostic = result.stdout + result.stderr
        if bool(result.returncode) != reject:
            raise SystemExit(f'unexpected result: {path} {defines}: {diagnostic}')
        if reject and (f'({line + 1})' not in diagnostic or error not in diagnostic):
            raise SystemExit(f'wrong diagnostic: {path} {defines}: {diagnostic}')
        checked += 1
print(f'PASS {len(cases)} original guards and {checked} profile controls')
