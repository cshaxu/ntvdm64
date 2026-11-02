"""Check real retained guards reject excluded profiles with the MSVC preprocessor."""

import argparse
import json
import os
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
entries = [entry for entry in json.loads(
    (repo / 'artifacts/mvdm-audit/s13-rejected-legacy-branches.json').read_text())
    if not entry.get('disposition', '').startswith('subsumed-')]
for path, marker, macro in (
    ('src/mvdm-host/softpc.new/base/bios/tape_io.c', 'Excluded unsupported CPU30 descriptor path', 'CPU_30_STYLE'),
    ('src/non-mvdm/windows/core/ntuser/inc/ntcftxt.h', 'Excluded unsupported Unicode class-client configuration', 'UNICODE'),
    ('src/mvdm-host/softpc.new/base/system/qevnt.c', 'Excluded unsupported quick-event hardware monitor path', 'MONITOR'),
    ('src/mvdm-host/softpc.new/host/src/nt_eoi.c', 'Excluded unsupported delayed-interrupt monitor declarations', 'MONITOR'),
    ('src/mvdm-host/softpc.new/host/src/nt_eoi.c', 'Excluded unsupported delayed-interrupt monitor dispatch', 'MONITOR'),
    ('src/mvdm-host/softpc.new/host/src/nt_eoi.c', 'Excluded unsupported monitor interrupt restart', 'MONITOR'),
    ('src/mvdm-host/softpc.new/host/src/nt_eoi.c', 'Excluded unsupported monitor IRET table', 'MONITOR'),
    ('src/mvdm-host/softpc.new/host/src/nt_timer.c', 'Excluded unsupported monitor timer switch state', 'MONITOR'),
    ('src/mvdm-host/softpc.new/host/src/nt_timer.c', 'Excluded unsupported monitor timer lock setup', 'MONITOR'),
    ('src/mvdm-host/softpc.new/host/src/nt_timer.c', 'Excluded unsupported monitor timer lock release', 'MONITOR'),
    ('src/mvdm-host/softpc.new/host/src/nt_timer.c', 'Excluded unsupported monitor heartbeat wait', 'MONITOR'),
    ('src/mvdm-host/softpc.new/base/system/ica.c', 'Excluded unsupported monitor PIC interrupt ownership', 'MONITOR'),
    ('src/mvdm-host/softpc.new/base/system/ica.c', 'Excluded unsupported monitor PIC IRET address', 'MONITOR'),
    ('src/mvdm-host/softpc.new/base/system/ica.c', 'Excluded unsupported monitor PIC IRET hook', 'MONITOR'),
    ('src/mvdm-host/softpc.new/host/src/nt_bop.c', 'Excluded unsupported monitor IRET BOP', 'MONITOR'),
    ('src/mvdm-host/softpc.new/host/src/nt_bop.c', 'Excluded unsupported monitor keyboard setup carry', 'MONITOR'),
    ('src/mvdm-host/softpc.new/host/src/nt_reset.c', 'Excluded unsupported monitor IVT declarations', 'MONITOR'),
    ('src/mvdm-host/softpc.new/host/src/nt_reset.c', 'Excluded unsupported monitor ICA initialization wait', 'MONITOR'),
    ('src/mvdm-host/softpc.new/host/src/config.c', 'Excluded unsupported monitor floating-point status', 'MONITOR'),
):
    lines = (repo / path).read_text(encoding='latin1').split('\n')
    matches = [i for i, line in enumerate(lines) if marker in line]
    if len(matches) != 1 or matches[0] == 0:
        raise SystemExit('missing unique restored rejection: ' + path)
    entries.append({'path': path, 'line': matches[0], 'macro': macro})
