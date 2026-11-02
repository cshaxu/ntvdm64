"""Verify first-depth CPU profile guards and diagnostic line mappings."""

import argparse
import json
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


def unique_sequence(lines, expected, label):
    matches = [i for i in range(len(lines) - len(expected) + 1)
               if [line.strip() for line in lines[i:i + len(expected)]] == expected]
    if len(matches) != 1:
        raise SystemExit(f'{label}: expected one matching guard, found {len(matches)}')
    return matches[0]


def find_guard(lines, condition, marker, following, selected, label):
    matches = []
    for start in range(len(lines) - 1):
        if not re.match(condition, lines[start]) or lines[start + 1].strip() != marker:
            continue
        depth = 1
        alternative = None
        for end in range(start + 2, len(lines)):
            directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|else|elif|endif)\b', lines[end])
            if not directive:
                continue
            kind = directive.group(1)
            if kind in ('if', 'ifdef', 'ifndef'):
                depth += 1
            elif kind == 'endif':
                depth -= 1
                if depth == 0:
                    if (end + 1 < len(lines) and lines[end + 1].strip() == '#line ' + str(following)
                            and ((selected is None and alternative is None) or
                                 (selected is not None and alternative is not None and
                                  lines[alternative + 1].strip() == '#line ' + str(selected)))):
                        matches.append((start, alternative if alternative is not None else end, end))
                    break
            elif depth == 1 and kind == 'else':
                alternative = end
    if len(matches) != 1:
        raise SystemExit(f'{label}: expected one matching guard, found {len(matches)}')
    return matches[0]


ntvdm_arms = json.loads((repo / 'artifacts/mvdm-audit/s13-bfs-depth1-ntvdm-arms.json').read_text(encoding='utf-8'))
for arm in ntvdm_arms:
    source = (repo / arm['path']).read_text(encoding='latin1').split('\n')
    marker = '#error ' + arm['error_marker']
    boundary = '#else' if arm['retained_sibling'] else '#endif'
    matches = [j for j in range(len(source) - 3)
               if source[j].strip() == '#ifndef NTVDM'
               and source[j + 1].strip() == marker
               and re.match(r'^\s*#\s*' + boundary[1:] + r'\b', source[j + 2])
               and source[j + 3].strip() == '#line ' + str(arm['continuation_line'])]
    if len(matches) != 1:
        raise SystemExit('NTVDM guard is missing or ambiguous: ' + arm['path'])
    i = matches[0]
    if source[i].strip() != '#ifndef NTVDM' or source[i + 1].strip() != marker:
        raise SystemExit('NTVDM guard differs: ' + arm['path'])
    if arm['retained_sibling']:
        if not re.match(r'^\s*#\s*else\b', source[i + 2]) or source[i + 3].strip() != '#line ' + str(arm['continuation_line']):
            raise SystemExit('NTVDM selected sibling differs: ' + arm['path'])
        snippet = ['#ifndef NTVDM', marker, '#else', source[i + 3],
                   'int selected_line = __LINE__;', '#endif']
    else:
        if not re.match(r'^\s*#\s*endif\b', source[i + 2]) or source[i + 3].strip() != '#line ' + str(arm['continuation_line']):
            raise SystemExit('NTVDM continuation differs: ' + arm['path'])
        snippet = ['#ifndef NTVDM', marker, '#endif', source[i + 3],
                   'int selected_line = __LINE__;']
    probe = scratch / 'ntvdm-guard.c'
    probe.write_text('\n'.join(snippet) + '\n', encoding='ascii')
    base = ['cl.exe', '/nologo', '/EP', '/TC']
    selected = subprocess.run(base + ['/DNTVDM', str(probe)], capture_output=True, text=True, errors='replace')
    excluded = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
    expected = r'int\s+selected_line\s*=\s*' + str(arm['continuation_line']) + r'\s*;'
    if selected.returncode or not re.search(expected, selected.stdout):
        raise SystemExit('NTVDM selected line failed: ' + arm['path'] + selected.stderr)
    if not excluded.returncode or arm['error_marker'] not in excluded.stdout + excluded.stderr:
        raise SystemExit('non-NTVDM guard failed: ' + arm['path'])
