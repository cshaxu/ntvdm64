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
    if not entry.get('disposition', '').startswith(('subsumed-', 'withdrawn-'))]
for path, marker, macro in (
    ('src/mvdm-host/softpc.new/base/bios/tape_io.c', 'Excluded unsupported CPU30 descriptor path', 'CPU_30_STYLE'),
    ('src/non-mvdm/windows/core/ntuser/inc/ntcftxt.h', 'Excluded unsupported Unicode class-client configuration', 'UNICODE'),
    ('src/non-mvdm/windows/core/ntuser/inc/ntcftxt.h', 'Excluded unsupported Unicode class-client menu-name profile', 'UNICODE'),
    ('src/non-mvdm/windows/core/ntuser/inc/ntcftxt.h', 'Excluded unsupported Unicode class-client registration profile', 'UNICODE'),
    ('src/mvdm-host/softpc.new/host/inc/host_def.h', 'Excluded unsupported i386 host library loader', 'i386'),
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
    guard_index = matches[0] - 1
    while guard_index >= 0 and re.match(r'^\s*#\s*line\b', lines[guard_index]):
        guard_index -= 1
    if guard_index < 0:
        raise SystemExit('missing restored rejection guard: ' + path)
    entries.append({'path': path, 'line': guard_index + 1, 'macro': macro})
blocks = []
names = set()
for entry in entries:
    path = repo / entry['path']
    lines = path.read_text(encoding='latin1').split('\n')
    line = entry.get('current_line', entry['line'])
    guard = lines[line - 1]
    error_index = line
    while error_index < len(lines) and re.match(r'^\s*#\s*line\b', lines[error_index]):
        error_index += 1
    error = lines[error_index] if error_index < len(lines) else ''
    line_directives = lines[line:error_index]
    entry['_diagnostic_line'] = (int(re.match(r'^\s*#\s*line\s+(\d+)', line_directives[-1]).group(1))
                                 if line_directives else line + 1)
    guard_match = re.match(r'^\s*#\s*(if|ifdef|else)\b', guard)
    if not guard_match:
        raise SystemExit(f'missing original guard: {entry["path"]}:{entry["line"]}')
    if not re.match(r'^\s*#\s*error\s+Excluded unsupported\b', error):
        raise SystemExit(f'missing rejection: {entry["path"]}:{entry["line"] + 1}')
    if guard_match.group(1) == 'else':
        stack = []
        for preceding in lines[:line - 1]:
            directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b', preceding)
            if not directive:
                continue
            op = directive.group(1)
            if op in ('if', 'ifdef', 'ifndef'):
                stack.append(preceding)
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
        blocks.append([f'#line {line - 1} "{entry["path"]}"', stack[-1], guard, *line_directives, error, '#endif'])
    else:
        names.update(set(re.findall(r'\b[A-Za-z_][A-Za-z_0-9]*\b', guard)) - {'if', 'ifdef', 'defined'})
        blocks.append([f'#line {line} "{entry["path"]}"', guard, *line_directives, error, '#endif'])
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
    expected = f'{entry["path"]}({entry["_diagnostic_line"]})'
    if expected.lower() not in diagnostics.lower():
        raise SystemExit('missing excluded-branch diagnostic: ' + expected + '\n' + diagnostics)
print(f'PASS {len(entries)} retained guards: normal selection and rejected profiles ({os.environ.get("VSCMD_ARG_TGT_ARCH", "unknown")})')

host_def = (repo / 'src/mvdm-host/softpc.new/host/inc/host_def.h').read_text(encoding='latin1')
host_block = ('#ifdef i386\n#error Excluded unsupported i386 host library loader\n'
              '#else\n#line 223\n#define SafeLoadLibrary(name) LoadLibrary(name)\n'
              '#endif\n#line 225\n')
if host_def.count(host_block) != 1:
    raise SystemExit('host loader selector or original line mapping changed')
probe.write_text(host_block.replace('#define SafeLoadLibrary(name) LoadLibrary(name)',
                                    'int selected_line = __LINE__;') +
                 'int following_line = __LINE__;\n', encoding='ascii')
result = subprocess.run(base, capture_output=True, text=True, errors='replace')
if (result.returncode or not re.search(r'int\s+selected_line\s*=\s*223\s*;', result.stdout)
        or not re.search(r'int\s+following_line\s*=\s*225\s*;', result.stdout)):
    raise SystemExit('host loader original diagnostic lines failed: ' + result.stderr)
print('PASS original host loader selector and line mapping')

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
if len(selected) != 2:
    raise SystemExit('missing CPU40 virtual interrupt state in an original outer alternative')
i = selected[0]
if virtual_state[i-1].strip() != '#ifdef CPU_40_STYLE' or virtual_state[i+1].strip() != '#else':
    raise SystemExit('virtual interrupt state lost CPU40 boundary')
probe.write_text('\n'.join(virtual_state[i-1:i+4]) + '\n', encoding='ascii')
allowed = subprocess.run(base[:-1] + ['/DCPU_40_STYLE'] + base[-1:], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base, capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or 'Excluded unsupported non-CPU40 virtual interrupt state' not in rejected.stdout + rejected.stderr:
    raise SystemExit('virtual interrupt state CPU40 selection failed: ' + allowed.stderr + rejected.stderr)
print('PASS CPU40 virtual interrupt state; unsupported hardware-monitor alternative rejected')

if virtual_state[i-2].strip() != '#ifndef i386' or virtual_state[i+4].strip() != '#else' or virtual_state[i+10].strip() != '#endif /* i386 */':
    raise SystemExit('virtual interrupt state lost original i386 outer boundary')
probe.write_text('\n'.join(virtual_state[i-2:i+11]) + '\n', encoding='ascii')
allowed = subprocess.run(base[:-1] + ['/DCPU_40_STYLE'] + base[-1:], capture_output=True, text=True, errors='replace')
combined = subprocess.run(base[:-1] + ['/DCPU_40_STYLE', '/Di386'] + base[-1:], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base[:-1] + ['/Di386'] + base[-1:], capture_output=True, text=True, errors='replace')
if allowed.returncode or combined.returncode or 'fVirtualInt |= MIPS_BIT_MASK;' not in combined.stdout or not rejected.returncode or 'Excluded unsupported non-CPU40 virtual interrupt state' not in rejected.stdout + rejected.stderr:
    raise SystemExit('original i386 virtual interrupt boundary failed: ' + allowed.stderr + rejected.stderr)
print('PASS original i386 boundary with CPU40 behavior preserved in either arm')

mouse = (repo / 'src/mvdm-host/softpc.new/base/keymouse/mouse_io.c').read_text(encoding='latin1')
mouse = mouse[mouse.index('LOCAL void jump_to_user_subroutine IFN3'):]
start = mouse.index('#ifdef CPU_30_STYLE')
end = mouse.index('#endif /* !CPU_30_STYLE */', start) + len('#endif /* !CPU_30_STYLE */')
probe.write_text(mouse[start:end] + '\n', encoding='ascii')
allowed = subprocess.run(base[:-1] + ['/DCPU_40_STYLE'] + base[-1:], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base[:-1] + ['/DCPU_40_STYLE', '/DCPU_30_STYLE'] + base[-1:], capture_output=True, text=True, errors='replace')
if allowed.returncode or 'setIP(offset);' not in allowed.stdout or not rejected.returncode or 'Excluded unsupported original CPU30 mouse callback' not in rejected.stdout + rejected.stderr:
    raise SystemExit('original mouse CPU30 outer boundary failed: ' + allowed.stderr + rejected.stderr)
print('PASS original mouse CPU30 boundary and selected CPU40 callback')

sas = (repo / 'src/mvdm-host/softpc.new/base/inc/sas.h').read_text(encoding='latin1').splitlines()
hits = [i for i, line in enumerate(sas) if 'unsupported non-CPU40 SAS interface' in line]
if len(hits) != 1 or not sas[hits[0] - 1].strip().startswith('#else'):
    raise SystemExit('missing original SAS non-CPU40 alternative rejection')
probe.write_text('#ifdef CPU_40_STYLE\n#else\n' + sas[hits[0]] + '\n#endif\n', encoding='ascii')
allowed = subprocess.run(base[:-1] + ['/DCPU_40_STYLE'] + base[-1:], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base, capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or 'unsupported non-CPU40 SAS interface' not in rejected.stdout + rejected.stderr:
    raise SystemExit('SAS CPU40 original boundary failed: ' + allowed.stderr + rejected.stderr)
print('PASS original SAS CPU40 boundary')

for name, condition, defines in (
    ('debug.h', '#ifdef PROD', ['/DPROD']),
    ('yoda.h', '#ifdef PROD', ['/DPROD']),
    ('egaports.h', '#if defined(C_VID) || defined(A2CPU)', ['/DC_VID']),
    ('error.h', '#ifdef NTVDM', ['/DNTVDM']),
):
    header = (repo / 'src/mvdm-host/softpc.new/base/inc' / name).read_text(encoding='latin1').splitlines()
    errors = [line for line in header if 'unsupported alternate host profile' in line]
    if len(errors) != 1:
        raise SystemExit('missing unique rejected alternate in ' + name)
    probe.write_text(condition + '\n#else\n' + errors[0] + '\n#endif\n', encoding='ascii')
    allowed = subprocess.run(base[:-1] + defines + base[-1:], capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(base, capture_output=True, text=True, errors='replace')
    if allowed.returncode or not rejected.returncode or 'unsupported alternate host profile' not in rejected.stdout + rejected.stderr:
        raise SystemExit('original alternate profile boundary failed: ' + name)
print('PASS original host header alternate profile boundaries')

for name in ('cpu_vid.h', 'gmi.h'):
    header = (repo / 'src/mvdm-host/softpc.new/base/inc' / name).read_text(encoding='latin1').splitlines()
    errors = [line for line in header if 'unsupported generic video callback profile' in line]
    if len(errors) != 1:
        raise SystemExit('missing rejected generic callback carrier in ' + name)
    probe.write_text('#if defined(CPU_40_STYLE) && defined(C_VID)\n#else\n' + errors[0] + '\n#endif\n', encoding='ascii')
    for defines, success in ((['/DCPU_40_STYLE', '/DC_VID'], True), (['/DCPU_40_STYLE'], False), (['/DC_VID'], False), ([], False)):
        result = subprocess.run(base[:-1] + defines + base[-1:], capture_output=True, text=True, errors='replace')
        if (result.returncode == 0) != success:
            raise SystemExit('generic callback profile boundary failed: ' + name)
print('PASS local CPU40/C-VID callback carrier boundaries')

for name, count in (('dpmi32.c', 1), ('dpmi32p.h', 1), ('dpmiselr.c', 1), ('modesw.c', 2)):
    header = (repo / 'src/mvdm-host/dpmi32' / name).read_text(encoding='latin1').splitlines()
    positions = [i for i, line in enumerate(header) if 'unsupported i386 kernel VDM host path' in line]
    if len(positions) != count:
        raise SystemExit('missing original DPMI i386 guard in ' + name)
    for i in positions:
        guard = i - 1
        while guard >= 0 and re.match(r'^\s*#\s*line\b', header[guard]):
            guard -= 1
        if guard < 0 or not re.match(r'^\s*#\s*(?:ifdef\s+i386\b|if\s+defined\s*\(\s*i386\s*\))', header[guard]):
            raise SystemExit('original DPMI i386 selector missing: ' + name)
        probe.write_text('\n'.join(header[guard:i+1]) + '\n#endif\n', encoding='ascii')
        allowed = subprocess.run(base, capture_output=True, text=True, errors='replace')
        rejected = subprocess.run(base[:-1] + ['/Di386'] + base[-1:], capture_output=True, text=True, errors='replace')
        if allowed.returncode or not rejected.returncode or 'unsupported i386 kernel VDM host path' not in rejected.stdout + rejected.stderr:
            raise SystemExit('original DPMI i386 boundary failed: ' + name)
print('PASS original DPMI i386 kernel VDM boundaries')

emm = (repo / 'src/mvdm-host/softpc.new/base/inc/emm.h').read_text(encoding='latin1').splitlines()
markers = [line for line in emm if 'unsupported X86GFX EMS mapping profile' in line]
if len(markers) != 1:
    raise SystemExit('missing rejected X86GFX EMS mapping profile')
probe.write_text('#if defined(X86GFX) && defined(NTVDM)\n' + markers[0] + '\n#endif\n', encoding='ascii')
for defines, success in (([], True), (['/DNTVDM'], True), (['/DX86GFX'], True), (['/DX86GFX', '/DNTVDM'], False)):
    result = subprocess.run(base[:-1] + defines + base[-1:], capture_output=True, text=True, errors='replace')
    if (result.returncode == 0) != success:
        raise SystemExit('EMS X86GFX/NTVDM compound boundary failed')
print('PASS original EMS X86GFX/NTVDM compound boundary')

nt_bop = (repo / 'src/mvdm-host/softpc.new/host/src/nt_bop.c').read_text(encoding='latin1').splitlines()
marker = 'unsupported hardware-host FPU save path'
positions = [i for i, line in enumerate(nt_bop) if marker in line]
if (len(positions) != 1 or positions[0] < 2
        or nt_bop[positions[0]-2].strip() != '#ifdef i386'
        or nt_bop[positions[0]-1].strip() != '#line 930'):
    raise SystemExit('nt_bop hardware FPU rejection lost its original boundary')
i = positions[0]
probe.write_text('\n'.join(nt_bop[i-2:i+2]) + '\n', encoding='ascii')
allowed = subprocess.run(base, capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base[:-1] + ['/Di386'] + base[-1:], capture_output=True, text=True, errors='replace')
if (allowed.returncode or not rejected.returncode
        or marker not in rejected.stdout + rejected.stderr
        or not re.search(r'\(930\)', rejected.stdout + rejected.stderr)):
    raise SystemExit('nt_bop i386 rejection control failed: ' + allowed.stderr + rejected.stderr)
print('PASS CCPU excludes original hardware-host FPU save path')

for path, macro, expected_count, divergence in (
    ('src/mvdm-host/softpc.new/base/inc/winhelp.h', 'HOST_HELP', 1, 'MVDM-HOST-DIV-605'),
    ('src/mvdm-host/softpc.new/host/src/nt_vflop.c', 'VFLOPPY', 1, 'MVDM-HOST-DIV-605'),
):
    lines = (repo / path).read_text(encoding='latin1').splitlines()
    marker = divergence + ': unsupported ' + macro + ' profile'
    positions = [i for i, line in enumerate(lines) if marker in line]
    if len(positions) != expected_count:
        raise SystemExit('incorrect excluded profile guard count: ' + path)
    for i in positions:
        guard = i - 1
        while guard >= 0 and re.match(r'^\s*#\s*line\b', lines[guard]):
            guard -= 1
        if guard < 0 or not re.match(r'^\s*#\s*ifdef\s+' + re.escape(macro) + r'\s*(?:(?:/\*.*\*/|//.*)\s*)?$', lines[guard]):
            raise SystemExit('excluded profile lost original selector: ' + path)
        probe.write_text('\n'.join(lines[guard:i+1] + ['#endif']) + '\n', encoding='ascii')
        allowed = subprocess.run(base, capture_output=True, text=True, errors='replace')
        rejected = subprocess.run(base[:-1] + ['/D' + macro] + base[-1:],
                                  capture_output=True, text=True, errors='replace')
        if allowed.returncode or not rejected.returncode or marker not in rejected.stdout + rejected.stderr:
            raise SystemExit('excluded profile selection failed: ' + path)
print('PASS original excluded host and WOW profile guards')

for path, expression, marker, definitions in (
):
    lines = (repo / path).read_text(encoding='latin1').splitlines()
    positions = [i for i, line in enumerate(lines) if marker in line]
    if len(positions) != 1 or lines[positions[0]-1].strip() != '#if ' + expression:
        raise SystemExit('compound profile lost original selector: ' + path)
    i = positions[0]
    probe.write_text('\n'.join(lines[i-1:i+1] + ['#endif']) + '\n', encoding='ascii')
    allowed = subprocess.run(base, capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(base[:-1] + ['/D' + d for d in definitions] + base[-1:],
                              capture_output=True, text=True, errors='replace')
    if allowed.returncode or not rejected.returncode or marker not in rejected.stdout + rejected.stderr:
        raise SystemExit('compound profile selection failed: ' + path)
    if len(definitions) > 1:
        for definition in definitions:
            single = subprocess.run(base[:-1] + ['/D' + definition] + base[-1:],
                                    capture_output=True, text=True, errors='replace')
            if single.returncode:
                raise SystemExit('single compound operand rejected: ' + path)
print('PASS original compound profile guards and individual operands')

probe.write_text('#ifdef ALLOCA\n#error uppercase-only selector\n#endif\n', encoding='ascii')
result = subprocess.run(base[:-1] + ['/Dalloca=_alloca'] + base[-1:], capture_output=True, text=True, errors='replace')
if result.returncode:
    raise SystemExit('lowercase alloca incorrectly rejected: ' + result.stderr)
print('PASS lowercase alloca remains distinct from ALLOCA profile')

# The host PMODE32 path is distinct from the supported guest PMODE32 ABI.
header = repo / 'src/mvdm-host/wow32/wusercli.h'
probe.write_text('#include "' + header.as_posix() + '"\n', encoding='ascii')
allowed = subprocess.run(base, capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base[:-1] + ['/DPMODE32'] + base[-1:], capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or 'Excluded unsupported PMODE32 host thunk profile' not in rejected.stdout + rejected.stderr:
    raise SystemExit('host PMODE32 guard failed')
print('PASS selected WOW thunks and excluded host PMODE32 profile')

# Original WOW outer selectors reject the unselected fast-monitor profile.
fastwow = (repo / 'src/mvdm-host/wow32/wow32.c').read_text(encoding='latin1')
pattern = r'(?m)^#if FASTBOPPING\s*\n#error Excluded unsupported FASTBOPPING host profile\s*$'
if len(re.findall(pattern, fastwow)) != 3:
    raise SystemExit('missing one of the three original FASTBOPPING outer guards')
probe.write_text('#if FASTBOPPING\n#error Excluded unsupported FASTBOPPING host profile\n#endif\n', encoding='ascii')
allowed = subprocess.run(base, capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base[:-1] + ['/DFASTBOPPING=1'] + base[-1:], capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or 'Excluded unsupported FASTBOPPING host profile' not in rejected.stdout + rejected.stderr:
    raise SystemExit('FASTBOPPING guard failed')
print('PASS original FASTBOPPING outer guards and selected profile')

# The ROM generic-loader selector is the original OpenNT expression.
rom = (repo / 'src/mvdm-host/softpc.new/base/system/rom.c').read_text(encoding='latin1')
selector = '#if !defined(NTVDM) || ( defined(NTVDM) && !defined(X86GFX) )'
if sum(line.strip() == selector for line in rom.splitlines()) != 1:
    raise SystemExit('original ROM selector missing or changed')
probe.write_text(selector + '\n#else\n#error unsupported ROM profile\n#endif\n', encoding='ascii')
allowed = subprocess.run(base[:-1] + ['/DNTVDM'] + base[-1:], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base[:-1] + ['/DNTVDM', '/DX86GFX'] + base[-1:], capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or 'unsupported ROM profile' not in rejected.stdout + rejected.stderr:
    raise SystemExit('ROM selector control failed')
print('PASS original ROM selector and excluded X86GFX profile')

# Both restored original video selectors retain the standalone SAS body.
video = (repo / 'src/mvdm-host/softpc.new/base/video/video.c').read_text(encoding='latin1')
selector = '#if ( defined(NTVDM) && defined(MONITOR) ) || defined( GISP_SVGA )'
if sum(line.strip() == selector for line in video.splitlines()) != 2:
    raise SystemExit('two original video outer selectors not restored')
probe.write_text(selector + '\n#else\n#error unsupported non-monitor video profile\n#endif\n', encoding='ascii')
allowed = subprocess.run(base[:-1] + ['/DNTVDM', '/DMONITOR', '/DMVDM_STANDALONE_SAS_VIDEO'] + base[-1:], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base[:-1] + ['/DNTVDM', '/UMONITOR', '/UGISP_SVGA'] + base[-1:], capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or 'unsupported non-monitor video profile' not in rejected.stdout + rejected.stderr:
    raise SystemExit('original video selector control failed')
print('PASS original video selectors and non-monitor rejection')

# Original mouse NTVDM outer guards must reject unsupported host profiles.
mouse = (repo / 'src/mvdm-host/softpc.new/base/keymouse/mouse_io.c').read_text(encoding='latin1')
pattern = r'(?m)^#ifndef NTVDM\s*\n#error Excluded unsupported non-NTVDM mouse profile\s*$'
if len(re.findall(pattern, mouse)) != 7:
    raise SystemExit('seven original mouse NTVDM outer guards not restored')
probe.write_text('#ifndef NTVDM\n#error Excluded unsupported non-NTVDM mouse profile\n#endif\n', encoding='ascii')
allowed = subprocess.run(base[:-1] + ['/DNTVDM'] + base[-1:], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base[:-1] + ['/UNTVDM'] + base[-1:], capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or 'Excluded unsupported non-NTVDM mouse profile' not in rejected.stdout + rejected.stderr:
    raise SystemExit('mouse NTVDM original guard failed')
print('PASS seven original mouse NTVDM outer guards')

# These shared host headers are also referenced by original guest sources.
# This is a source-protection check; it does not build or validate the guest.
fax_guest = (repo / 'src/mvdm-guest/wow16/test/shell/wowexfax.c').read_text(encoding='latin1')
exec_guest = (repo / 'src/mvdm-guest/wow16/test/shell/wowexec.c').read_text(encoding='latin1')
guest_make = (repo / 'src/mvdm-guest/wow16/test/shell/makefile').read_text(encoding='latin1')
fax = (repo / 'src/mvdm-host/inc/wowfax.h').read_text(encoding='latin1')
info = (repo / 'src/mvdm-host/inc/wowinfo.h').read_text(encoding='latin1')
for text, pattern in ((fax_guest, r'(?m)^#define\s+_WOWFAX16_\b'),
                      (fax_guest, r'(?m)^#include\s+"wowfax\.h"'),
                      (exec_guest, r'(?m)^#include\s+"wowinfo\.h"'),
                      (guest_make, r'(?m)^COBJ\s*=.*-DDEBUG'),
                      (fax, r'(?m)^#if\s+defined\(_WOWFAX16_\)'),
                      (fax, r'(?m)^#ifdef\s+DEBUG'),
                      (fax, r'(?m)^#ifndef\s+_DEF_WOW32_'),
                      (info, r'(?m)^#ifndef\s+UNALIGNED')):
    if not re.search(pattern, text):
        raise SystemExit('shared guest-header protection check failed: ' + pattern)
for text, selector in ((fax, r'#if\s+defined\(_WOWFAX16_\)'),
                       (fax, r'#ifdef\s+DEBUG'),
                       (fax, r'#ifndef\s+_DEF_WOW32_'),
                       (info, r'#ifndef\s+UNALIGNED')):
    if re.search(r'(?m)^' + selector + r'[^\n]*\n\s*#error\b', text):
        raise SystemExit('shared guest-header body was replaced: ' + selector)
print('PASS original guest source references and shared header guards retained')

# Original kernel-only RTL time pragma condition must stay excluded.
time_source = (repo / 'src/non-mvdm/base/ntos/rtl/time.c').read_text(encoding='latin1')
selector = '#if defined(ALLOC_PRAGMA) && defined(NTOS_KERNEL_RUNTIME)'
marker = '#error Excluded unsupported kernel RTL time allocation profile'
if selector + '\n' + marker not in time_source:
    raise SystemExit('original RTL time selector not restored')
probe.write_text(selector + '\n' + marker + '\n#endif\n', encoding='ascii')
for defs in ([], ['/DALLOC_PRAGMA'], ['/DNTOS_KERNEL_RUNTIME']):
    if subprocess.run(base[:-1] + defs + base[-1:], capture_output=True).returncode:
        raise SystemExit('single RTL time selector operand incorrectly rejected')
rejected = subprocess.run(base[:-1] + ['/DALLOC_PRAGMA', '/DNTOS_KERNEL_RUNTIME'] + base[-1:], capture_output=True, text=True, errors='replace')
if not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
    raise SystemExit('kernel RTL time selector not rejected')
print('PASS original RTL time selector and both operands')

# DPMI kernel-VDM state is not available in the selected CCPU worker.
dpmi_data = (repo / 'src/mvdm-host/dpmi32/dpmidata.h').read_text(encoding='latin1')
marker = '#error Excluded unsupported i386 kernel VDM DPMI state'
if '#ifdef i386\n' + marker not in dpmi_data:
    raise SystemExit('DPMI original i386 exclusion missing')
probe.write_text('#ifdef i386\n' + marker + '\n#endif\n', encoding='ascii')
allowed = subprocess.run(base[:-1] + ['/Ui386'] + base[-1:], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base[:-1] + ['/Di386'] + base[-1:], capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
    raise SystemExit('DPMI i386 state guard failed')
print('PASS DPMI i386 kernel state exclusion')

# Two original i386-only fast IRET arms cannot run in the CCPU worker.
dpmi = (repo / 'src/mvdm-host/dpmi32/dpmiint.c').read_text(encoding='latin1')
marker = '#error Excluded unsupported i386 kernel VDM interrupt return'
if len(re.findall(r'(?m)^#ifdef i386\s*\n' + re.escape(marker) + r'\s*$', dpmi)) != 2:
    raise SystemExit('original i386 IRET guards missing')
probe.write_text('#ifdef i386\n' + marker + '\n#endif\n', encoding='ascii')
allowed = subprocess.run(base[:-1] + ['/Ui386'] + base[-1:], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base[:-1] + ['/Di386'] + base[-1:], capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
    raise SystemExit('i386 IRET rejection failed')
print('PASS original i386 IRET guards')

# Retired VDM_TIB pointer publication is unavailable in the CCPU worker.
dpmi = (repo / 'src/mvdm-host/dpmi32/dpmiint.c').read_text(encoding='latin1')
marker = '#error Excluded unsupported i386 kernel VDM stack publication'
if dpmi.count(marker) != 1 or '#ifdef i386\n' + marker not in dpmi:
    raise SystemExit('kernel VDM stack publication guard missing')
probe.write_text('#ifdef i386\n' + marker + '\n#endif\n', encoding='ascii')
allowed = subprocess.run(base[:-1] + ['/Ui386', '/DCPU_40_STYLE'] + base[-1:], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base[:-1] + ['/Di386'] + base[-1:], capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
    raise SystemExit('kernel VDM stack publication rejection failed')
print('PASS kernel VDM stack publication exclusion')

# Original SOFTPC i386 branches refer to the unavailable kernel VDM.
softpc = (repo / 'src/mvdm-host/inc/softpc.h').read_text(encoding='latin1')
for marker in ('Excluded unsupported i386 kernel VDM declarations',
               'Excluded unsupported i386 kernel VDM register interface'):
    if not re.search(r'(?m)^#ifdef i386\s*\n(?:#line\s+\d+\s*\n)?#error ' + re.escape(marker), softpc):
        raise SystemExit('SOFTPC i386 exclusion missing: ' + marker)
    probe.write_text('#ifdef i386\n#error ' + marker + '\n#endif\n', encoding='ascii')
    allowed = subprocess.run(base[:-1] + ['/Ui386'] + base[-1:], capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(base[:-1] + ['/Di386'] + base[-1:], capture_output=True, text=True, errors='replace')
    if allowed.returncode or not rejected.returncode or marker not in rejected.stdout + rejected.stderr:
        raise SystemExit('SOFTPC i386 exclusion control failed: ' + marker)
if 'extern USHORT __cdecl getAX(VOID);' not in softpc:
    raise SystemExit('selected CPU40 cdecl declaration was removed')
print('PASS SOFTPC i386 kernel exclusions and retained CPU40 cdecl')

# The retained EDIT subset is deliberately included in two cohorts. A single
# original whole-file guard would suppress the second, state-bearing inclusion.
usercli = (repo / 'src/non-mvdm/windows/core/ntuser/client/usercli.h').read_bytes()
for name, contents in (
    ('usercli-current.h', usercli),
    ('usercli-single-guard.h', b'#ifndef _USERCLI_\n#define _USERCLI_\n' + usercli + b'\n#endif\n'),
):
    (scratch / name).write_bytes(contents)
    probe.write_text('#define WOW_EDIT_LOOKASIDE_DECLARATIONS_ONLY\n'
                     '#include "' + name + '"\n'
                     '#undef WOW_EDIT_LOOKASIDE_DECLARATIONS_ONLY\n'
                     '#include "' + name + '"\n', encoding='ascii')
    result = subprocess.run(base, capture_output=True, text=True, errors='replace')
    has_lookaside = 'typedef struct _LOOKASIDE' in result.stdout
    has_edit = 'typedef struct tagED {' in result.stdout
    if result.returncode or not has_lookaside or has_edit != (name == 'usercli-current.h'):
        raise SystemExit('USER client two-cohort guard control failed: ' + result.stderr)
print('PASS USER client two-cohort inclusion and single-guard negative control')

# The selected NT declaration subset now keeps its original include guard.
ntexapi = (repo / 'src/non-mvdm/public/sdk/inc/ntexapi.h').read_text(encoding='latin1')
if not ntexapi.startswith('#ifndef _NTEXAPI_\n#define _NTEXAPI_\n'):
    raise SystemExit('original NTEXAPI include guard missing')
(scratch / 'ntexapi-guard.h').write_text(ntexapi, encoding='latin1')
for predefine in (False, True):
    probe.write_text(('#define _NTEXAPI_\n' if predefine else '') +
                     '#include "ntexapi-guard.h"\n#include "ntexapi-guard.h"\n',
                     encoding='ascii')
    result = subprocess.run(base, capture_output=True, text=True, errors='replace')
    count = result.stdout.count('NtQueryPerformanceCounter')
    if result.returncode or count != (0 if predefine else 1):
        raise SystemExit('NTEXAPI original guard control failed: ' + result.stderr)
print('PASS original NTEXAPI include guard and predefined-guard control')

ntregapi = (repo / 'src/non-mvdm/public/sdk/inc/ntregapi.h').read_text(encoding='latin1')
if '#ifndef _NTREGAPI_\n#define _NTREGAPI_\n' not in ntregapi:
    raise SystemExit('original NTREGAPI include guard missing')
(scratch / 'ntregapi-guard.h').write_text(ntregapi, encoding='latin1')
for predefine in (False, True):
    probe.write_text(('#define _NTREGAPI_\n' if predefine else '') +
                     '#include "ntregapi-guard.h"\n#include "ntregapi-guard.h"\n',
                     encoding='ascii')
    result = subprocess.run(base, capture_output=True, text=True, errors='replace')
    count = result.stdout.count('typedef struct _KEY_VALUE_PARTIAL_INFORMATION')
    if result.returncode or count != (0 if predefine else 1):
        raise SystemExit('NTREGAPI original guard control failed: ' + result.stderr)
print('PASS original NTREGAPI include guard and predefined-guard control')

nturtl = (repo / 'src/non-mvdm/public/sdk/inc/nturtl.h').read_text(encoding='latin1')
if '#ifndef _NTURTL_\n#define _NTURTL_\n' not in nturtl:
    raise SystemExit('original NTURTL include guard missing')
(scratch / 'nturtl-guard.h').write_text(nturtl, encoding='latin1')
for predefine in (False, True):
    probe.write_text(('#define _NTURTL_\n' if predefine else '') +
                     '#include "nturtl-guard.h"\n#include "nturtl-guard.h"\n',
                     encoding='ascii')
    result = subprocess.run(base[:-1] + ['/I' + str(repo / 'src/compat/include'),
                             '/I' + str(repo / 'src/non-mvdm/public/sdk/inc')] + base[-1:],
                            capture_output=True, text=True, errors='replace')
    count = result.stdout.count('RtlIsDosDeviceName_U')
    if result.returncode or count != (0 if predefine else 1):
        raise SystemExit('NTURTL original guard control failed: ' + result.stderr)
print('PASS original NTURTL include guard and predefined-guard control')

ntrtl = (repo / 'src/non-mvdm/public/sdk/inc/ntrtl.h').read_text(encoding='latin1')
if '#ifndef _NTRTL_\n#define _NTRTL_\n' not in ntrtl:
    raise SystemExit('original NTRTL include guard missing')
(scratch / 'ntrtl-guard.h').write_text(ntrtl, encoding='latin1')
for predefine in (False, True):
    probe.write_text(('#define _NTRTL_\n' if predefine else '') +
                     '#include "ntrtl-guard.h"\n#include "ntrtl-guard.h"\n',
                     encoding='ascii')
    result = subprocess.run(base[:-1] + ['/I' + str(repo / 'src/compat/include'),
                             '/I' + str(repo / 'src/non-mvdm/public/sdk/inc')] + base[-1:],
                            capture_output=True, text=True, errors='replace')
    count = result.stdout.count('RtlOemToUnicodeN(')
    if result.returncode or count != (0 if predefine else 1):
        raise SystemExit('NTRTL original guard control failed: ' + result.stderr)
print('PASS original NTRTL include guard and predefined-guard control')

for source, guard, marker in (
    ('src/non-mvdm/windows/core/ntuser/client/globals.h', '_GLOBALS_', 'LOCALALLOC'),
    ('src/non-mvdm/windows/core/ntuser/rtl/userrtl.h', '_USERRTL_', 'REBASEALWAYS'),
):
    header = (repo / source).read_text(encoding='latin1')
    if '#ifndef ' + guard + '\n#define ' + guard + '\n' not in header:
        raise SystemExit('original USER header guard missing: ' + source)
    (scratch / 'user-guard.h').write_text(header, encoding='latin1')
    for predefine in (False, True):
        probe.write_text(('#define ' + guard + '\n' if predefine else '') +
                         '#include "user-guard.h"\n#include "user-guard.h"\n'
                         '#ifdef ' + marker + '\nUSER_GUARD_SELECTED\n#endif\n', encoding='ascii')
        result = subprocess.run(base, capture_output=True, text=True, errors='replace')
        if result.returncode or ('USER_GUARD_SELECTED' in result.stdout) == predefine:
            raise SystemExit('USER original guard control failed: ' + source + result.stderr)
print('PASS original GLOBALS and USERRTL guards and predefined-guard controls')

userk = (repo / 'src/non-mvdm/windows/core/ntuser/kernel/userk.h').read_text(encoding='latin1')
if '#ifndef _USERK_\n#define _USERK_\n' not in userk:
    raise SystemExit('original USERK include guard missing')
(scratch / 'userk-guard.h').write_text(userk, encoding='latin1')
for prefix, expected_flags, expected_full in (
    ('', True, True),
    ('#define WOW_USER_OBJECT_FLAGS_ONLY\n', True, False),
    ('#define _USERK_\n', False, False),
):
    probe.write_text(prefix + '#include "userk-guard.h"\n#include "userk-guard.h"\n'
                     '#ifdef OCF_THREADOWNED\nUSERK_FLAGS_SELECTED\n#endif\n'
                     '#ifdef PSEUDO_EVENT_ON\nUSERK_FULL_SELECTED\n#endif\n', encoding='ascii')
    result = subprocess.run(base, capture_output=True, text=True, errors='replace')
    if (result.returncode or ('USERK_FLAGS_SELECTED' in result.stdout) != expected_flags or
            ('USERK_FULL_SELECTED' in result.stdout) != expected_full):
        raise SystemExit('USERK original guard control failed: ' + result.stderr)
print('PASS original USERK guard with flags-only, full and predefined controls')

# The retained USER subsets can be requested by separate includes in one TU.
user = (repo / 'src/non-mvdm/windows/core/ntuser/inc/user.h').read_bytes()
for name, contents in (
    ('user-current.h', user),
    ('user-single-guard.h', b'#ifndef _USER_\n#define _USER_\n' + user + b'\n#endif\n'),
):
    (scratch / name).write_bytes(contents)
    probe.write_text('#include "' + name + '"\n'
                     '#define WOW_ORIGINAL_DIALOG_GETSET_LAYOUT\n'
                     '#define WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT\n'
                     '#include "' + name + '"\n', encoding='ascii')
    result = subprocess.run(base, capture_output=True, text=True, errors='replace')
    expected = name == 'user-current.h'
    if (result.returncode or ('typedef struct _CALLPROCDATA' in result.stdout) != expected or
            ('typedef struct tagBUTN {' in result.stdout) != expected):
        raise SystemExit('USER multi-cohort guard control failed: ' + result.stderr)
print('PASS USER multi-cohort inclusion and single-guard negative control')

clglobal = (repo / 'src/non-mvdm/windows/core/ntuser/client/clglobal.c').read_text(encoding='latin1')
for condition, marker, definition in (
    ('#if DBG', 'Excluded unsupported USER debug-global profile', '/DDBG=1'),
    ('#ifdef WX86', 'Excluded unsupported USER WX86-global profile', '/DWX86'),
):
    pattern = (re.escape(condition) + r'\n(?:\s*#\s*line\s+\d+\s*\n)*'
               + re.escape('#error ' + marker) + r'\n#endif')
    matches = list(re.finditer(pattern, clglobal))
    if len(matches) != 1:
        raise SystemExit('USER global original exclusion missing: ' + condition)
    boundary = matches[0].group(0)
    probe.write_text(boundary + '\n', encoding='ascii')
    allowed = subprocess.run(base, capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(base[:-1] + [definition] + base[-1:],
                              capture_output=True, text=True, errors='replace')
    if allowed.returncode or not rejected.returncode or marker not in rejected.stdout + rejected.stderr:
        raise SystemExit('USER global exclusion control failed: ' + condition)
print('PASS original USER debug-global and WX86-global exclusion guards')

clmenu = (repo / 'src/non-mvdm/windows/core/ntuser/client/clmenu.c').read_text(encoding='latin1')
for macro, marker in (
    ('MEMPHIS_MENUS', 'Excluded unsupported Memphis menu conversion profile'),
    ('MEMPHIS_MENU_WATERMARKS', 'Excluded unsupported Memphis menu watermark profile'),
):
    pattern = (re.escape('#ifdef ' + macro) + r'\n(?:\s*#\s*line\s+\d+\s*\n)*'
               + re.escape('#error ' + marker) + r'\n#endif')
    matches = list(re.finditer(pattern, clmenu))
    if len(matches) != 1:
        raise SystemExit('USER menu original exclusion missing: ' + macro)
    boundary = matches[0].group(0)
    probe.write_text(boundary + '\n', encoding='ascii')
    allowed = subprocess.run(base, capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(base[:-1] + ['/D' + macro] + base[-1:],
                              capture_output=True, text=True, errors='replace')
    if allowed.returncode or not rejected.returncode or marker not in rejected.stdout + rejected.stderr:
        raise SystemExit('USER menu exclusion control failed: ' + macro)
print('PASS original Memphis menu conversion and watermark exclusions')

handtabl = (repo / 'src/non-mvdm/windows/core/ntuser/kernel/handtabl.c').read_text(encoding='latin1')
boundary = ('#line 544\n#if 0\n#line 545\n'
            '#error Excluded original disabled USER handle-table check\n'
            '#endif\n#line 564')
if handtabl.count(boundary) != 1:
    raise SystemExit('original disabled handle-table check boundary missing')
probe.write_text(boundary + '\n', encoding='ascii')
allowed = subprocess.run(base, capture_output=True, text=True, errors='replace')
probe.write_text(boundary.replace('#if 0', '#if 1', 1) + '\n', encoding='ascii')
rejected = subprocess.run(base, capture_output=True, text=True, errors='replace')
if (allowed.returncode or not rejected.returncode or
        'Excluded original disabled USER handle-table check' not in rejected.stdout + rejected.stderr or
        '(545)' not in rejected.stdout + rejected.stderr):
    raise SystemExit('original disabled handle-table check control failed')
print('PASS original literal-disabled USER handle-table guard')
