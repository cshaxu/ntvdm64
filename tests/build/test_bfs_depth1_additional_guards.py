"""Check restored first-depth guards in the current mirror sources."""

import argparse
from pathlib import Path
import re
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
    ('src/non-mvdm/windows/core/ntuser/kernel/queue.c', 785,
     '#ifdef DEBUG', 'DIVERGENCE OPENNT-HOST-146: DEBUG user profile is not selected',
     [([], False), (['DEBUG=1'], True)]),
    ('src/non-mvdm/windows/core/ntuser/client/globals.h', 18,
     '#if DBG', 'Excluded unsupported original DBG globals',
     [([], False), (['DBG=1'], True)]),
    ('src/non-mvdm/windows/core/ntuser/client/globals.h', 135,
     '#ifdef WX86', 'Excluded unsupported original WX86 globals',
     [([], False), (['WX86=1'], True)]),
    ('src/mvdm-host/softpc.new/host/inc/host_cpu.h', 163,
     '#ifdef A2CPU', 'Excluded unsupported original A2CPU register carrier',
     [([], False), (['A2CPU=1'], True)]),
    ('src/mvdm-host/softpc.new/host/inc/host_cpu.h', 188,
     '#ifdef A2CPU', 'Excluded unsupported original A2CPU register carrier',
     [([], False), (['A2CPU=1'], True)]),
    ('src/mvdm-host/softpc.new/base/video/video.c', 2857,
     '#ifndef MONITOR', 'Excluded unsupported original non-MONITOR stream buffer release',
     [([], True), (['MONITOR=1'], False)]),
    ('src/non-mvdm/public/sdk/inc/ntpsapi.h', 386,
     '#ifdef _IA64_', 'Excluded unsupported original IA64 process API profile',
     [([], False), (['_IA64_=1'], True)]),
    ('src/non-mvdm/public/sdk/inc/ntpsapi.h', 463,
     '#if DBG', 'Excluded unsupported original DBG process API profile',
     [([], False), (['DBG=1'], True)]),
    ('src/non-mvdm/windows/core/ntuser/rtl/userrtl.h', 42,
     '#if DBG', 'Excluded unsupported original DBG user RTL profile',
     [([], False), (['DBG=1'], True)]),
    ('src/non-mvdm/windows/core/ntuser/rtl/userrtl.h', 118,
     '#ifdef FE_SB', 'Excluded unsupported original FE_SB user RTL profile',
     [([], False), (['FE_SB=1'], True)]),
    ('src/non-mvdm/windows/core/ntuser/kernel/handtabl.c', 600,
     '#ifdef FE_IME', 'Unsupported FE_IME original branch',
     [([], False), (['FE_IME=1'], True)]),
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
    ('src/non-mvdm/windows/core/ntuser/client/usercli.h', 47,
     '#ifdef FE_IME', 'Excluded unsupported original USER client condition',
     [([], False), (['FE_IME=1'], True)]),
    ('src/non-mvdm/windows/core/ntuser/inc/user.h', 743,
     '#ifdef FE_IME', 'Excluded unsupported original USER condition',
     [([], False), (['FE_IME=1'], True)]),
    ('src/non-mvdm/windows/core/ntuser/kernel/userk.h', 1875,
     '#ifdef MEMPHIS_MENUS', 'Excluded unsupported original USER kernel condition',
     [([], False), (['MEMPHIS_MENUS=1'], True)]),
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