print('PASS six NTVDM root guards, selected lines, and rejected profiles')
timer = (repo / 'src/mvdm-host/softpc.new/base/system/timer.c').read_text(encoding='latin1').split('\n')
timer_start = unique_sequence(timer, ['#ifndef NTVDM', '#error Excluded unsupported non-NTVDM timer profile',
                                     '#else   /* NTVDM */', '#line 1816'], 'timer NTVDM guard')
timer_end = unique_sequence(timer, ['#endif /* NTVDM */', '#line 1959'], 'timer NTVDM end')
if timer_end <= timer_start:
    raise SystemExit('timer NTVDM end precedes its guard')
probe = scratch / 'ntvdm-timer2-guard.c'
probe.write_text('\n'.join(timer[timer_start:timer_start + 4] + ['int selected_line = __LINE__;',
                 '#endif', '#line 1959', 'int following_line = __LINE__;']) + '\n', encoding='ascii')
base = ['cl.exe', '/nologo', '/EP', '/TC']
selected = subprocess.run(base + ['/DNTVDM', str(probe)], capture_output=True, text=True, errors='replace')
excluded = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
if (selected.returncode or not re.search(r'int\s+selected_line\s*=\s*1816\s*;', selected.stdout)
        or not re.search(r'int\s+following_line\s*=\s*1959\s*;', selected.stdout)):
    raise SystemExit('timer NTVDM selected lines failed: ' + selected.stderr)
if not excluded.returncode or 'Excluded unsupported non-NTVDM timer profile' not in excluded.stdout + excluded.stderr:
    raise SystemExit('timer non-NTVDM guard failed')
print('PASS timer NTVDM alternative and both logical line controls')
reset = (repo / 'src/mvdm-host/softpc.new/base/bios/reset.c').read_text(encoding='latin1').split('\n')
expected = ['#ifndef NTVDM\t/* No signon for NTVDM - transparent integration */',
            '#error Excluded unsupported non-NTVDM BIOS signon profile',
            '#endif\t/* NTVDM */', '#line 898']
unique_sequence(reset, expected, 'BIOS signon guard')
probe = scratch / 'ntvdm-reset-guard.c'
probe.write_text('\n'.join(expected + ['int following_line = __LINE__;']) + '\n', encoding='ascii')
selected = subprocess.run(base + ['/DNTVDM', str(probe)], capture_output=True, text=True, errors='replace')
excluded = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
if selected.returncode or not re.search(r'int\s+following_line\s*=\s*898\s*;', selected.stdout):
    raise SystemExit('BIOS signon selected line failed: ' + selected.stderr)
if not excluded.returncode or 'Excluded unsupported non-NTVDM BIOS signon profile' not in excluded.stdout + excluded.stderr:
    raise SystemExit('BIOS signon non-NTVDM guard failed')