blocks = []
names = set()
for entry in entries:
    path = repo / entry['path']
    lines = path.read_text(encoding='latin1').split('\n')
    guard, error = lines[entry['line'] - 1:entry['line'] + 1]
    guard_match = re.match(r'^\s*#\s*(if|ifdef|else)\b', guard)
    if not guard_match:
        raise SystemExit(f'missing original guard: {entry["path"]}:{entry["line"]}')
    if not re.match(r'^\s*#\s*error\s+Excluded unsupported\b', error):
        raise SystemExit(f'missing rejection: {entry["path"]}:{entry["line"] + 1}')
    if guard_match.group(1) == 'else':
        stack = []
        for line in lines[:entry['line'] - 1]:
            directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b', line)
            if not directive:
                continue
            op = directive.group(1)
            if op in ('if', 'ifdef', 'ifndef'):
                stack.append(line)
            elif op == 'endif' and stack:
                stack.pop()
            elif op == 'elif' and stack:
                stack[-1] = ''
        if entry['macro'] == '1':
            expected_parent = r'^\s*#\s*if\s+1\b'
        elif entry['macro'] == 'LDBIOS':
            expected_parent = r'^\s*#\s*if\s+!defined\(LDBIOS\)\s*&&\s*!defined\(CPU_30_STYLE\)'
        else:
            expected_parent = r'^\s*#\s*ifndef\s+' + re.escape(entry['macro']) + r'\b'
        if not stack or not re.match(expected_parent, stack[-1]):
            raise SystemExit('unsupported alternative structure: ' + entry['path'])
        if entry['macro'] != '1':
            names.add(entry['macro'])
        blocks.append([f'#line {entry["line"] - 1} "{entry["path"]}"', stack[-1], guard, error, '#endif'])
    else:
        names.update(set(re.findall(r'\b[A-Za-z_][A-Za-z_0-9]*\b', guard)) - {'if', 'ifdef', 'defined'})
        blocks.append([f'#line {entry["line"]} "{entry["path"]}"', guard, error, '#endif'])
probe = scratch / 'excluded-guard-probe.c'
probe.write_text('\n'.join(line for block in blocks for line in block) + '\n', encoding='ascii')
base = ['cl.exe', '/nologo', '/EP', '/TC', str(probe)]
control = subprocess.run(base, capture_output=True, text=True, errors='replace')
if control.returncode:
    raise SystemExit('ordinary preprocessing failed: ' + control.stderr)
for entry, block in zip(entries, blocks):
    selected = list(block)
    if entry['macro'] == '0':
        selected[1] = '#if 1'  # Exercise a permanently-false original arm in isolation.
    elif entry['macro'] == '1':
        selected[1] = '#if 0'  # Exercise the inactive else arm in isolation.
    probe.write_text('\n'.join(selected) + '\n', encoding='ascii')
    negative = subprocess.run(base[:-1] + ['/D' + name for name in sorted(names)] + base[-1:],
                              capture_output=True, text=True, errors='replace')
    diagnostics = negative.stdout + negative.stderr
    if negative.returncode == 0:
        raise SystemExit('unsupported condition unexpectedly succeeded: ' + entry['path'])
    expected = f'{entry["path"]}({entry["line"] + 1})'
    if expected.lower() not in diagnostics.lower():
        raise SystemExit('missing excluded-branch diagnostic: ' + expected + '\n' + diagnostics)
print(f'PASS {len(entries)} retained guards: normal selection and rejected profiles ({os.environ.get("VSCMD_ARG_TGT_ARCH", "unknown")})')

dpmi = (repo / 'src/mvdm-host/dpmi32/dpmiint.c').read_text(encoding='latin1').split('\n')
marker = 'Excluded unsupported VdmSetInt21Handler kernel VDM path'
positions = [i for i, line in enumerate(dpmi) if marker in line]
if len(positions) != 1:
    raise SystemExit('missing unique DPMI rejection')
i = positions[0]
if not (re.match(r'^\s*#\s*ifdef\s+i386\b', dpmi[i-2]) and
        re.match(r'^\s*#\s*ifndef\s+CPU_40_STYLE\b', dpmi[i-1])):
    raise SystemExit('DPMI rejection lost its i386/CPU40 boundary')
