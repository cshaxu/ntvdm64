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

if virtual_state[i-2].strip() != '#ifndef i386' or virtual_state[i+4].strip() != '#else' or virtual_state[i+6].strip() != '#endif /* i386 */':
    raise SystemExit('virtual interrupt state lost original i386 outer boundary')
probe.write_text('\n'.join(virtual_state[i-2:i+7]) + '\n', encoding='ascii')
allowed = subprocess.run(base[:-1] + ['/DCPU_40_STYLE'] + base[-1:], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base[:-1] + ['/DCPU_40_STYLE', '/Di386'] + base[-1:], capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or 'Excluded unsupported original i386 host monitor virtual interrupt state' not in rejected.stdout + rejected.stderr:
    raise SystemExit('original i386 virtual interrupt boundary failed: ' + allowed.stderr + rejected.stderr)
print('PASS original i386 virtual interrupt boundary')

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

nt_bop = (repo / 'src/mvdm-host/softpc.new/host/src/nt_bop.c').read_text(encoding='latin1').splitlines()
marker = 'unsupported hardware-host FPU save path'
positions = [i for i, line in enumerate(nt_bop) if marker in line]
if len(positions) != 1 or nt_bop[positions[0]-1].strip() != '#ifdef i386':
    raise SystemExit('nt_bop hardware FPU rejection lost its original boundary')
i = positions[0]
probe.write_text('\n'.join(nt_bop[i-1:i+2]) + '\n', encoding='ascii')
allowed = subprocess.run(base, capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base[:-1] + ['/Di386'] + base[-1:], capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or marker not in rejected.stdout + rejected.stderr:
    raise SystemExit('nt_bop i386 rejection control failed: ' + allowed.stderr + rejected.stderr)
print('PASS CCPU excludes original hardware-host FPU save path')

for path, macro, expected_count, divergence in (
    ('src/mvdm-host/softpc.new/base/bios/bootstra.c', 'SECURE', 4, 'MVDM-HOST-DIV-602'),
    ('src/mvdm-host/softpc.new/base/inc/cpu2.h', 'EGATEST', 1, 'MVDM-HOST-DIV-602'),
    ('src/mvdm-host/wow32/wkglobal.c', 'API16', 8, 'MVDM-HOST-DIV-603'),
    ('src/mvdm-host/wow32/wklocal.c', 'API16', 2, 'MVDM-HOST-DIV-603'),
    ('src/mvdm-host/softpc.new/host/src/stf_conf.c', 'STUBBED', 4, 'MVDM-HOST-DIV-604'),
    ('src/mvdm-host/sim32/sim32.h', 'ALLOCA', 1, 'MVDM-HOST-DIV-611'),
    ('src/non-mvdm/public/internal/base/inc/isnkrnl.h', '_PNP_POWER', 1, 'OPENNT-HOST-151'),
    ('src/mvdm-host/bde/bdesym.c', 'NEED_ABSOLUTES', 1, 'MVDM-HOST-DIV-610'),
    ('src/mvdm-host/bde/bdesym.c', 'NEED_INDICES', 4, 'MVDM-HOST-DIV-610'),
    ('src/mvdm-host/fax/wowfaxui/wowfaxui.h', 'NOLOG', 1, 'MVDM-HOST-DIV-610'),
    ('src/mvdm-host/sim32/sim32.h', 'SIM_32', 1, 'MVDM-HOST-DIV-610'),
    ('src/mvdm-host/softpc.new/base/inc/jformate.h', 'JLD_PRIVATE', 1, 'MVDM-HOST-DIV-610'),
    ('src/mvdm-host/softpc.new/base/inc/lm_attr.h', 'VMS', 1, 'MVDM-HOST-DIV-610'),
    ('src/mvdm-host/softpc.new/base/inc/ms_windo.h', 'MSWDVR', 1, 'MVDM-HOST-DIV-610'),
    ('src/mvdm-host/softpc.new/base/inc/spc_icon.h', 'SOFTWINDOWS', 1, 'MVDM-HOST-DIV-610'),
    ('src/mvdm-host/vdmexts/sym.c', 'NEED_ABSOLUTES', 1, 'MVDM-HOST-DIV-610'),
    ('src/mvdm-host/vdmexts/sym.c', 'NEED_INDICES', 4, 'MVDM-HOST-DIV-610'),
    ('src/mvdm-host/softpc.new/dat2obj/dat2obj.c', '_MIPS_', 1, 'MVDM-HOST-DIV-609'),
    ('src/mvdm-host/softpc.new/dat2obj/dat2obj.c', '_PPC_', 1, 'MVDM-HOST-DIV-609'),
    ('src/mvdm-host/softpc.new/dat2obj/dat2obj.c', 'ALPHA', 1, 'MVDM-HOST-DIV-609'),
    ('src/mvdm-host/softpc.new/base/system/cmos.c', 'TEST_HARNESS', 2, 'MVDM-HOST-DIV-608'),
    ('src/mvdm-host/softpc.new/base/system/cmos.c', 'FLOPPY_B', 1, 'MVDM-HOST-DIV-612'),
    ('src/mvdm-host/softpc.new/base/dasm386/decode.c', 'CPU_286', 13, 'MVDM-HOST-DIV-607'),
    ('src/non-mvdm/base/win32/server/srvinit.c', 'WX86', 2, 'OPENNT-HOST-150'),
    ('src/non-mvdm/public/internal/ds/inc/token.h', 'DOS3', 1, 'OPENNT-HOST-150'),
    ('src/mvdm-host/softpc.new/base/dasm386/decode.c', 'INTEL_BOOK_NOT_OS2', 1, 'MVDM-HOST-DIV-605'),
    ('src/mvdm-host/softpc.new/base/dasm386/decode.c', 'NO_CHIP_BUG', 1, 'MVDM-HOST-DIV-605'),
    ('src/mvdm-host/softpc.new/base/dasm386/decode.c', 'NPX_287', 1, 'MVDM-HOST-DIV-605'),
    ('src/mvdm-host/softpc.new/base/dasm386/decode.c', 'OLDPIG', 1, 'MVDM-HOST-DIV-605'),
    ('src/mvdm-host/softpc.new/base/inc/cpu3.h', 'SWIN_CPU_OPTS', 1, 'MVDM-HOST-DIV-605'),
    ('src/mvdm-host/softpc.new/base/inc/gore.h', 'INTERLEAVED', 1, 'MVDM-HOST-DIV-605'),
    ('src/mvdm-host/softpc.new/base/inc/host_msw.h', 'MSWDVR_DEBUG', 1, 'MVDM-HOST-DIV-605'),
    ('src/mvdm-host/softpc.new/base/inc/keycaps.h', 'UIF_TEST', 1, 'MVDM-HOST-DIV-605'),
    ('src/mvdm-host/softpc.new/base/inc/novell.h', 'V4CLIENT', 1, 'MVDM-HOST-DIV-605'),
    ('src/mvdm-host/softpc.new/base/inc/smeg_hea.h', 'SMEG', 1, 'MVDM-HOST-DIV-605'),
    ('src/mvdm-host/softpc.new/base/inc/winhelp.h', 'HOST_HELP', 1, 'MVDM-HOST-DIV-605'),
    ('src/mvdm-host/softpc.new/base/system/quick_ev.c', 'QEVENT_TESTER', 1, 'MVDM-HOST-DIV-605'),
    ('src/mvdm-host/softpc.new/dat2obj/dat2obj.c', 'TEST_CASE', 2, 'MVDM-HOST-DIV-605'),
    ('src/mvdm-host/softpc.new/host/src/nt_vflop.c', 'VFLOPPY', 1, 'MVDM-HOST-DIV-605'),
):
    lines = (repo / path).read_text(encoding='latin1').splitlines()
    marker = divergence + ': unsupported ' + macro + ' profile'
    positions = [i for i, line in enumerate(lines) if marker in line]
    if len(positions) != expected_count:
        raise SystemExit('incorrect excluded profile guard count: ' + path)
    for i in positions:
        if not re.match(r'^\s*#\s*ifdef\s+' + re.escape(macro) + r'\s*(?:(?:/\*.*\*/|//.*)\s*)?$', lines[i-1]):
            raise SystemExit('excluded profile lost original selector: ' + path)
        probe.write_text('\n'.join(lines[i-1:i+1] + ['#endif']) + '\n', encoding='ascii')
        allowed = subprocess.run(base, capture_output=True, text=True, errors='replace')
        rejected = subprocess.run(base[:-1] + ['/D' + macro] + base[-1:],
                                  capture_output=True, text=True, errors='replace')
        if allowed.returncode or not rejected.returncode or marker not in rejected.stdout + rejected.stderr:
            raise SystemExit('excluded profile selection failed: ' + path)
print('PASS original excluded host and WOW profile guards')

for path, expression, marker, definitions in (
    ('src/mvdm-host/softpc.new/base/inc/cdrom.h', 'defined(GEN_DRVR)',
     'unsupported GEN_DRVR profile', ['GEN_DRVR']),
    ('src/mvdm-host/softpc.new/base/inc/host_msw.h', 'defined(MSWDVR) && defined(XWINDOW)',
     'unsupported MSWDVR_XWINDOW profile', ['MSWDVR', 'XWINDOW']),
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