print('PASS BIOS signon NTVDM guard and logical line control')
batch3 = json.loads((repo / 'artifacts/mvdm-audit/s13-bfs-depth1-ntvdm-batch3.json').read_text(encoding='utf-8'))
for arm in batch3:
    source = (repo / arm['path']).read_text(encoding='latin1').split('\n')
    marker = '#error Excluded unsupported non-NTVDM ' + ('time service' if arm['path'].endswith('/time_day.c') else 'PIC') + ' profile'
    matches = [j for j in range(len(source) - 3)
               if source[j].strip() == '#ifndef NTVDM'
               and source[j + 1].strip() == marker
               and re.match(r'^\s*#\s*endif\b', source[j + 2])
               and source[j + 3].strip() == '#line ' + str(arm['following_line'])]
    if len(matches) != 1:
        raise SystemExit('NTVDM time/PIC guard is missing or ambiguous: ' + arm['path'])
    i = matches[0]
    if (source[i].strip() != '#ifndef NTVDM' or source[i + 1].strip() != marker
            or not re.match(r'^\s*#\s*endif\b', source[i + 2])
            or source[i + 3].strip() != '#line ' + str(arm['following_line'])):
        raise SystemExit('NTVDM time/PIC guard differs: ' + arm['path'])
    probe = scratch / 'ntvdm-batch3-guard.c'
    probe.write_text('\n'.join(['#ifndef NTVDM', marker, '#endif', source[i + 3],
                               'int following_line = __LINE__;']) + '\n', encoding='ascii')
    selected = subprocess.run(base + ['/DNTVDM', str(probe)], capture_output=True, text=True, errors='replace')
    excluded = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+following_line\s*=\s*' + str(arm['following_line']) + r'\s*;', selected.stdout):
        raise SystemExit('NTVDM time/PIC selected line failed: ' + arm['path'] + selected.stderr)
    if not excluded.returncode or marker[7:] not in excluded.stdout + excluded.stderr:
        raise SystemExit('non-NTVDM time/PIC guard failed: ' + arm['path'])
print('PASS five NTVDM time/PIC guards and logical line controls')
else_arms = json.loads((repo / 'artifacts/mvdm-audit/s13-bfs-depth1-ntvdm-else-new.json').read_text(encoding='utf-8'))
for arm in else_arms:
    source = (repo / arm['path']).read_text(encoding='latin1').split('\n')
    marker = '#error Excluded unsupported non-NTVDM alternative'
    matches = [j for j in range(len(source) - 3)
               if re.match(r'^\s*#\s*else\b', source[j])
               and source[j + 1].strip() == marker
               and re.match(r'^\s*#\s*endif\b', source[j + 2])
               and source[j + 3].strip() == '#line ' + str(arm['following_line'])]
    if len(matches) != 1:
        raise SystemExit('NTVDM alternative is missing or ambiguous: ' + arm['path'])
    i = matches[0]
    stack = []
    for line in source[:i]:
        directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
        if directive and directive.group(1) == 'endif':
            if stack:
                stack.pop()
        elif directive:
            stack.append(line)
    if not stack or not re.match(r'^\s*#\s*ifdef\s+NTVDM\b', stack[-1]):
        raise SystemExit('NTVDM alternative parent differs: ' + arm['path'])
    probe = scratch / 'ntvdm-else-guard.c'
    probe.write_text('\n'.join(['#ifdef NTVDM', 'int selected = 1;', '#else', marker,
                               '#endif', source[i + 3].strip(), 'int following_line = __LINE__;']) + '\n', encoding='ascii')
    selected = subprocess.run(base + ['/DNTVDM', str(probe)], capture_output=True, text=True, errors='replace')
    excluded = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+following_line\s*=\s*' + str(arm['following_line']) + r'\s*;', selected.stdout):
        raise SystemExit('NTVDM alternative selected line failed: ' + arm['path'] + selected.stderr)
    if not excluded.returncode or marker[7:] not in excluded.stdout + excluded.stderr:
        raise SystemExit('non-NTVDM alternative guard failed: ' + arm['path'])