probe.write_text('\n'.join(dpmi[i-2:i+1] + ['#endif', '#endif']) + '\n', encoding='ascii')
allowed = subprocess.run(base[:-1] + ['/Di386', '/DCPU_40_STYLE'] + base[-1:], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base[:-1] + ['/Di386'] + base[-1:], capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or marker not in rejected.stdout + rejected.stderr:
    raise SystemExit('DPMI i386/CPU40 control failed: ' + allowed.stderr + rejected.stderr)
print('PASS DPMI i386 with CPU40 accepted; legacy kernel path rejected')

send_header = (repo / 'src/non-mvdm/windows/core/ntuser/inc/ntsend.h').as_posix()
probe.write_text(f'#include "{send_header}"\nCOPYLPTSTRID(p,s)\nCLEANUPLPTSTR(x)\n', encoding='ascii')
for defines, expected, forbidden in (
    ([], 'RtlCaptureAnsiString', 'RtlInitUnicodeString'),
    (['/DUNICODE'], 'RtlInitUnicodeString', 'RtlCaptureAnsiString'),
):
    result = subprocess.run(base[:-1] + defines + base[-1:], capture_output=True,
                            text=True, errors='replace')
    if result.returncode or expected not in result.stdout or forbidden in result.stdout:
        raise SystemExit('ntsend text-width selection failed: ' + result.stderr)
print('PASS original ntsend ANSI and Unicode capture selections')

cpu_header = (repo / 'src/mvdm-host/softpc.new/base/inc/cpu.h').as_posix()
probe.write_text(f'#include "{cpu_header}"\n', encoding='ascii')
fixture = subprocess.run(base[:-1] + ['/DNTVDM', '/DCCPU', '/DCPU_40_STYLE'] + base[-1:],
                         capture_output=True, text=True, errors='replace')
if fixture.returncode:
    raise SystemExit('CCPU fixture header inclusion failed: ' + fixture.stderr)
print('PASS CCPU fixture includes cpu.h without selecting the excluded monitor profile')

userk_header = (repo / 'src/non-mvdm/windows/core/ntuser/kernel/userk.h').as_posix()
probe.write_text(f'#include "{userk_header}"\n', encoding='ascii')
for defines, present in (([], True), (['/DMSDWP'], False)):
    result = subprocess.run(base[:-1] + defines + base[-1:], capture_output=True,
                            text=True, errors='replace')
    if result.returncode or any((name in result.stdout) != present for name in
                                ('EnterWowCritSect', 'ExitWowCritSect')):
        raise SystemExit('userk MSDWP selection failed: ' + result.stderr)
print('PASS original userk MSDWP function selection')

ntrtl_header = (repo / 'src/non-mvdm/public/sdk/inc/ntrtl.h').as_posix()
probe.write_text(f'#include "{ntrtl_header}"\n', encoding='ascii')
includes = ['/I' + str(repo / 'src/compat/include'),
            '/I' + str(repo / 'src/non-mvdm/public/sdk/inc')]
for defines, present in (([], True), (['/DMIDL_PASS'], False)):
    result = subprocess.run(base[:-1] + includes + defines + base[-1:],
                            capture_output=True, text=True, errors='replace')
    if result.returncode or ('RtlConvertLongToLuid' in result.stdout) != present:
        raise SystemExit('ntrtl MIDL_PASS selection failed: ' + result.stderr)
print('PASS original ntrtl MIDL_PASS inline selection')

virtual_state = (repo / 'src/mvdm-host/softpc.new/host/src/nt_msscs.c').read_text(encoding='latin1').split('\n')
selected = [i for i, line in enumerate(virtual_state) if 'fVirtualInt |= MIPS_BIT_MASK;' in line]
if len(selected) != 1:
    raise SystemExit('missing unique virtual interrupt state selection')
i = selected[0]
if virtual_state[i-1].strip() != '#ifdef CPU_40_STYLE' or virtual_state[i+1].strip() != '#else':
    raise SystemExit('virtual interrupt state lost CPU40 boundary')
probe.write_text('\n'.join(virtual_state[i-1:i+4]) + '\n', encoding='ascii')
allowed = subprocess.run(base[:-1] + ['/DCPU_40_STYLE'] + base[-1:], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base, capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or 'Excluded unsupported non-CPU40 virtual interrupt state' not in rejected.stdout + rejected.stderr:
    raise SystemExit('virtual interrupt state CPU40 selection failed: ' + allowed.stderr + rejected.stderr)
print('PASS CPU40 virtual interrupt state; unsupported hardware-monitor alternative rejected')