source = (repo / 'src/non-mvdm/windows/core/ntuser/client/clmsg.c').read_text(encoding='latin1').split('\n')
for original, selected_line, continuation in ((400, 413, 415), (416, 429, 431), (433, 447, 449)):
    start = source.index(f'#line {original}')
    expected = [f'#line {original}', '#ifdef FE_SB // SendMessageWorker()', f'#line {original + 1}',
                '#error Excluded unsupported original FE_SB branch', '#else', f'#line {selected_line}',
                '#endif // FE_SB', f'#line {continuation}']
    if [line.strip() for line in source[start:start + len(expected)]] != expected:
        raise SystemExit(f'clmsg original nested guard differs at {original}')
    probe = scratch / 'clmsg-nested-guard.c'
    probe.write_text('\n'.join(expected[:6] + ['int selected_line = __LINE__;'] + expected[6:] +
                               ['int after_guard = __LINE__;']) + '\n', encoding='ascii')
    selected = subprocess.run(['cl.exe', '/nologo', '/EP', '/TC', str(probe)], capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(['cl.exe', '/nologo', '/EP', '/TC', '/DFE_SB=1', str(probe)], capture_output=True, text=True, errors='replace')
    if (selected.returncode or not re.search(r'int\s+selected_line\s*=\s*' + str(selected_line) + r'\s*;', selected.stdout)
            or not re.search(r'int\s+after_guard\s*=\s*' + str(continuation) + r'\s*;', selected.stdout)
            or not rejected.returncode or 'Excluded unsupported original FE_SB branch' not in rejected.stdout + rejected.stderr):
        raise SystemExit(f'clmsg nested profile controls failed at {original}')
print('PASS three original clmsg nested FE_SB guards and line controls')

source = (repo / 'src/mvdm-host/softpc.new/base/video/video.c').read_text(encoding='latin1').split('\n')
start = source.index('#line 1511')
expected = ['#line 1511',
            '#if ( defined(NTVDM) && defined(MONITOR) && !defined(MVDM_STANDALONE_SAS_VIDEO) ) || defined(GISP_SVGA)/* No Ega planes... */',
            '#line 1512', '#error Excluded unsupported original monitor or GISP video pointer',
            '#else', '#line 1514']
if [line.strip() for line in source[start:start + len(expected)]] != expected:
    raise SystemExit('video original pointer boundary differs')
probe = scratch / 'video-pointer-guard.c'
probe.write_text('\n'.join(expected + ['int selected = 1;', '#endif']) + '\n', encoding='ascii')
for defines, reject in ((['NTVDM=1', 'MONITOR=1', 'MVDM_STANDALONE_SAS_VIDEO=1'], False),
                        (['NTVDM=1', 'MONITOR=1'], True), (['GISP_SVGA=1'], True)):
    result = subprocess.run(['cl.exe', '/nologo', '/EP', '/TC'] + [f'/D{x}' for x in defines] + [str(probe)],
                            capture_output=True, text=True, errors='replace')
    if bool(result.returncode) != reject or (reject and 'Excluded unsupported original monitor or GISP video pointer' not in result.stdout + result.stderr):
        raise SystemExit(f'video pointer profile failed: {defines}')
print('PASS video standalone SAS and excluded monitor/GISP pointer profiles')

for filename, original, continuation, carrier in (
    ('egawrtm0.c', 209, 318, 'mode0_copy_handlers'),
    ('egwrtm12.c', 156, 257, 'mode1_handlers'),
):
    source = (repo / 'src/mvdm-host/softpc.new/base/video' / filename).read_text(encoding='latin1').split('\n')
    start = source.index(f'#line {original}')
    end = next(i for i in range(start, len(source)) if source[i].strip() == '#endif /* A_VID */')
    if source[start + 1].strip() != '#ifdef A_VID' or source[end + 1].strip() != f'#line {continuation}':
        raise SystemExit(f'{filename}: original A_VID frame differs')
    probe = scratch / 'video-table-guard.c'
    probe.write_text('\n'.join(source[start:end + 2] + ['int after_table = __LINE__;']) + '\n', encoding='ascii')
    for defines, reject, marker in ((['CPU_40_STYLE=1'], False, carrier),
                                    (['A_VID=1', 'CPU_40_STYLE=1'], True, 'Excluded unsupported original A_VID writer-table profile'),
                                    ([], True, 'Excluded unsupported non-CPU40 writer-table profile')):
        result = subprocess.run(['cl.exe', '/nologo', '/EP', '/TC'] + [f'/D{x}' for x in defines] + [str(probe)],
                                capture_output=True, text=True, errors='replace')
        if (bool(result.returncode) != reject or marker not in result.stdout + result.stderr
                or (not reject and not re.search(r'int\s+after_table\s*=\s*' + str(continuation) + r'\s*;', result.stdout))):
            raise SystemExit(f'{filename}: writer-table profile failed for {defines}')
print('PASS original A_VID frames and selected CPU40 table carriers')

for filename, rejected_line, selected_line in (('wowfax.h', 93, 95), ('wowinfo.h', 23, 25)):
    source = (repo / 'src/mvdm-host/inc' / filename).read_text(encoding='latin1').split('\n')
    start = next(i for i, line in enumerate(source) if line.startswith('#if defined(MIPS) || defined(_ALPHA_)'))
    end = next(i for i in range(start, len(source)) if source[i].startswith('#endif'))
    group = source[start:end + 1]
    if (f'#line {rejected_line}' not in group or f'#line {selected_line}' not in group
            or '#error Excluded unsupported original MIPS/Alpha UNALIGNED fallback' not in group):
        raise SystemExit(f'{filename}: shared fallback boundary differs')
    probe = scratch / 'shared-unaligned-guard.c'
    probe.write_text('\n'.join(group + ['int selected = 1;']) + '\n', encoding='ascii')
    for defines, reject in (([], False), (['MIPS=1'], True), (['_ALPHA_=1'], True)):
        result = subprocess.run(['cl.exe', '/nologo', '/EP', '/TC'] + [f'/D{x}' for x in defines] + [str(probe)],
                                capture_output=True, text=True, errors='replace')
        if bool(result.returncode) != reject or (reject and f'({rejected_line})' not in result.stdout + result.stderr):
            raise SystemExit(f'{filename}: shared fallback profile failed for {defines}')
print('PASS shared x86 UNALIGNED fallback and excluded MIPS/Alpha profiles')

# ntregapi.h is shared with the C WOW registry binding.
source = (repo / 'src/non-mvdm/public/sdk/inc/ntregapi.h').read_text(encoding='latin1').split('\n')
for original, diagnostic in ((29, 30), (520, 521)):
    start = source.index(f'#line {original}')
    expected = [f'#line {original}', '#ifdef __cplusplus', f'#line {diagnostic}',
                '#error Excluded unsupported C++ NT registry declaration context', '#endif']
    if [line.strip() for line in source[start:start + len(expected)]] != expected:
        raise SystemExit(f'ntregapi original C++ guard differs at {original}')
    probe = scratch / 'ntregapi-cxx-guard.c'
    probe.write_text('\n'.join(expected + ['int selected_c = 1;']) + '\n', encoding='ascii')
    for defines, reject in (([], False), (['__cplusplus=1'], True)):
        result = subprocess.run(['cl.exe', '/nologo', '/EP', '/TC'] + [f'/D{x}' for x in defines] + [str(probe)],
                                capture_output=True, text=True, errors='replace')
        if bool(result.returncode) != reject or (reject and f'({diagnostic})' not in result.stdout + result.stderr):
            raise SystemExit(f'ntregapi original C++ guard failed at {original}')
print('PASS ntregapi original C++ boundary and diagnostic lines')

# The shared executive header is currently consumed by x86 C product targets.
source = (repo / 'src/non-mvdm/public/sdk/inc/ntexapi.h').read_text(encoding='latin1').split('\n')
for original, diagnostic, macro in ((30, 31, '__cplusplus'), (1063, 1064, 'DEVL'),
                                    (1439, 1440, '_MAC'), (1459, 1460, '_WIN64'),
                                    (1480, 1481, '_WIN64'), (1573, 1574, 'REMOTE_BOOT')):
    start = source.index(f'#line {original}')
    end = next(i for i in range(start + 1, len(source)) if source[i].strip().startswith('#endif'))
    group = source[start:end + 1]
    if f'#line {diagnostic}' not in group or not any(line.startswith('#error Excluded unsupported') for line in group):
        raise SystemExit(f'ntexapi original rejected guard differs at {original}')
    probe = scratch / 'ntexapi-guard.c'
    probe.write_text('\n'.join(group + ['int selected_c = 1;']) + '\n', encoding='ascii')
    for defines, reject in (([], False), ([macro + '=1'], True)):
        result = subprocess.run(['cl.exe', '/nologo', '/EP', '/TC'] + [f'/D{x}' for x in defines] + [str(probe)], capture_output=True, text=True, errors='replace')
        if bool(result.returncode) != reject or (reject and f'({diagnostic})' not in result.stdout + result.stderr):
            raise SystemExit(f'ntexapi original rejected guard failed at {original}')
print('PASS ntexapi original executive profile guards and diagnostic lines')

# Three video paths previously lacked an original diagnostic line.
source = (repo / 'src/mvdm-host/softpc.new/base/video/video.c').read_text(encoding='latin1').split('\n')
for diagnostic, marker, selected_defines, rejected_defines in (
    (315, 'Unsupported excluded original configuration', ['C_VID=1'], []),
    (1887, 'DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.', ['CPU_40_STYLE=1'], []),
    (2045, 'Excluded unsupported non-monitor video profile', ['NTVDM=1', 'MONITOR=1', 'MVDM_STANDALONE_SAS_VIDEO=1'], []),
):
    target = next(i for i in range(len(source) - 1) if source[i].strip() == f'#line {diagnostic}' and source[i + 1].strip() == '#error ' + marker)
    stack = []
    frame = None
    for i, line in enumerate(source):
        directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
        if directive:
            if directive.group(1) == 'endif':
                start = stack.pop()
                if frame == start:
                    end = i
                    break
            else:
                stack.append(i)
        if i == target:
            frame = stack[-1]
    if frame is None:
        raise SystemExit(f'video diagnostic frame not found: {diagnostic}')
    probe = scratch / 'video-original-error-line.c'
    probe.write_text('\n'.join(source[frame:end + 1]) + '\n', encoding='ascii')
    for defines, reject in ((selected_defines, False), (rejected_defines, True)):
        result = subprocess.run(['cl.exe', '/nologo', '/EP', '/TC'] + [f'/D{x}' for x in defines] + [str(probe)], capture_output=True, text=True, errors='replace')
        if bool(result.returncode) != reject or (reject and (f'({diagnostic})' not in result.stdout + result.stderr or marker not in result.stdout + result.stderr)):
            raise SystemExit(f'video original line check failed {diagnostic}: {result.stderr}')
print('PASS video original excluded diagnostic lines and selected profiles')