print('PASS five NTVDM alternatives and logical line controls')
debug_arms = json.loads((repo / 'artifacts/mvdm-audit/s13-bfs-depth1-debug-wkman.json').read_text(encoding='utf-8'))
debug_arms += json.loads((repo / 'artifacts/mvdm-audit/s13-bfs-depth1-debug-wow-batch.json').read_text(encoding='utf-8'))
debug_arms += json.loads((repo / 'artifacts/mvdm-audit/s13-bfs-depth1-debug-wow-special.json').read_text(encoding='utf-8'))
for arm in debug_arms:
    source = (repo / arm['path']).read_bytes().decode('latin1').split('\n')
    marker = '#error Excluded unsupported DEBUG WOW profile'
    i, cut, end = find_guard(source, r'^\s*#\s*ifdef\s+DEBUG\b', marker,
                             arm['following_line'], arm['selected_alternative_line'], arm['path'])
    if (not re.match(r'^\s*#\s*ifdef\s+DEBUG\b', source[i]) or source[i + 1].strip() != marker
            or not re.match(r'^\s*#\s*endif\b', source[end])
            or source[end + 1].strip() != '#line ' + str(arm['following_line'])):
        raise SystemExit('WOW DEBUG outer guard differs: ' + arm['path'])
    snippet = ['#ifdef DEBUG', marker]
    if arm['selected_alternative_line'] is not None:
        if (not re.match(r'^\s*#\s*else\b', source[cut]) or
                source[cut + 1].strip() != '#line ' + str(arm['selected_alternative_line'])):
            raise SystemExit('WOW DEBUG selected alternative differs: ' + arm['path'])
        snippet += ['#else', source[cut + 1].strip(), 'int selected_line = __LINE__;']
    snippet += ['#endif', source[end + 1].strip(), 'int following_line = __LINE__;']
    probe = scratch / 'wow-debug-guard.c'
    probe.write_text('\n'.join(snippet) + '\n', encoding='ascii')
    selected = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
    excluded = subprocess.run(base + ['/DDEBUG', str(probe)], capture_output=True, text=True, errors='replace')
    if (selected.returncode or not re.search(r'int\s+following_line\s*=\s*' + str(arm['following_line']) + r'\s*;', selected.stdout)
            or (arm['selected_alternative_line'] is not None and not re.search(r'int\s+selected_line\s*=\s*' + str(arm['selected_alternative_line']) + r'\s*;', selected.stdout))):
        raise SystemExit('WOW DEBUG selected line failed: ' + selected.stderr)
    if not excluded.returncode or marker[7:] not in excluded.stdout + excluded.stderr:
        raise SystemExit('unsupported WOW DEBUG profile did not fail')
print('PASS WOW DEBUG guards, alternatives and line controls')
spc_arms = json.loads((repo / 'artifacts/mvdm-audit/s13-bfs-depth1-spc486.json').read_text(encoding='utf-8'))
snippet = []
for index, arm in enumerate(spc_arms):
    source = (repo / arm['path']).read_bytes().decode('latin1').split('\n')
    marker = '#error Excluded unsupported SPC486 profile'
    start, cut, end = find_guard(source, r'^\s*#\s*ifdef\s+SPC486\b', marker,
                                 arm['following_line'], arm['selected_alternative_line'], arm['path'])
    if (not re.match(r'^\s*#\s*ifdef\s+SPC486\b', source[start]) or source[start + 1].strip() != marker
            or not re.match(r'^\s*#\s*endif\b', source[end])
            or source[end + 1].strip() != '#line ' + str(arm['following_line'])):
        raise SystemExit('SPC486 outer guard differs: ' + arm['path'])
    snippet += ['#ifdef SPC486', marker]
    if arm['selected_alternative_line'] is not None:
        if (not re.match(r'^\s*#\s*else\b', source[cut]) or
                source[cut + 1].strip() != '#line ' + str(arm['selected_alternative_line'])):
            raise SystemExit('SPC486 selected alternative differs: ' + arm['path'])
        snippet += ['#else', source[cut + 1].strip(), f'int selected_{index} = __LINE__;']
    snippet += ['#endif', source[end + 1].strip(), f'int following_{index} = __LINE__;']
probe = scratch / 'spc486-guards.c'
probe.write_text('\n'.join(snippet) + '\n', encoding='ascii')
selected = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
excluded = subprocess.run(base + ['/DSPC486', str(probe)], capture_output=True, text=True, errors='replace')
for index, arm in enumerate(spc_arms):
    if (selected.returncode or not re.search(r'int\s+following_' + str(index) + r'\s*=\s*' + str(arm['following_line']) + r'\s*;', selected.stdout)
            or (arm['selected_alternative_line'] is not None and not re.search(r'int\s+selected_' + str(index) + r'\s*=\s*' + str(arm['selected_alternative_line']) + r'\s*;', selected.stdout))):
        raise SystemExit('SPC486 selected line failed: ' + arm['path'] + selected.stderr)
if not excluded.returncode or marker[7:] not in excluded.stdout + excluded.stderr:
    raise SystemExit('unsupported SPC486 profile did not fail')
print('PASS SPC486 guards, alternatives and line controls')
lines = (repo / 'src/mvdm-host/softpc.new/base/support/main.c').read_text(
    encoding='latin1').split('\n')
marker = '#error Excluded unsupported non-CPU40 host startup profile'
matches = [i for i, line in enumerate(lines) if line.strip() == marker]
if len(matches) != 1:
    raise SystemExit('missing unique startup guard')
i = matches[0]
expected = ['#ifndef CPU_40_STYLE', marker, '#else', '#line 61',
            'extern void host_start_cpu(void);', '#endif', '#line 62',
            'extern int InitialiseDosEmulation(int argc, char **argv);']
if [line.strip() for line in lines[i-1:i+7]] != expected:
    raise SystemExit('startup guard, selected declaration or line controls changed')

probe = scratch / 'startup-guard.c'
probe.write_text('\n'.join(expected[:4] + ['int selected_line = __LINE__;'] +
                           expected[5:7] + ['int following_line = __LINE__;']) + '\n',
                 encoding='ascii')
base = ['cl.exe', '/nologo', '/EP', '/TC']
selected = subprocess.run(base + ['/DCPU_40_STYLE', str(probe)],
                          capture_output=True, text=True, errors='replace')
excluded = subprocess.run(base + [str(probe)], capture_output=True,
                          text=True, errors='replace')
if (selected.returncode or not re.search(r'int\s+selected_line\s*=\s*61\s*;', selected.stdout)
        or not re.search(r'int\s+following_line\s*=\s*62\s*;', selected.stdout)):
    raise SystemExit('CPU40 guard or diagnostic line mapping failed: ' + selected.stderr)
if not excluded.returncode or marker[7:] not in excluded.stdout + excluded.stderr:
    raise SystemExit('unsupported non-CPU40 path did not fail at the restored guard')
print('PASS CPU40 declaration, line mapping, and unsupported non-CPU40 rejection')

lines = (repo / 'src/mvdm-host/softpc.new/base/support/xt.c').read_text(
    encoding='latin1').split('\n')
condition = '#if !defined(MAC_LIKE) && !defined(CPU_40_STYLE)'
matches = [i for i, line in enumerate(lines) if line.strip() == condition]
if len(matches) != 1:
    raise SystemExit('missing unique parity-table guard')
i = matches[0]
expected = ['#if !defined(MAC_LIKE) && !defined(CPU_30_STYLE)', condition,
            '#error Excluded unsupported non-CPU40 parity-table profile',
            '#endif /* !defined(MAC_LIKE) && !defined(CPU_40_STYLE) */',
            '#endif /* !defined(MAC_LIKE) && !defined(CPU_30_STYLE) */', '#line 607']
if [line.strip() for line in lines[i-1:i+5]] != expected:
    raise SystemExit('parity-table guard or line control changed')
probe.write_text('\n'.join(expected + ['int following_line = __LINE__;']) + '\n',
                 encoding='ascii')
for defines in (['/DCPU_40_STYLE'], ['/DMAC_LIKE'], ['/DCPU_30_STYLE']):
    selected = subprocess.run(base + defines + [str(probe)], capture_output=True,
                              text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+following_line\s*=\s*607\s*;', selected.stdout):
        raise SystemExit('parity-table selected branch or line mapping failed: ' + selected.stderr)
excluded = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
if not excluded.returncode or 'Excluded unsupported non-CPU40 parity-table profile' not in excluded.stdout + excluded.stderr:
    raise SystemExit('unsupported parity-table arm did not fail at its guard')
print('PASS parity-table conjunction, line mapping, and unsupported arm rejection')

lines = (repo / 'src/mvdm-host/softpc.new/base/support/main.c').read_text(
    encoding='latin1').split('\n')
condition = '#if defined(PROFILE) && !defined(CPU_40_STYLE)'
matches = [i for i, line in enumerate(lines) if line.strip() == condition]
if len(matches) != 1:
    raise SystemExit('missing unique profiling startup guard')
i = matches[0]
marker = '#error Excluded unsupported non-CPU40 profiling startup'
if (lines[i+1].strip() != marker or not re.match(r'^\s*#\s*endif\b', lines[i+2])
        or lines[i+3].strip() != '#line 221'):
    raise SystemExit('profiling startup guard or line control changed')
probe.write_text('\n'.join(lines[i:i+4] + ['int following_line = __LINE__;']) + '\n',
                 encoding='ascii')
for defines in (['/DCPU_40_STYLE', '/DPROFILE'], []):
    selected = subprocess.run(base + defines + [str(probe)], capture_output=True,
                              text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+following_line\s*=\s*221\s*;', selected.stdout):
        raise SystemExit('profiling conjunction or line mapping failed: ' + selected.stderr)
excluded = subprocess.run(base + ['/DPROFILE', str(probe)], capture_output=True,
                          text=True, errors='replace')
if not excluded.returncode or marker[7:] not in excluded.stdout + excluded.stderr:
    raise SystemExit('unsupported profiling arm did not fail at its guard')
print('PASS profiling conjunction, line mapping, and unsupported arm rejection')

lines = (repo / 'src/mvdm-host/softpc.new/host/src/config.c').read_text(
    encoding='latin1').split('\n')
marker = '#error Excluded original conditional branch'
matches = [i for i, line in enumerate(lines) if line.strip() == marker
           and i > 0 and lines[i-1].strip() == '#ifdef MONITOR'
           and i+2 < len(lines) and lines[i+1].strip() == '#endif'
           and lines[i+2].strip() == '#line 886']
if len(matches) != 1:
    raise SystemExit('missing unique monitor backfill guard')
i = matches[0]
probe.write_text('\n'.join(lines[i-1:i+3] + ['int following_line = __LINE__;']) + '\n',
                 encoding='ascii')
selected = subprocess.run(base + [str(probe)], capture_output=True,
                          text=True, errors='replace')
excluded = subprocess.run(base + ['/DMONITOR', str(probe)], capture_output=True,
                          text=True, errors='replace')
if selected.returncode or not re.search(r'int\s+following_line\s*=\s*886\s*;', selected.stdout):
    raise SystemExit('monitor backfill selected branch or line mapping failed: ' + selected.stderr)
if not excluded.returncode or marker[7:] not in excluded.stdout + excluded.stderr:
    raise SystemExit('unsupported monitor backfill arm did not fail at its guard')
print('PASS single monitor backfill guard, line mapping, and unsupported arm rejection')

condition = '#if (defined(i386) || defined(_M_IX86)) && !defined(CPU_40_STYLE)'
matches = [i for i, line in enumerate(lines) if line.strip() == condition]
if len(matches) != 1:
    raise SystemExit('missing unique i386 memory-size guard')
i = matches[0]
marker = '#error Excluded unsupported non-CPU40 i386 memory-size profile'
if (lines[i+1].strip() != marker or lines[i+2].strip() != '#else'
        or lines[i+3].strip() != '#line 510'
        or lines[i+4].strip() != 'vdmMemorySize = GetVDMSize(VDMForWOW);'):
    raise SystemExit('memory-size guard, selected calculation or line control changed')
probe.write_text('\n'.join(lines[i:i+4] + ['int selected_line = __LINE__;', '#endif']) + '\n',
                 encoding='ascii')
selected = subprocess.run(base + ['/DCPU_40_STYLE', '/Di386', str(probe)],
                          capture_output=True, text=True, errors='replace')
excluded = subprocess.run(base + ['/Di386', str(probe)], capture_output=True,
                          text=True, errors='replace')
if selected.returncode or not re.search(r'int\s+selected_line\s*=\s*510\s*;', selected.stdout):
    raise SystemExit('CPU40 memory-size selection or line mapping failed: ' + selected.stderr)
if not excluded.returncode or marker[7:] not in excluded.stdout + excluded.stderr:
    raise SystemExit('unsupported i386 memory-size arm did not fail at its guard')
print('PASS CPU40 memory-size selection, line mapping, and unsupported arm rejection')

lines = (repo / 'src/mvdm-host/softpc.new/base/support/xt.c').read_text(
    encoding='latin1').split('\n')
outer = '#if defined(CCPU) && !defined(CPU_30_STYLE)'
inner = '#if defined(CCPU) && !defined(CPU_40_STYLE)'
matches = [i for i, line in enumerate(lines) if line.strip() == outer]
if len(matches) != 1:
    raise SystemExit('missing restored original xt.c outer selector')
i = matches[0]
expected = [outer, inner,
            '#error DIVERGENCE MVDM-HOST-DIV-515: compound branch is not selected for this source; see mirror register.',
            '#endif /* defined(CCPU) && !defined(CPU_40_STYLE) */',
            '#endif /* defined(CCPU) && !defined(CPU_30_STYLE) */', '#line 147']
if [line.strip() for line in lines[i:i+6]] != expected:
    raise SystemExit('xt.c original selector or existing adaptation changed')
probe.write_text('\n'.join(expected + ['int after_outer = __LINE__;']) + '\n', encoding='ascii')
selected = subprocess.run(base + ['/DCCPU', '/DCPU_40_STYLE', str(probe)],
                          capture_output=True, text=True, errors='replace')
excluded = subprocess.run(base + ['/DCCPU', str(probe)],
                          capture_output=True, text=True, errors='replace')
if selected.returncode or not re.search(r'int\s+after_outer\s*=\s*147\s*;', selected.stdout):
    raise SystemExit('xt.c selected outer selector or original line mapping failed: ' + selected.stderr)
if not excluded.returncode or 'MVDM-HOST-DIV-515' not in excluded.stdout + excluded.stderr:
    raise SystemExit('xt.c existing non-CPU40 adaptation no longer rejects')
line_matches = [i for i, line in enumerate(lines) if line.strip() == '#line 340']
if len(line_matches) != 1 or lines[line_matches[0]-1].strip() != '#endif /* CPU_30_STYLE */':
    raise SystemExit('xt.c original post-CPU30 logical line mapping changed')
print('PASS xt.c original outer selector, existing adaptation and line mappings')

lines = (repo / 'src/mvdm-host/softpc.new/host/src/nt_graph.c').read_text(
    encoding='latin1').split('\n')
outer = '#ifndef X86GFX'
inner = '#if !defined(X86GFX) && !defined(CCPU)'
matches = [i for i, line in enumerate(lines) if line.strip() == inner]
if len(matches) != 1:
    raise SystemExit('missing unique nt_graph CCPU display selector')
i = matches[0]
expected = [outer, inner,
            '#error DIVERGENCE MVDM-HOST-DIV-580: non-CCPU non-X86GFX display branch is not selected',
            '#endif', '#endif /* !X86GFX */', '#line 764']
if [line.strip() for line in lines[i-1:i+5]] != expected:
    raise SystemExit('nt_graph original selector or existing adaptation changed')
probe.write_text('\n'.join(expected + ['int after_outer = __LINE__;']) + '\n', encoding='ascii')
for defines in (['/DCCPU'], ['/DX86GFX']):
    selected = subprocess.run(base + defines + [str(probe)], capture_output=True,
                              text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+after_outer\s*=\s*764\s*;', selected.stdout):
        raise SystemExit('nt_graph original selector or line mapping failed: ' + selected.stderr)
excluded = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
if not excluded.returncode or 'MVDM-HOST-DIV-580' not in excluded.stdout + excluded.stderr:
    raise SystemExit('nt_graph existing non-CCPU adaptation no longer rejects')
print('PASS nt_graph original outer selector, existing adaptation and line mapping')

for header, continuation, payload in (
    ('mvdm.h', 84, 'FETCHWORD(x)\nFETCHDWORD(x)\nSTOREWORD(x,y)\nSTOREDWORD(x,y)'),
    ('suballoc.h', 37, 'int selected = COMMIT_GRANULARITY;'),
):
    text = (repo / 'src/mvdm-host/inc' / header).read_text(encoding='latin1')
    start = text.index('#ifdef i386\n')
    end = text.index(f'#line {continuation}\n', start) + len(f'#line {continuation}\n')
    group = text[start:end]
    outer, rest = group.split('\n#else\n#if defined(_M_IX86)\n', 1)
    adapted, rest = rest.split('\n#else\n', 1)
    generic, tail = rest.split('\n#endif\n#endif\n', 1)
    direct = outer.split('\n', 1)[1]
    if direct != adapted or tail.strip() != f'#line {continuation}':
        raise SystemExit(f'{header}: original i386 boundary or MSVC selection changed')
    baseline = '#if defined(i386) || defined(_M_IX86)\n' + direct + '\n#else\n' + generic + '\n#endif\n'
    profiles = (['/Di386'], ['/U_M_IX86', '/D_M_IX86=600'], ['/U_M_IX86', '/Ui386'])
    for defines in profiles:
        outputs = []
        for content in (baseline, group):
            probe.write_text(content + payload + '\n', encoding='ascii')
            result = subprocess.run(base + defines + [str(probe)], capture_output=True,
                                    text=True, errors='replace')
            if result.returncode:
                raise SystemExit(f'{header}: macro profile failed: ' + result.stderr)
            outputs.append('\n'.join(line for line in result.stdout.splitlines() if line.strip()))
        if outputs[0] != outputs[1]:
            raise SystemExit(f'{header}: original selector changed macro output')
    probe.write_text(group + 'int after_outer = __LINE__;\n', encoding='ascii')
    line = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
    if line.returncode or not re.search(r'int\s+after_outer\s*=\s*' + str(continuation) + r'\s*;', line.stdout):
        raise SystemExit(f'{header}: original continuation line changed')
    first_line, other_line = ((74, 79) if header == 'mvdm.h' else (33, 35))
    occurrences = [(first_line, 0), (first_line, 1), (other_line, 0)]
    for defines, (expected_line, occurrence) in zip(profiles, occurrences):
        marker = f'#line {expected_line}\n'
        parts = group.split(marker)
        if len(parts) <= occurrence + 1:
            raise SystemExit(f'{header}: missing original alternative line directive')
        parts[occurrence + 1] = '#error BRANCH_LINE_CONTROL\n' + parts[occurrence + 1]
        probe.write_text(marker.join(parts), encoding='ascii')
        line = subprocess.run(base + defines + [str(probe)], capture_output=True,
                              text=True, errors='replace')
        if not line.returncode or not re.search(r'\(' + str(expected_line) + r'\).*BRANCH_LINE_CONTROL',
                                                line.stdout + line.stderr):
            raise SystemExit(f'{header}: original alternative diagnostic line changed')
print('PASS original i386 header selectors, MSVC profiles and continuation lines')
