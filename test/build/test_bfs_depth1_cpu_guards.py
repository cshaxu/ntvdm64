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
    matches = []
    for i in range(len(lines)):
        j = i
        for item in expected:
            if item.startswith('#error '):
                while j < len(lines) and re.match(r'^\s*#\s*line\b', lines[j]):
                    j += 1
            if j >= len(lines) or lines[j].strip() != item:
                break
            j += 1
        else:
            matches.append(i)
    if len(matches) != 1:
        raise SystemExit(f'{label}: expected one matching guard, found {len(matches)}')
    return matches[0]


def guard_error_index(lines, start):
    index = start + 1
    while index < len(lines) and re.match(r'^\s*#\s*line\b', lines[index]):
        index += 1
    return index


def find_guard(lines, condition, marker, following, selected, label):
    matches = []
    for start in range(len(lines) - 1):
        error_index = guard_error_index(lines, start)
        if not re.match(condition, lines[start]) or error_index >= len(lines) or lines[error_index].strip() != marker:
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
        raise SystemExit(f'{label}: expected one matching guard for following={following}, selected={selected}; found {matches}')
    return matches[0]


ntvdm_arms = json.loads((repo / 'artifacts/mvdm-audit/s13-bfs-depth1-ntvdm-arms.json').read_text(encoding='utf-8'))
line_corrections = {}
for batch in (2, 3, 4):
    records = json.loads((repo / f'artifacts/mvdm-audit/s13-depth1-unique-anchor-batch{batch}.json').read_text(encoding='utf-8'))
    for record in records:
        for anchor in record['anchors']:
            key = (record['path'], anchor['old'])
            if key in line_corrections and line_corrections[key] != anchor['new']:
                raise SystemExit(f'ambiguous original line correction: {key}')
            line_corrections[key] = anchor['new']
ica_else = json.loads((repo / 'artifacts/mvdm-audit/s13-bfs-depth1-ntvdm-else-new.json').read_text(encoding='utf-8'))
for record in ica_else:
    if record['path'].endswith('/base/system/ica.c'):
        line_corrections[(record['path'], record['following_line'])] = record['original_end'] + 1
for arm in ntvdm_arms:
    arm = dict(arm)
    arm['continuation_line'] = line_corrections.get(
        (arm['path'], arm['continuation_line']), arm['continuation_line'])
    if not arm['retained_sibling']:
        # An excluded arm's continuation immediately follows its original endif.
        arm['continuation_line'] = arm['original_arm_lines'][1] + 1
    source = (repo / arm['path']).read_text(encoding='latin1').split('\n')
    marker = '#error ' + arm['error_marker']
    boundary = '#else' if arm['retained_sibling'] else '#endif'
    matches = []
    for j in range(len(source) - 3):
        if source[j].strip() != '#ifndef NTVDM':
            continue
        error = guard_error_index(source, j)
        if (error + 2 < len(source) and source[error].strip() == marker
                and re.match(r'^\s*#\s*' + boundary[1:] + r'\b', source[error + 1])
                and source[error + 2].strip() == '#line ' + str(arm['continuation_line'])):
            matches.append((j, error))
    if len(matches) != 1:
        raise SystemExit('NTVDM guard is missing or ambiguous: ' + arm['path'])
    i, error = matches[0]
    if source[i].strip() != '#ifndef NTVDM' or source[error].strip() != marker:
        raise SystemExit('NTVDM guard differs: ' + arm['path'])
    if arm['retained_sibling']:
        if not re.match(r'^\s*#\s*else\b', source[error + 1]) or source[error + 2].strip() != '#line ' + str(arm['continuation_line']):
            raise SystemExit('NTVDM selected sibling differs: ' + arm['path'])
        snippet = ['#ifndef NTVDM', marker, '#else', source[error + 2],
                   'int selected_line = __LINE__;', '#endif']
    else:
        if not re.match(r'^\s*#\s*endif\b', source[error + 1]) or source[error + 2].strip() != '#line ' + str(arm['continuation_line']):
            raise SystemExit('NTVDM continuation differs: ' + arm['path'])
        snippet = ['#ifndef NTVDM', marker, '#endif', source[error + 2],
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
timer_matches = []
for i, line in enumerate(timer):
    if line.strip() != '#ifndef NTVDM':
        continue
    j = guard_error_index(timer, i)
    if (j + 2 < len(timer) and timer[j].strip() == '#error Excluded unsupported non-NTVDM timer profile'
            and timer[j + 1].strip() == '#else   /* NTVDM */' and timer[j + 2].strip() == '#line 1950'):
        timer_matches.append((i, j))
if len(timer_matches) != 1:
    raise SystemExit('timer NTVDM guard missing or ambiguous')
timer_start, timer_error = timer_matches[0]
timer_end = unique_sequence(timer, ['#endif /* NTVDM */', '#line 2093'], 'timer NTVDM end')
if timer_end <= timer_start:
    raise SystemExit('timer NTVDM end precedes its guard')
probe = scratch / 'ntvdm-timer2-guard.c'
probe.write_text('\n'.join(timer[timer_start:timer_error + 3] + ['int selected_line = __LINE__;',
                 '#endif', '#line 2093', 'int following_line = __LINE__;']) + '\n', encoding='ascii')
base = ['cl.exe', '/nologo', '/EP', '/TC']
selected = subprocess.run(base + ['/DNTVDM', str(probe)], capture_output=True, text=True, errors='replace')
excluded = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
if (selected.returncode or not re.search(r'int\s+selected_line\s*=\s*1950\s*;', selected.stdout)
        or not re.search(r'int\s+following_line\s*=\s*2093\s*;', selected.stdout)):
    raise SystemExit('timer NTVDM selected lines failed: ' + selected.stderr)
if not excluded.returncode or 'Excluded unsupported non-NTVDM timer profile' not in excluded.stdout + excluded.stderr:
    raise SystemExit('timer non-NTVDM guard failed')
print('PASS timer NTVDM alternative and both logical line controls')
timer = (repo / 'src/mvdm-host/softpc.new/host/src/nt_timer.c').read_text(encoding='latin1').split('\n')
start = next(i for i, line in enumerate(timer) if line.strip() == '#line 251'
             and i + 1 < len(timer) and timer[i + 1].strip() == 'void host_timer_init(void)')
calls = [i for i in range(start, len(timer))
         if 'DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__)' in timer[i]][:3]
if len(calls) != 3:
    raise SystemExit('timer initialization diagnostic calls missing')
snippet = timer[start:calls[-1] + 1]
for n, index in enumerate(calls):
    snippet[index - start] = f'int timer_diagnostic_line_{n} = __LINE__;'
probe.write_text('\n'.join(snippet) + '\n', encoding='ascii')
result = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
if result.returncode or any(not re.search(rf'int\s+timer_diagnostic_line_{n}\s*=\s*{line}\s*;', result.stdout)
                            for n, line in enumerate((264, 286, 291))):
    raise SystemExit('timer initialization original diagnostic lines not preserved: ' + result.stderr)
print('PASS original timer initialization diagnostic line mapping')
reset = (repo / 'src/mvdm-host/softpc.new/base/bios/reset.c').read_text(encoding='latin1').split('\n')
expected = ['#ifndef NTVDM\t/* No signon for NTVDM - transparent integration */',
            '#error Excluded unsupported non-NTVDM BIOS signon profile',
            '#endif\t/* NTVDM */', '#line 985']
unique_sequence(reset, expected, 'BIOS signon guard')
probe = scratch / 'ntvdm-reset-guard.c'
probe.write_text('\n'.join(expected + ['int following_line = __LINE__;']) + '\n', encoding='ascii')
selected = subprocess.run(base + ['/DNTVDM', str(probe)], capture_output=True, text=True, errors='replace')
excluded = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
if selected.returncode or not re.search(r'int\s+following_line\s*=\s*985\s*;', selected.stdout):
    raise SystemExit('BIOS signon selected line failed: ' + selected.stderr)
if not excluded.returncode or 'Excluded unsupported non-NTVDM BIOS signon profile' not in excluded.stdout + excluded.stderr:
    raise SystemExit('BIOS signon non-NTVDM guard failed')
print('PASS BIOS signon NTVDM guard and logical line control')
batch3 = json.loads((repo / 'artifacts/mvdm-audit/s13-bfs-depth1-ntvdm-batch3.json').read_text(encoding='utf-8'))
for arm in batch3:
    arm = dict(arm)
    arm['following_line'] = arm['original_arm_lines'][1] + 1
    source = (repo / arm['path']).read_text(encoding='latin1').split('\n')
    marker = '#error Excluded unsupported non-NTVDM ' + ('time service' if arm['path'].endswith('/time_day.c') else 'PIC') + ' profile'
    matches = []
    for j, line in enumerate(source):
        if line.strip() != '#ifndef NTVDM':
            continue
        error = guard_error_index(source, j)
        if (error + 2 < len(source) and source[error].strip() == marker
                and re.match(r'^\s*#\s*endif\b', source[error + 1])
                and source[error + 2].strip() == '#line ' + str(arm['following_line'])):
            matches.append((j, error))
    if len(matches) != 1:
        raise SystemExit('NTVDM time/PIC guard is missing or ambiguous: ' + arm['path'])
    i, error = matches[0]
    if (source[i].strip() != '#ifndef NTVDM' or source[error].strip() != marker
            or not re.match(r'^\s*#\s*endif\b', source[error + 1])
            or source[error + 2].strip() != '#line ' + str(arm['following_line'])):
        raise SystemExit('NTVDM time/PIC guard differs: ' + arm['path'])
    probe = scratch / 'ntvdm-batch3-guard.c'
    probe.write_text('\n'.join(['#ifndef NTVDM', marker, '#endif', source[error + 2],
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
    arm = dict(arm)
    arm['following_line'] = arm['original_end'] + 1
    source = (repo / arm['path']).read_text(encoding='latin1').split('\n')
    marker = '#error Excluded unsupported non-NTVDM alternative'
    matches = []
    for j, line in enumerate(source):
        if not re.match(r'^\s*#\s*else\b', line):
            continue
        error = guard_error_index(source, j)
        if (error + 2 < len(source) and source[error].strip() == marker
                and re.match(r'^\s*#\s*endif\b', source[error + 1])
                and source[error + 2].strip() == '#line ' + str(arm['following_line'])):
            matches.append((j, error))
    if len(matches) != 1:
        raise SystemExit('NTVDM alternative is missing or ambiguous: ' + arm['path'])
    i, error = matches[0]
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
                               '#endif', source[error + 2].strip(), 'int following_line = __LINE__;']) + '\n', encoding='ascii')
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
    arm = dict(arm)
    arm['following_line'] = arm['original_end'] + 1
    if arm['selected_alternative_line'] is not None:
        arm['selected_alternative_line'] = arm['original_cut'] + 1
    source = (repo / arm['path']).read_bytes().decode('latin1').split('\n')
    marker = '#error Excluded unsupported DEBUG WOW profile'
    i, cut, end = find_guard(source, r'^\s*#\s*ifdef\s+DEBUG\b', marker,
                             arm['following_line'], arm['selected_alternative_line'], arm['path'])
    if (not re.match(r'^\s*#\s*ifdef\s+DEBUG\b', source[i]) or source[guard_error_index(source, i)].strip() != marker
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
    arm = dict(arm)
    arm['following_line'] = arm['original_end'] + 1
    if arm['selected_alternative_line'] is not None:
        arm['selected_alternative_line'] = arm['original_cut'] + 1
    source = (repo / arm['path']).read_bytes().decode('latin1').split('\n')
    marker = '#error Excluded unsupported SPC486 profile'
    start, cut, end = find_guard(source, r'^\s*#\s*ifdef\s+SPC486\b', marker,
                                 arm['following_line'], arm['selected_alternative_line'], arm['path'])
    if (not re.match(r'^\s*#\s*ifdef\s+SPC486\b', source[start]) or source[guard_error_index(source, start)].strip() != marker
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
    arm = dict(arm)
    arm['following_line'] = arm['original_end'] + 1
    if arm['selected_alternative_line'] is not None:
        arm['selected_alternative_line'] = arm['original_cut'] + 1
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
            'extern void host_start_cpu(void);', '#endif', '#line 57',
            'extern int InitialiseDosEmulation(int argc, char **argv);']
start = i - 1
while start >= 0 and re.match(r'^\s*#\s*line\b', lines[start]):
    start -= 1
if guard_error_index(lines, start) != i or [lines[start].strip()] + [line.strip() for line in lines[i:i+7]] != expected:
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
        or not re.search(r'int\s+following_line\s*=\s*57\s*;', selected.stdout)):
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
error_index = guard_error_index(lines, i)
following_index = error_index + 2
while following_index + 1 < len(lines) and lines[following_index + 1].strip().startswith('#line '):
    following_index += 1
if (lines[error_index].strip() != marker or not re.match(r'^\s*#\s*endif\b', lines[error_index+1])
        or lines[following_index].strip() != '#line 217'):
    raise SystemExit('profiling startup guard or line control changed')
probe.write_text('\n'.join(lines[i:following_index+1] + ['int following_line = __LINE__;']) + '\n',
                 encoding='ascii')
for defines in (['/DCPU_40_STYLE', '/DPROFILE'], []):
    selected = subprocess.run(base + defines + [str(probe)], capture_output=True,
                              text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+following_line\s*=\s*217\s*;', selected.stdout):
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
           and i > 1 and lines[i-2].strip() == '#ifdef MONITOR'
           and lines[i-1].strip() == '#line 1195'
           and i+2 < len(lines) and lines[i+1].strip() == '#endif'
           and lines[i+2].strip() == '#line 1215']
if len(matches) != 1:
    raise SystemExit('missing unique monitor backfill guard')
i = matches[0]
probe.write_text('\n'.join(lines[i-2:i+3] + ['int following_line = __LINE__;']) + '\n',
                 encoding='ascii')
selected = subprocess.run(base + [str(probe)], capture_output=True,
                          text=True, errors='replace')
excluded = subprocess.run(base + ['/DMONITOR', str(probe)], capture_output=True,
                          text=True, errors='replace')
if selected.returncode or not re.search(r'int\s+following_line\s*=\s*1215\s*;', selected.stdout):
    raise SystemExit('monitor backfill selected branch or line mapping failed: ' + selected.stderr)
if not excluded.returncode or marker[7:] not in excluded.stdout + excluded.stderr:
    raise SystemExit('unsupported monitor backfill arm did not fail at its guard')
print('PASS single monitor backfill guard, line mapping, and unsupported arm rejection')

marker = '#error Excluded unsupported non-CPU40 i386 memory-size profile'
matches = [i for i, line in enumerate(lines) if line.strip() == '#ifdef i386'
           and i + 2 < len(lines) and lines[i + 1].strip() == '#if defined(CPU_40_STYLE)'
           and lines[i + 2].strip() == '#line 510']
if len(matches) != 1:
    raise SystemExit('original i386 memory-size outer selector missing')
start = matches[0]
depth = 1
for end in range(start + 1, len(lines)):
    directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', lines[end])
    if not directive:
        continue
    if directive.group(1) in ('if', 'ifdef', 'ifndef'):
        depth += 1
    else:
        depth -= 1
        if not depth:
            break
else:
    raise SystemExit('original i386 memory-size outer selector unclosed')
if lines[end + 1].strip() != '#line 523':
    raise SystemExit('original memory-size continuation changed')
probe.write_text('\n'.join(lines[start:end + 2] + ['int after_config = __LINE__;']) + '\n', encoding='ascii')
selected_outputs = []
for defines in (['/Ui386', '/U_M_IX86'], ['/Ui386', '/U_M_IX86', '/DCPU_40_STYLE'],
                ['/Ui386', '/D_M_IX86=600', '/DCPU_40_STYLE'], ['/Di386', '/U_M_IX86', '/DCPU_40_STYLE']):
    selected = subprocess.run(base + defines + [str(probe)], capture_output=True, text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+after_config\s*=\s*523\s*;', selected.stdout):
        raise SystemExit('CPU40 memory-size selection or original line failed: ' + selected.stderr)
    selected_outputs.append('\n'.join(line for line in selected.stdout.splitlines() if line.strip()))
if len(set(selected_outputs)) != 1:
    raise SystemExit('CPU40 memory-size profile outputs differ')
for defines in (['/Ui386', '/D_M_IX86=600'], ['/Di386', '/U_M_IX86']):
    excluded = subprocess.run(base + defines + [str(probe)], capture_output=True, text=True, errors='replace')
    if not excluded.returncode or marker[7:] not in excluded.stdout + excluded.stderr:
        raise SystemExit('unsupported i386 memory-size arm did not fail at its guard')
print('PASS original memory-size outer selector, six profiles and line mapping')

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
    if '#error Excluded unsupported historical i386' not in direct or tail.strip() != f'#line {continuation}':
        raise SystemExit(f'{header}: original i386 boundary or MSVC selection changed')
    baseline = '#if defined(_M_IX86)\n' + adapted + '\n#else\n' + generic + '\n#endif\n'
    profiles = (['/U_M_IX86', '/D_M_IX86=600'], ['/U_M_IX86', '/Ui386'])
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
    probe.write_text(group + payload + '\n', encoding='ascii')
    rejected = subprocess.run(base + ['/Di386', str(probe)], capture_output=True,
                              text=True, errors='replace')
    if not rejected.returncode or 'Excluded unsupported historical i386' not in rejected.stdout + rejected.stderr:
        raise SystemExit(f'{header}: unsupported historical i386 profile did not fail')
    probe.write_text(group + 'int after_outer = __LINE__;\n', encoding='ascii')
    line = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
    if line.returncode or not re.search(r'int\s+after_outer\s*=\s*' + str(continuation) + r'\s*;', line.stdout):
        raise SystemExit(f'{header}: original continuation line changed')
    first_line, other_line = ((74, 79) if header == 'mvdm.h' else (33, 35))
    occurrences = [(first_line, 1), (other_line, 0)]
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

trycall_marker = '#error Excluded unsupported NO_W32TRYCALL WOW profile'
for path, following, selected_line in (
    ('src/mvdm-host/wow32/wkman.c', 1179, None),
    ('src/mvdm-host/wow32/wkman.c', 1192, None),
    ('src/mvdm-host/wow32/wow32.c', 1152, 1125),
):
    source = (repo / path).read_text(encoding='latin1').split('\n')
    start, cut, end = find_guard(source, r'^\s*#\s*if\s+NO_W32TRYCALL\b',
                                 trycall_marker, following, selected_line, path)
    snippet = ['#if NO_W32TRYCALL', trycall_marker]
    if selected_line is not None:
        snippet += ['#else', source[cut + 1].strip(), 'int selected_line = __LINE__;']
    snippet += ['#endif', source[end + 1].strip(), 'int following_line = __LINE__;']
    probe.write_text('\n'.join(snippet) + '\n', encoding='ascii')
    selected = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(base + ['/DNO_W32TRYCALL=1', str(probe)],
                              capture_output=True, text=True, errors='replace')
    if (selected.returncode or not re.search(r'int\s+following_line\s*=\s*' + str(following) + r'\s*;', selected.stdout)
            or (selected_line is not None and not re.search(r'int\s+selected_line\s*=\s*' + str(selected_line) + r'\s*;', selected.stdout))):
        raise SystemExit('WOW try-call selected mapping failed: ' + path + selected.stderr)
    if not rejected.returncode or trycall_marker[7:] not in rejected.stdout + rejected.stderr:
        raise SystemExit('unsupported WOW try-call profile did not fail: ' + path)
print('PASS three scoped WOW try-call exclusions and original line mappings')

wdib_marker = '#error Excluded unsupported i386 WOW DIB profile'
wdib_lines = (repo / 'src/mvdm-host/wow32/wdib.c').read_text(encoding='latin1').split('\n')
for following in (322, 667):
    matches = []
    for i, line in enumerate(wdib_lines):
        if line.strip() != '#else':
            continue
        error = guard_error_index(wdib_lines, i)
        if (error + 2 < len(wdib_lines) and wdib_lines[error].strip() == wdib_marker
                and wdib_lines[error + 1].strip() == '#endif'
                and wdib_lines[error + 2].strip() == '#line ' + str(following)):
            matches.append(error)
    if len(matches) != 1:
        raise SystemExit('WOW DIB guard or line mapping missing')
    end = matches[0] + 1
    snippet = ['#ifndef i386', 'int selected = 1;', '#else', wdib_marker,
               '#endif', wdib_lines[end + 1].strip(), 'int following_line = __LINE__;']
    probe.write_text('\n'.join(snippet) + '\n', encoding='ascii')
    selected = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(base + ['/Di386', str(probe)], capture_output=True, text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+following_line\s*=\s*' + str(following) + r'\s*;', selected.stdout):
        raise SystemExit('WOW DIB selected mapping failed: ' + selected.stderr)
    if not rejected.returncode or wdib_marker[7:] not in rejected.stdout + rejected.stderr:
        raise SystemExit('unsupported WOW DIB i386 profile did not fail')
print('PASS scoped WOW DIB exclusions and original continuation lines')

header_marker = '#error Excluded unsupported i386 WOW header profile'
for path, continuation, has_else in (
    ('src/mvdm-host/inc/wow.h', 40, False),
    ('src/mvdm-host/wow32/wspool.h', 64, True),
):
    lines = (repo / path).read_text(encoding='latin1').splitlines()
    starts = [i for i, line in enumerate(lines) if line.strip() == '#ifdef i386'
              and guard_error_index(lines, i) < len(lines)
              and lines[guard_error_index(lines, i)].strip() == header_marker]
    if len(starts) != 1:
        raise SystemExit('WOW i386 header guard missing: ' + path)
    start = starts[0]
    error_index = guard_error_index(lines, start)
    if has_else:
        if [x.strip() for x in lines[error_index + 1:error_index + 5]] != ['#else', '#line 62', '#define SafeLoadLibrary(name) LoadLibrary(name)', '#endif']:
            raise SystemExit('WOW spool selected alternative changed')
        snippet = ['#ifdef i386', header_marker, '#else', 'int selected = 1;', '#endif', '#line 64', 'int following_line = __LINE__;']
    else:
        if [x.strip() for x in lines[error_index + 1:error_index + 3]] != ['#endif', '#line 40']:
            raise SystemExit('WOW header original continuation changed')
        snippet = ['#ifdef i386', header_marker, '#endif', lines[error_index + 2].strip(), 'int following_line = __LINE__;']
    probe.write_text('\n'.join(snippet) + '\n', encoding='ascii')
    good = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
    bad = subprocess.run(base + ['/Di386', str(probe)], capture_output=True, text=True, errors='replace')
    if good.returncode or not re.search(r'int\s+following_line\s*=\s*' + str(continuation) + r'\s*;', good.stdout):
        raise SystemExit('WOW header selected or original-line control failed: ' + path)
    if not bad.returncode or header_marker[7:] not in bad.stdout + bad.stderr:
        raise SystemExit('WOW header i386 rejection failed: ' + path)
print('PASS two scoped WOW i386 header guards and original continuation lines')

dpmi_header = (repo / 'src/mvdm-host/dpmi32/dpmiint.h').read_text(encoding='latin1')
outer, tail = dpmi_header.split('#ifndef i386\n', 1)
body, tail = tail.split('\n#else\n', 1)
rejected, tail = tail.split('\n#endif // i386\n#line 46\n', 1)
if not body.startswith('#line 24\n') or '#error Excluded unsupported i386 kernel VDM handler carrier' not in rejected:
    raise SystemExit('DPMI original outer selector or software carrier differs')
probe.write_text('#ifndef i386\n' + body + '\n#else\n' + rejected +
                 '\n#endif\n#line 46\nint after_dpmi = __LINE__;\n', encoding='ascii')
for defines in ([], ['/DCPU_40_STYLE']):
    selected = subprocess.run(base + defines + [str(probe)], capture_output=True, text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+after_dpmi\s*=\s*46\s*;', selected.stdout):
        raise SystemExit('DPMI selected selector or original line failed: ' + selected.stderr)
excluded = subprocess.run(base + ['/Di386', str(probe)], capture_output=True, text=True, errors='replace')
if not excluded.returncode or 'Excluded unsupported i386 kernel VDM handler carrier' not in excluded.stdout + excluded.stderr:
    raise SystemExit('DPMI unsupported hardware-monitor profile did not fail')
print('PASS original DPMI outer condition, software carrier and continuation')

hook_lines = (repo / 'src/mvdm-host/dpmi32/dpmiint.c').read_text(encoding='latin1').splitlines()
for field, original_body_line, continuation in (('fDpmiHookInts', 1168, 1175), ('fDpmiIntsHaveBeenHooked', 1202, 1209)):
    marker = '    if (' + field + ') {'
    instances = [i for i, line in enumerate(hook_lines) if line == marker]
    if len(instances) != 1:
        raise SystemExit('DPMI hook carrier count differs: ' + field)
    first = instances[0]
    if (hook_lines[first - 2].strip() != '#ifndef i386' or
            hook_lines[first - 1].strip() != '#line ' + str(original_body_line)):
        raise SystemExit('DPMI hook selector differs: ' + field)
    cut = next(i for i in range(first, len(hook_lines)) if hook_lines[i].strip() == '#else')
    end = next(i for i in range(cut, len(hook_lines)) if hook_lines[i].strip() == '#endif // i386')
    error = guard_error_index(hook_lines, cut)
    if (hook_lines[error].strip() != '#error Excluded unsupported i386 kernel VDM interrupt hook path'
            or end != error + 1 or hook_lines[end + 1].strip() != '#line ' + str(continuation)):
        raise SystemExit('DPMI hook rejection or continuation differs: ' + field)
    snippet = hook_lines[first - 2:end + 2] + ['int after_hook = __LINE__;']
    probe.write_text('\n'.join(snippet) + '\n', encoding='ascii')
    for defines in ([], ['/DCPU_40_STYLE']):
        result = subprocess.run(base + defines + [str(probe)], capture_output=True, text=True, errors='replace')
        if result.returncode or not re.search(r'int\s+after_hook\s*=\s*' + str(continuation) + r'\s*;', result.stdout):
            raise SystemExit('DPMI hook selected profile failed: ' + field + result.stderr)
    rejected = subprocess.run(base + ['/Di386', str(probe)], capture_output=True, text=True, errors='replace')
    if not rejected.returncode or 'Excluded unsupported i386 kernel VDM interrupt hook path' not in rejected.stdout + rejected.stderr:
        raise SystemExit('DPMI hardware-monitor hook was not rejected: ' + field)
print('PASS original DPMI hook selectors, software profile and hardware rejection')

host_cpu = (repo / 'src/mvdm-host/softpc.new/host/inc/host_cpu.h').read_text(encoding='latin1')
if '#ifdef CPU_30_STYLE\n#line 18\n#error Excluded original conditional branch\n#else /* CPU_30_STYLE */\n#line 113\n' not in host_cpu or not host_cpu.rstrip().endswith('#line 507'):
    raise SystemExit('host_cpu original outer selector or line mapping missing')
probe.write_text(host_cpu + 'int after_host_cpu = __LINE__;\n', encoding='ascii')
selected = subprocess.run(base + ['/DLITTLEND', str(probe)], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base + ['/DLITTLEND', '/DCPU_30_STYLE', str(probe)], capture_output=True, text=True, errors='replace')
if selected.returncode or 'typedef union' not in selected.stdout or not re.search(r'int\s+after_host_cpu\s*=\s*507\s*;', selected.stdout):
    raise SystemExit('host_cpu CPU40 selected carrier or original line failed: ' + selected.stderr)
if not rejected.returncode or 'Excluded original conditional branch' not in rejected.stdout + rejected.stderr:
    raise SystemExit('host_cpu CPU30 selection did not reject')
print('PASS host_cpu original CPU30 outer and selected CPU40 union')

ios_lines = (repo / 'src/mvdm-host/softpc.new/base/support/ios.c').read_text(encoding='latin1').splitlines()
ios_marker = '#error Excluded unsupported monitor IOS carrier'
indices = [i for i, line in enumerate(ios_lines) if line.strip() == ios_marker]
if len(indices) != 1:
    raise SystemExit('IOS original monitor guard missing')
i = indices[0]
guard_start = i - 1
while guard_start >= 0 and re.match(r'^\s*#\s*line\b', ios_lines[guard_start]):
    guard_start -= 1
if ([ios_lines[guard_start].strip()] + [x.strip() for x in ios_lines[i:i + 4]] !=
        ['#if defined(NTVDM) && defined(MONITOR)', ios_marker, '#else', '#if defined(NTVDM) && defined(CCPU)', '#line 414']):
    raise SystemExit('IOS outer condition or CCPU alternative changed')
end = next(j for j in range(i + 4, len(ios_lines)) if ios_lines[j].strip() == '#endif')
if ios_lines[end + 1].strip() != '#endif /* NTVDM & MONITOR */' or ios_lines[end + 2].strip() != '#line 444':
    raise SystemExit('IOS original continuation changed')
body = ios_lines[i + 4:end]
snippet = ['#if defined(NTVDM) && defined(MONITOR)', ios_marker, '#else', '#if defined(NTVDM) && defined(CCPU)', '#line 414'] + body + ['#endif', '#endif', '#line 444', 'int after_ios = __LINE__;']
probe.write_text('\n'.join(snippet) + '\n', encoding='ascii')
for defines, carrier in ((['/DNTVDM', '/DCCPU'], True), (['/DNTVDM'], False), (['/DCCPU'], False)):
    result = subprocess.run(base + defines + [str(probe)], capture_output=True, text=True, errors='replace')
    if result.returncode or ('VOID insb' in result.stdout) != carrier or not re.search(r'int\s+after_ios\s*=\s*444\s*;', result.stdout):
        raise SystemExit('IOS supported selection or line failed: ' + result.stderr)
rejected = subprocess.run(base + ['/DNTVDM', '/DMONITOR', str(probe)], capture_output=True, text=True, errors='replace')
if not rejected.returncode or ios_marker[7:] not in rejected.stdout + rejected.stderr:
    raise SystemExit('IOS monitor profile did not reject')
print('PASS IOS original MONITOR outer and preserved CCPU selection')

for path, macro, continuation in (
    ('src/mvdm-host/wow32/wmsgbm.h', 'BM_CLICK', None),
    ('src/mvdm-host/inc/wowmmed.h', '_INC_MMSYSTEM', 27),
):
    source = (repo / path).read_text(encoding='latin1').splitlines()
    marker = '#error Excluded unsupported missing ' + macro + ' SDK profile'
    matches = [i for i, line in enumerate(source) if line.strip() == '#ifndef ' + macro
               and guard_error_index(source, i) + 1 < len(source)
               and source[guard_error_index(source, i)].strip() == marker
               and source[guard_error_index(source, i) + 1].strip() == '#endif']
    if len(matches) != 1:
        raise SystemExit('WOW SDK fallback guard missing: ' + path)
    i = matches[0]
    error_index = guard_error_index(source, i)
    snippet = source[i:error_index + 2]
    if continuation is not None:
        if source[error_index + 2].strip() != '#line ' + str(continuation):
            raise SystemExit('WOW SDK fallback original continuation missing: ' + path)
        snippet += [source[error_index + 2], 'int after_fallback = __LINE__;']
    probe.write_text('\n'.join(snippet) + '\n', encoding='ascii')
    selected = subprocess.run(base + ['/D' + macro, str(probe)], capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(base + ['/U' + macro, str(probe)], capture_output=True, text=True, errors='replace')
    if selected.returncode or (continuation is not None and not re.search(r'int\s+after_fallback\s*=\s*' + str(continuation) + r'\s*;', selected.stdout)):
        raise SystemExit('WOW SDK provider or original line check failed: ' + path + selected.stderr)
    if not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
        raise SystemExit('WOW missing SDK provider did not reject: ' + path)
print('PASS scoped WOW SDK fallbacks and original continuation')

for path, macro, original_line in (
    ('src/mvdm-host/inc/mvdm.h', 'MAKEWORD', 68),
    ('src/mvdm-host/inc/wow.h', 'WM_USER', 1212),
):
    source = (repo / path).read_text(encoding='latin1').splitlines()
    marker = '#error Excluded unsupported missing ' + macro + ' SDK profile'
    matches = [(i, guard_error_index(source, i)) for i, line in enumerate(source)
               if line.strip() == '#ifndef ' + macro
               and source[guard_error_index(source, i)].strip() == marker
               and source[guard_error_index(source, i) + 1].strip() == '#endif']
    if len(matches) != 1:
        raise SystemExit('SDK provider guard missing: ' + path)
    i, error = matches[0]
    preceding = [(j, re.match(r'^\s*#\s*line\s+(\d+)', line)) for j, line in enumerate(source[:i])]
    j, line_directive = [(j, match) for j, match in preceding if match][-1]
    if int(line_directive.group(1)) + (i - j - 1) != original_line:
        raise SystemExit('SDK guard original logical line changed: ' + path)
    probe.write_text('\n'.join(['#line ' + str(original_line)] + source[i:error + 3] + ['int after_sdk = __LINE__;']) + '\n', encoding='ascii')
    selected = subprocess.run(base + ['/D' + macro, str(probe)], capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(base + ['/U' + macro, str(probe)], capture_output=True, text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+after_sdk\s*=\s*' + str(original_line + 3) + r'\s*;', selected.stdout):
        raise SystemExit('SDK selected guard or original continuation failed: ' + path)
    if not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
        raise SystemExit('missing SDK provider did not reject: ' + path)
print('PASS MAKEWORD and WM_USER SDK guards and original lines')

for macro, original_line in (('SUCCESS', 26), ('NULL', 60)):
    source = (repo / 'src/mvdm-host/softpc.new/base/inc/cntlbop.h').read_text(encoding='latin1').splitlines()
    marker = '#error Excluded unsupported missing ' + macro + ' control-BOP profile'
    matches = [i for i, line in enumerate(source) if line.strip() == '#ifndef ' + macro
               and guard_error_index(source, i) + 1 < len(source)
               and source[guard_error_index(source, i)].strip() == marker
               and source[guard_error_index(source, i) + 1].strip() == '#endif']
    if len(matches) != 1:
        raise SystemExit('control-BOP fallback guard or original line missing: ' + macro)
    start = matches[0]
    error_index = guard_error_index(source, start)
    if start > 0 and source[start - 1].strip().startswith('#line ') and source[start - 1].strip() != '#line ' + str(original_line):
        raise SystemExit('control-BOP original guard line changed: ' + macro)
    snippet = ['#line ' + str(original_line)] + source[start:error_index + 2]
    following = original_line + (error_index + 2 - start)
    if error_index + 2 < len(source) and source[error_index + 2].strip().startswith('#line '):
        snippet.append(source[error_index + 2])
        following = int(source[error_index + 2].strip().split()[1])
    probe.write_text('\n'.join(snippet + ['int after_bop = __LINE__;']) + '\n', encoding='ascii')
    selected = subprocess.run(base + ['/D' + macro, str(probe)], capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(base + ['/U' + macro, str(probe)], capture_output=True, text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+after_bop\s*=\s*' + str(following) + r'\s*;', selected.stdout):
        raise SystemExit('control-BOP provider or original line failed: ' + macro)
    if not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
        raise SystemExit('missing control-BOP provider did not reject: ' + macro)
print('PASS control-BOP fallback guards and original lines')

for macro, original_line in (('TRUE', 46), ('FALSE', 50)):
    source = (repo / 'src/mvdm-host/softpc.new/base/inc/base_def.h').read_text(encoding='latin1').splitlines()
    marker = '#error Excluded unsupported missing ' + macro + ' base profile'
    matches = [i for i, line in enumerate(source) if line.strip() == '#ifndef ' + macro
               and guard_error_index(source, i) + 1 < len(source)
               and source[guard_error_index(source, i)].strip() == marker
               and source[guard_error_index(source, i) + 1].strip() == '#endif']
    if len(matches) != 1:
        raise SystemExit('base Boolean fallback guard or original line missing: ' + macro)
    start = matches[0]
    error_index = guard_error_index(source, start)
    if start > 0 and source[start - 1].strip().startswith('#line ') and source[start - 1].strip() != '#line ' + str(original_line):
        raise SystemExit('base Boolean original guard line changed: ' + macro)
    snippet = ['#line ' + str(original_line)] + source[start:error_index + 2]
    following = original_line + (error_index + 2 - start)
    if error_index + 2 < len(source) and source[error_index + 2].strip().startswith('#line '):
        snippet.append(source[error_index + 2])
        following = int(source[error_index + 2].strip().split()[1])
    probe.write_text('\n'.join(snippet + ['int after_bool = __LINE__;']) + '\n', encoding='ascii')
    if following != original_line + 3:
        raise SystemExit('base Boolean original continuation changed: ' + macro)
    selected = subprocess.run(base + ['/D' + macro, str(probe)], capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(base + ['/U' + macro, str(probe)], capture_output=True, text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+after_bool\s*=\s*' + str(following) + r'\s*;', selected.stdout):
        raise SystemExit('base Boolean provider or original line failed: ' + macro)
    if not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
        raise SystemExit('missing base Boolean provider did not reject: ' + macro)
print('PASS base Boolean fallback guards and original lines')

source = (repo / 'src/non-mvdm/private/inc/dlcio.h').read_text(encoding='latin1').splitlines()
marker = '#error Excluded unsupported missing MAX_PATH SDK profile'
matches = [i for i, line in enumerate(source) if line.startswith('#ifndef    MAX_PATH')
           and i > 0 and source[i - 1].strip() == '#line 470'
           and source[guard_error_index(source, i)].strip() == marker
           and source[guard_error_index(source, i) + 1].strip() == '#endif'
           and source[guard_error_index(source, i) + 2].strip() == '#line 473']
if len(matches) != 1:
    raise SystemExit('DLCIO MAX_PATH guard or original line missing')
i = matches[0]
end = guard_error_index(source, i) + 3
probe.write_text('\n'.join(source[i - 1:end] + ['int after_dlcio = __LINE__;']) + '\n', encoding='ascii')
selected = subprocess.run(base + ['/DMAX_PATH=260', str(probe)], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base + ['/UMAX_PATH', str(probe)], capture_output=True, text=True, errors='replace')
if selected.returncode or not re.search(r'int\s+after_dlcio\s*=\s*473\s*;', selected.stdout):
    raise SystemExit('DLCIO selected provider or line failed: ' + selected.stderr)
if not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
    raise SystemExit('DLCIO missing provider did not reject')
print('PASS DLCIO SDK MAX_PATH guard and original lines')

source = (repo / 'src/non-mvdm/windows/core/ntuser/kernel/server.c').read_text(encoding='latin1').splitlines()
marker = '#error Excluded unsupported FE_IME server global'
matches = [i for i, line in enumerate(source) if line.strip() == '#ifdef FE_IME'
           and source[guard_error_index(source, i)].strip() == marker
           and source[guard_error_index(source, i)+1].strip() == '#endif']
if len(matches) != 1:
    raise SystemExit('restored original FE_IME server guard missing')
i = matches[0]
j = guard_error_index(source, i)
line190 = source.index('#line 190', j + 2)
selector = source.index('#if defined(WOW_ORIGINAL_DEFAULT_MESSAGES)')
if (source[i-1].strip() != '#line 53' or selector <= i or line190 <= selector or
        not source[line190+2].startswith('CONST WORD gawButtonWndProc')):
    raise SystemExit('restored server original line anchors changed')
probe.write_text('\n'.join(source[i-1:j+2] + ['#line 190',
                      '/* selected original table */', 'int retained_line = __LINE__;']) + '\n', encoding='ascii')
selected = subprocess.run(base + ['/UFE_IME', str(probe)], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base + ['/DFE_IME', str(probe)], capture_output=True, text=True, errors='replace')
if selected.returncode or not re.search(r'int\s+retained_line\s*=\s*191\s*;', selected.stdout):
    raise SystemExit('server selected FE_IME guard or line failed: ' + selected.stderr)
if not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
    raise SystemExit('unsupported FE_IME server global did not reject')
print('PASS restored server FE_IME guard and original line')

source = (repo / 'src/non-mvdm/windows/core/ntuser/kernel/server.c').read_text(encoding='latin1').splitlines()
marker = '#error DIVERGENCE OPENNT-HOST-132: FE_IME branch is not selected; see mirror register.'
i = unique_sequence(source, ['#line 583', '#ifdef FE_IME', '#line 584', marker,
                             '#endif', '#if defined(WOW_ORIGINAL_DEFAULT_MESSAGES)',
                             '#line 597', '0'], 'server message-table FE_IME guard')
depth = 0
for line in source[:i + 1]:
    directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
    if directive:
        depth += -1 if directive.group(1) == 'endif' else 1
if depth:
    raise SystemExit('original message-table FE_IME guard is not first-depth')
probe.write_text('\n'.join(source[i:i + 5] + ['int after_server = __LINE__;']) + '\n', encoding='ascii')
allowed = subprocess.run(base + ['/UFE_IME', str(probe)], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base + ['/DFE_IME', str(probe)], capture_output=True, text=True, errors='replace')
if allowed.returncode or not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
    raise SystemExit('message-table FE_IME exclusion control failed')
print('PASS original server message-table FE_IME first-depth exclusion')

source = (repo / 'src/non-mvdm/windows/core/ntuser/kernel/input.c').read_text(encoding='latin1').splitlines()
marker = '#error Excluded original disabled input branch'
i = unique_sequence(source, ['#line 1409', '#if 0', '#line 1410', marker,
                             '#endif', '#line 1431'],
                    'original disabled input condition')
depth = 0
for line in source[:i + 1]:
    directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
    if directive:
        depth += -1 if directive.group(1) == 'endif' else 1
if depth:
    raise SystemExit('original disabled input condition is not first-depth')
probe.write_text('\n'.join(source[i:i + 6] + ['int input_after = __LINE__;']) + '\n', encoding='ascii')
allowed = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
probe.write_text('\n'.join(source[i:i + 6]).replace('#if 0', '#if 1', 1) + '\n', encoding='ascii')
rejected = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
if (allowed.returncode or not re.search(r'int\s+input_after\s*=\s*1431\s*;', allowed.stdout)
        or not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr
        or '(1410)' not in rejected.stdout + rejected.stderr):
    raise SystemExit('original disabled input branch/line controls failed')
print('PASS original input literal-zero first-depth exclusion and lines')

marker = '#error Excluded unsupported original NEVER input branch'
i = unique_sequence(source, ['#line 2295', '#ifdef NEVER', '#line 2296', marker,
                             '#endif', '#line 2310'], 'original input NEVER condition')
depth = 0
for line in source[:i + 1]:
    directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
    if directive:
        depth += -1 if directive.group(1) == 'endif' else 1
if depth:
    raise SystemExit('original input NEVER condition is not first-depth')
probe.write_text('\n'.join(source[i:i + 6] + ['int never_after = __LINE__;']) + '\n', encoding='ascii')
allowed = subprocess.run(base + ['/UNEVER', str(probe)], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base + ['/DNEVER=1', str(probe)], capture_output=True, text=True, errors='replace')
if (allowed.returncode or not re.search(r'int\s+never_after\s*=\s*2310\s*;', allowed.stdout)
        or not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr
        or '(2296)' not in rejected.stdout + rejected.stderr):
    raise SystemExit('original input NEVER branch/line controls failed')
print('PASS original input NEVER first-depth exclusion and lines')

source = (repo / 'src/non-mvdm/windows/core/ntuser/client/clres.c').read_text(encoding='latin1').splitlines()
for original, continuation in ((1480, 1484), (2298, 2302), (2624, 2629),
                               (3751, 3763), (4516, 4520), (4574, 4578)):
    matches = [i for i, line in enumerate(source) if line.strip() == f'#line {original}']
    if len(matches) != 1:
        raise SystemExit(f'clres original disabled boundary {original} is not unique')
    i = matches[0]
    if (not re.match(r'^\s*#\s*if\s+0\b', source[i + 1])
            or source[i + 2].strip() != f'#line {original + 1}'
            or not source[i + 3].strip().startswith('#error ')):
        raise SystemExit(f'clres original disabled boundary {original} is malformed')
    end = next(j for j in range(i + 4, len(source)) if source[j].strip() == '#endif')
    if source[end + 1].strip() != f'#line {continuation}':
        raise SystemExit(f'clres original continuation {continuation} missing')
    depth = 0
    for line in source[:i + 1]:
        directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
        if directive:
            depth += -1 if directive.group(1) == 'endif' else 1
    if depth:
        raise SystemExit(f'clres original disabled boundary {original} is not first-depth')
    snippet = source[i:end + 2]
    probe.write_text('\n'.join(snippet + ['int clres_after = __LINE__;']) + '\n', encoding='ascii')
    allowed = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
    snippet[1] = snippet[1].replace('#if 0', '#if 1', 1)
    probe.write_text('\n'.join(snippet) + '\n', encoding='ascii')
    rejected = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
    if (allowed.returncode or not re.search(rf'int\s+clres_after\s*=\s*{continuation}\s*;', allowed.stdout)
            or not rejected.returncode or f'({original + 1})' not in rejected.stdout + rejected.stderr):
        raise SystemExit(f'clres disabled boundary {original} controls failed')
print('PASS six original clres literal-zero first-depth exclusions and lines')

source = (repo / 'src/non-mvdm/base/ntos/ex/probe.c').read_text(encoding='latin1').splitlines()
marker = '#error Excluded unsupported original Alpha probe branch'
i = unique_sequence(source, ['#line 106', '#if defined(_ALPHA_)', '#line 107', marker,
                             '#else', '#line 122', '#endif', '#line 129'],
                    'original Alpha probe condition')
depth = 0
for line in source[:i + 1]:
    directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
    if directive:
        depth += -1 if directive.group(1) == 'endif' else 1
if depth:
    raise SystemExit('original Alpha probe condition is not first-depth')
probe.write_text('\n'.join(source[i:i + 8] + ['int alpha_after = __LINE__;']) + '\n', encoding='ascii')
allowed = subprocess.run(base + ['/U_ALPHA_', str(probe)], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base + ['/D_ALPHA_=1', str(probe)], capture_output=True, text=True, errors='replace')
if (allowed.returncode or not re.search(r'int\s+alpha_after\s*=\s*129\s*;', allowed.stdout)
        or not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr
        or '(107)' not in rejected.stdout + rejected.stderr):
    raise SystemExit('original Alpha probe condition controls failed')
print('PASS original Alpha probe first-depth exclusion and lines')

source = (repo / 'src/non-mvdm/windows/core/ntuser/client/ntstubs.c').read_text(encoding='latin1').splitlines()
marker = '#error Excluded unsupported FE_SB client message owner'
matches = [i for i, line in enumerate(source) if line.strip() == '#ifdef FE_SB // fnINWPARAMDBCSCHAR()'
           and source[guard_error_index(source, i)].strip() == marker
           and source[guard_error_index(source, i)+1].strip() == '#endif // FE_SB']
if len(matches) != 1 or source[matches[0]-1].strip() != '#line 1611':
    raise SystemExit('restored original ntstubs FE_SB guard or line missing')
i = matches[0]
j = guard_error_index(source, i)
if source[j+2].strip() != '#line 1646':
    raise SystemExit('ntstubs FE_SB original continuation missing')
probe.write_text('\n'.join(source[i-1:j+3] + ['int after_ntstubs_guard = __LINE__;']) + '\n', encoding='ascii')
selected = subprocess.run(base + ['/UFE_SB', str(probe)], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base + ['/DFE_SB', str(probe)], capture_output=True, text=True, errors='replace')
if selected.returncode or not re.search(r'int\s+after_ntstubs_guard\s*=\s*1646\s*;', selected.stdout):
    raise SystemExit('ntstubs FE_SB selected guard or line failed: ' + selected.stderr)
if not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
    raise SystemExit('unsupported FE_SB client message owner did not reject')
print('PASS restored ntstubs FE_SB guard and original line')

source = (repo / 'src/non-mvdm/windows/core/ntuser/client/ntstubs.c').read_text(encoding='latin1').splitlines()
for macro, original_line in (('MEMPHIS_MENUS', 380), ('MEMPHIS_MENU_WATERMARKS', 500)):
    marker = '#error Excluded unsupported ' + macro + ' client owner'
    if macro == 'MEMPHIS_MENUS':
        expected = ['#line 380', '#ifdef MEMPHIS_MENUS', marker,
                    '#else // MEMPHIS_MENUS', '#line 440',
                    '#endif // MEMPHIS_MENUS', '#line 500']
        unique_sequence(source, expected, 'ntstubs Memphis menu boundary')
        probe.write_text('\n'.join(expected[:5] + ['int selected_menu_line = __LINE__;'] +
                                   expected[5:] + ['int following_menu_line = __LINE__;']) + '\n', encoding='ascii')
        selected = subprocess.run(base + ['/U' + macro, str(probe)], capture_output=True, text=True, errors='replace')
        rejected = subprocess.run(base + ['/D' + macro, str(probe)], capture_output=True, text=True, errors='replace')
        if (selected.returncode or not re.search(r'int\s+selected_menu_line\s*=\s*440\s*;', selected.stdout)
                or not re.search(r'int\s+following_menu_line\s*=\s*500\s*;', selected.stdout)
                or not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr):
            raise SystemExit('ntstubs Memphis menu boundary control failed')
        continue
    matches = [i for i, line in enumerate(source) if line.strip() == '#ifdef ' + macro
               and source[guard_error_index(source, i)].strip() == marker
               and source[guard_error_index(source, i)+1].strip() == '#endif // ' + macro]
    if len(matches) != 1 or source[matches[0]-1].strip() != '#line ' + str(original_line):
        raise SystemExit('restored ntstubs menu guard or line missing: ' + macro)
    i = matches[0]
    j = guard_error_index(source, i)
    if source[j+2].strip() != '#line 523':
        raise SystemExit('restored ntstubs menu continuation missing')
    probe.write_text('\n'.join(source[i-1:j+3] + ['int after_menu_guard = __LINE__;']) + '\n', encoding='ascii')
    selected = subprocess.run(base + ['/U' + macro, str(probe)], capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(base + ['/D' + macro, str(probe)], capture_output=True, text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+after_menu_guard\s*=\s*523\s*;', selected.stdout):
        raise SystemExit('ntstubs menu selected guard or line failed: ' + macro + selected.stderr)
    if not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
        raise SystemExit('unsupported ntstubs menu owner did not reject: ' + macro)
print('PASS restored ntstubs menu guards and original lines')

source = (repo / 'src/non-mvdm/windows/core/ntuser/client/usercli.h').read_text(encoding='latin1').splitlines()
marker = '#error Excluded unsupported FE_SB client messaging macros'
matches = [i for i, line in enumerate(source) if line.strip() == '#ifdef FE_SB // DBCS Messaging macros for client side.' and source[i+1].strip() == marker and source[i+2].strip() == '#endif // FE_SB']
if len(matches) != 1 or source[matches[0]-1].strip() != '#line 2374':
    raise SystemExit('restored usercli FE_SB guard or original line missing')
i = matches[0]
probe.write_text('\n'.join(source[i-1:i+3] + ['int after_usercli_guard = __LINE__;']) + '\n', encoding='ascii')
selected = subprocess.run(base + ['/UFE_SB', str(probe)], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base + ['/DFE_SB', str(probe)], capture_output=True, text=True, errors='replace')
if selected.returncode or not re.search(r'int\s+after_usercli_guard\s*=\s*2377\s*;', selected.stdout):
    raise SystemExit('usercli FE_SB selected guard or line failed: ' + selected.stderr)
if not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
    raise SystemExit('unsupported FE_SB client messaging macros did not reject')
print('PASS restored usercli FE_SB guard and original line')

source = (repo / 'src/non-mvdm/windows/core/ntuser/kernel/server.c').read_text(encoding='latin1').splitlines()
matches = [i for i, line in enumerate(source) if line.strip() == '#ifndef PAGE_SIZE' and source[i+1].strip() == '#define PAGE_SIZE 0x1000' and source[i+2].strip() == '#endif']
if len(matches) != 1 or source[matches[0]-1].strip() != '#line 1257':
    raise SystemExit('original server PAGE_SIZE fallback or line missing')
i = matches[0]
probe.write_text('\n'.join(source[i-1:i+3] + ['int page_value = PAGE_SIZE;', 'int after_page_guard = __LINE__;']) + '\n', encoding='ascii')
default = subprocess.run(base + ['/UPAGE_SIZE', str(probe)], capture_output=True, text=True, errors='replace')
provided = subprocess.run(base + ['/DPAGE_SIZE=8192', str(probe)], capture_output=True, text=True, errors='replace')
if default.returncode or provided.returncode or not re.search(r'int\s+page_value\s*=\s*0x1000\s*;', default.stdout) or not re.search(r'int\s+page_value\s*=\s*8192\s*;', provided.stdout) or not re.search(r'int\s+after_page_guard\s*=\s*1261\s*;', default.stdout):
    raise SystemExit('server PAGE_SIZE selected or provided branch failed')
print('PASS original server PAGE_SIZE fallback and original line')

source = (repo / 'src/non-mvdm/windows/core/ntuser/client/usercli.h').read_text(encoding='latin1').splitlines()
for macro, name, original_line, selected_line, end_line in (('DBG', 'HANDLE_WARNING', 2307, 2317, 2318), ('DBG', 'CHECK_HANDLE_WARNING', 2320, 2337, 2338), ('HANDLE_FIXUP', 'FIXUP_HANDLE', 2342, 2352, 2356), ('HANDLE_FIXUP', 'FIXUP_HANDLEZ', 2358, 2368, 2372)):
    marker = '#error Excluded unsupported ' + macro + ' ' + name + ' profile'
    matches = [i for i, line in enumerate(source) if line.strip() == marker]
    if len(matches) != 1:
        raise SystemExit('original fixup guard missing: ' + name)
    i = matches[0]; end = next(j for j in range(i+1, len(source)) if source[j].strip() == '#endif')
    start = i - 1
    while start >= 0 and re.match(r'^\s*#\s*line\b', source[start]):
        start -= 1
    if source[start-1].strip() != '#line ' + str(original_line) or source[start].strip() != '#if ' + macro or source[i+1].strip() != '#else' or source[i+2].strip() != '#line ' + str(selected_line):
        raise SystemExit('original fixup selector/selected line missing: ' + name)
    probe.write_text('\n'.join(source[start-1:end+1] + ['int after_fixup_guard = __LINE__;']) + '\n', encoding='ascii')
    selected = subprocess.run(base + ['/U' + macro, str(probe)], capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(base + ['/D' + macro + '=1', str(probe)], capture_output=True, text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+after_fixup_guard\s*=\s*' + str(end_line+1) + r'\s*;', selected.stdout):
        raise SystemExit('fixup retail selection or original line failed: ' + name + selected.stderr)
    if not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr or ('(' + str(original_line+1) + ')') not in rejected.stdout + rejected.stderr:
        raise SystemExit('unsupported fixup profile or error line not rejected: ' + name)
print('PASS original retail fixup guards and logical lines')

source = (repo / 'src/non-mvdm/windows/core/ntuser/kernel/cleanup.c').read_text(encoding='latin1').splitlines()
marker = '#error Excluded unsupported non-x86 WOW cleanup probe alignment'
matches = [i for i, line in enumerate(source) if line.strip() == marker]
if len(matches) != 1:
    raise SystemExit('cleanup x86 guard missing')
i = matches[0]
cut = i - 1
while cut >= 0 and re.match(r'^\s*#\s*line\b', source[cut]):
    cut -= 1
start = cut - 3
if source[start].strip() != '#line 229' or source[start+1].strip() != '#if defined(_X86_)' or source[cut].strip() != '#else' or source[i+1].strip() != '#endif' or source[i+2].strip() != '#line 234':
    raise SystemExit('cleanup original selector or line anchors missing')
probe.write_text('\n'.join(source[start:i+3] + ['int cleanup_after = __LINE__;']) + '\n', encoding='ascii')
selected = subprocess.run(base + ['/D_X86_=1', str(probe)], capture_output=True, text=True, errors='replace')
rejected = subprocess.run(base + ['/U_X86_', str(probe)], capture_output=True, text=True, errors='replace')
if selected.returncode or 'sizeof(BYTE)' not in selected.stdout or not re.search(r'int\s+cleanup_after\s*=\s*234\s*;', selected.stdout):
    raise SystemExit('cleanup x86 selected branch or line failed')
if not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr or '(232)' not in rejected.stdout + rejected.stderr:
    raise SystemExit('cleanup non-x86 branch or original error line not rejected')
print('PASS original cleanup x86 branch and lines')

source = (repo / 'src/non-mvdm/windows/core/ntuser/client/mngrayc.c').read_text(encoding='latin1').splitlines()
marker = '#error DIVERGENCE OPENNT-HOST-128: original literal #if 0 excludes this legacy branch; see mirror register.'
for original, continuation in ((209, 218), (251, 258)):
    matches = [i for i, line in enumerate(source) if line.strip() == '#line ' + str(original)]
    if len(matches) != 1:
        raise SystemExit('original gray-drawing guard line missing')
    i = matches[0]
    error = guard_error_index(source, i + 1)
    if (source[i-1].strip() != '#endif /* WOW_ORIGINAL_STATE_DRAWING */' or
            not source[i+1].strip().startswith('#if 0') or source[error].strip() != marker or
            source[error+1].strip() != '#endif' or source[error+2].strip() != '#line ' + str(continuation) or
            source[error+3].strip() != '#ifdef WOW_ORIGINAL_STATE_DRAWING' or
            source[error+4].strip() != '#line ' + str(continuation) or source[error+5].strip() != 'break;'):
        raise SystemExit('gray-drawing original depth or continuation changed')
    probe.write_text('\n'.join(['#line ' + str(original), '#if 0', marker, '#endif',
                                 '#line ' + str(continuation), 'int gray_after = __LINE__;']) + '\n', encoding='ascii')
    selected = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+gray_after\s*=\s*' + str(continuation) + r'\s*;', selected.stdout):
        raise SystemExit('gray-drawing continuation line failed')
    probe.write_text(probe.read_text(encoding='ascii').replace('#if 0', '#if 1', 1), encoding='ascii')
    rejected = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
    if not rejected.returncode or marker[7:] not in rejected.stdout + rejected.stderr:
        raise SystemExit('gray-drawing disabled branch did not reject')
print('PASS original gray-drawing outer disabled guards and continuation lines')

source = (repo / 'src/non-mvdm/windows/core/ntuser/client/clmsg.c').read_text(encoding='latin1').splitlines()
depth = 0
for index, line in enumerate(source):
    directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
    if line.strip() in ('#line 566', '#line 572', '#line 905', '#line 1214') and depth != 0:
        raise SystemExit('default-window original condition is not first-depth')
    if directive:
        depth += -1 if directive.group(1) == 'endif' else 1
    if depth < 0:
        raise SystemExit('default-window condition structure is unbalanced')
if depth:
    raise SystemExit('default-window condition structure is unbalanced')
for original, condition, marker, definition, continuation in (
    (566, '#ifdef FE_IME', 'DIVERGENCE OPENNT-HOST-132: FE_IME branch is not selected; see mirror register.', '/DFE_IME', 572),
    (572, '#ifdef DEBUG', 'Excluded unsupported original DEBUG default-window path', '/DDEBUG', 580),
    (905, '#if defined(FE_IME) // DefWindowProcWorker()', 'DIVERGENCE OPENNT-HOST-132: FE_IME branch is not selected; see mirror register.', '/DFE_IME', 1210),
    (1214, '#ifdef DEBUG', 'Excluded unsupported original DEBUG default-window close', '/DDEBUG', 1217),
):
    i = source.index('#line ' + str(original))
    j = guard_error_index(source, i + 1)
    if source[i+1].strip() != condition or source[j].strip() != '#error ' + marker or source[j+1].strip() != '#endif':
        raise SystemExit('default-window original guard missing at ' + str(original))
    probe.write_text('\n'.join(['#line ' + str(original), condition, '#error ' + marker,
                                 '#endif', '#line ' + str(continuation),
                                 'int default_after = __LINE__;']) + '\n', encoding='ascii')
    selected = subprocess.run(base + [str(probe)], capture_output=True, text=True, errors='replace')
    rejected = subprocess.run(base + [definition, str(probe)], capture_output=True, text=True, errors='replace')
    if selected.returncode or not re.search(r'int\s+default_after\s*=\s*' + str(continuation) + r'\s*;', selected.stdout):
        raise SystemExit('default-window continuation failed at ' + str(original))
    if not rejected.returncode or marker not in rejected.stdout + rejected.stderr:
        raise SystemExit('default-window unsupported condition did not reject')
print('PASS four original default-window outer guards and continuation lines')

source = (repo / 'src/non-mvdm/windows/core/ntuser/kernel/ntstubs.c').read_text(encoding='latin1')
guard = ('#line 8377\n#if 0\n'
         '#error Excluded original disabled full-screen message stub\n'
         '#endif\n#line 8412')
if guard not in source:
    raise SystemExit('kernel ntstubs original disabled full-screen guard missing')
print('PASS original kernel ntstubs disabled full-screen guard and continuation')

for path, original, continuation, marker in (
    ('src/non-mvdm/windows/core/ntuser/kernel/server.c', 461, 464,
     'DIVERGENCE OPENNT-HOST-128: original literal #if 0 excludes this legacy branch; see mirror register.'),
    ('src/non-mvdm/windows/core/ntuser/client/clmsg.c', 1650, 1677,
     'Excluded original disabled WMCTLCOLOR handler'),
):
    lines = (repo / path).read_text(encoding='latin1').splitlines()
    depth = 0
    for line in lines:
        directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
        if line.strip() == '#line ' + str(original) and depth:
            raise SystemExit(path + ' original disabled guard is not first-depth')
        if directive:
            depth += -1 if directive.group(1) == 'endif' else 1
    i = lines.index('#line ' + str(original))
    j = guard_error_index(lines, i + 1)
    if lines[i+1].strip() != '#if 0' or lines[j].strip() != '#error ' + marker or lines[j+1].strip() != '#endif':
        raise SystemExit(path + ' original disabled guard missing')
    if '#line ' + str(continuation) not in lines[j+2:j+5]:
        raise SystemExit(path + ' original continuation missing')
print('PASS original server and clmsg first-depth disabled guards')

lines = (repo / 'src/non-mvdm/windows/core/ntuser/client/clres.c').read_text(encoding='latin1').splitlines()
i = lines.index('#line 1674')
depth = 0
for line in lines[:i]:
    directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
    if directive:
        depth += -1 if directive.group(1) == 'endif' else 1
j = guard_error_index(lines, i + 1)
if depth or not lines[i + 1].startswith('#if 0') or not lines[j].startswith('#error '):
    raise SystemExit('clres bitmap original first-depth disabled guard missing')
if '#line 1692' not in lines[i:i + 10] or '#line 1703' not in lines[i:i + 25]:
    raise SystemExit('clres original bitmap continuation lines missing')
print('PASS clres first-depth original bitmap guard and continuations')

lines = (repo / 'src/mvdm-host/softpc.new/host/src/nt_fulsc.c').read_text(encoding='latin1').splitlines()
i = lines.index('#line 902')
j = guard_error_index(lines, i + 1)
if not lines[i + 1].lstrip().startswith('#if 0') or not lines[j].lstrip().startswith('#error '):
    raise SystemExit('nt_fulsc first disabled original guard missing')
if '#line 925' not in lines[i:i + 6] or '#line 964' not in lines[i:i + 12]:
    raise SystemExit('nt_fulsc original continuation lines missing')
print('PASS nt_fulsc original adjacent disabled guards and continuation lines')

lines = (repo / 'src/mvdm-host/wow32/wow32.c').read_text(encoding='latin1').splitlines()
for current, expected, selector in (
    (166, 181, '#if defined (_X86_)'),
    (446, 468, '#ifdef DEBUG'),
    (698, 778, '#ifdef DEBUG_OR_WOWPROFILE'),
    (703, 784, '#ifdef DEBUG_OR_WOWPROFILE'),
    (818, 963, '#ifdef DEBUG_OR_WOWPROFILE'),
    (823, 967, '#ifdef  WOWPROFILE'),
    (887, 1031, '#ifdef DEBUG_OR_WOWPROFILE'),
):
    matches = []
    for i, line in enumerate(lines):
        if not line.startswith(selector):
            continue
        j = guard_error_index(lines, i)
        if (j + 2 < len(lines) and lines[j].startswith('#error ')
                and lines[j + 1].lstrip().startswith('#endif')
                and lines[j + 2] == '#line ' + str(expected)):
            matches.append(i)
    if len(matches) != 1:
        raise SystemExit('wow32 original continuation mismatch at current line ' + str(current))
print('PASS seven original WOW32 disabled-guard continuation lines')

lines = (repo / 'src/non-mvdm/windows/core/ntuser/client/rtlinit.c').read_text(encoding='latin1').splitlines()
for original, continuation in ((319, 322), (326, 333), (349, 352), (361, 364)):
    i = lines.index('#line ' + str(original))
    depth = 0
    for line in lines[:i]:
        directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
        if directive:
            depth += -1 if directive.group(1) == 'endif' else 1
    j = guard_error_index(lines, i + 1)
    if (depth or lines[i + 1] != '#if DBG' or not lines[j].startswith('#error ')
            or not lines[j + 1].startswith('#endif')
            or '#line ' + str(continuation) not in lines[j + 2:j + 5]):
        raise SystemExit('rtlinit original DBG guard or continuation mismatch at ' + str(original))
lines = (repo / 'src/non-mvdm/windows/core/ntuser/client/dlgmgr.c').read_text(encoding='latin1').splitlines()
i = lines.index('#line 1056')
j = guard_error_index(lines, i + 1)
if lines[i + 1] != '#ifdef SYSMODALWINDOWS' or not lines[j].startswith('#error ') or lines[j + 2] != '#line 1066':
    raise SystemExit('dlgmgr original SYSMODALWINDOWS guard or continuation mismatch')
print('PASS original rtlinit DBG guards and dlmgr original continuations')

lines = (repo / 'src/non-mvdm/windows/core/ntuser/client/ntstubs.c').read_text(encoding='latin1').splitlines()
for original, continuation in ((953, 956), (965, 970)):
    i = lines.index('#line ' + str(original))
    depth = 0
    for line in lines[:i]:
        directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
        if directive:
            depth += -1 if directive.group(1) == 'endif' else 1
    j = guard_error_index(lines, i + 1)
    if (depth or lines[i + 1] != '#ifdef DEBUG' or not lines[j].startswith('#error ')
            or not lines[j + 1].startswith('#endif')
            or '#line ' + str(continuation) not in lines[j + 2:j + 5]):
        raise SystemExit('ntstubs original DEBUG guard or continuation mismatch at ' + str(original))
lines = (repo / 'src/mvdm-host/softpc.new/host/src/nt_event.c').read_text(encoding='latin1').splitlines()
i = lines.index('#line 1493')
j = guard_error_index(lines, i + 1)
if (lines[i + 1] != '#ifndef PROD' or not lines[j].startswith('#error ')
        or not lines[j + 1].startswith('#endif') or lines[j + 2] != '#line 1496'):
    raise SystemExit('nt_event original PROD guard or continuation mismatch')
print('PASS original ntstubs DEBUG and nt_event PROD outer guards')

lines = (repo / 'src/non-mvdm/windows/core/ntuser/client/dlgmgr2.c').read_text(encoding='latin1').splitlines()
for original, selector, continuation in ((221, '#ifdef FE_SB', 235), (240, '#ifdef FE_IME', 244), (281, '#ifdef LATER', 309)):
    i = lines.index('#line ' + str(original))
    depth = 0
    for line in lines[:i]:
        directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
        if directive:
            depth += -1 if directive.group(1) == 'endif' else 1
    j = guard_error_index(lines, i + 1)
    if depth or not lines[i + 1].startswith(selector) or not lines[j].startswith('#error '):
        raise SystemExit('dlgmgr2 original first-depth guard missing at ' + str(original))
    if '#line ' + str(continuation) not in lines[i:i + 18]:
        raise SystemExit('dlgmgr2 original continuation missing at ' + str(original))
print('PASS original dlgmgr2 first-depth guards and continuations')

lines = (repo / 'src/non-mvdm/windows/core/ntuser/rtl/chartran.c').read_text(encoding='latin1').splitlines()
for original, continuation in ((446, 450), (457, 499)):
    i = lines.index('#line ' + str(original))
    j = guard_error_index(lines, i + 1)
    if not lines[i + 1].startswith('#ifdef FE_SB') or not lines[j].startswith('#error '):
        raise SystemExit('chartran original FE_SB guard missing at ' + str(original))
    if '#line ' + str(continuation) not in lines[i:i + 12]:
        raise SystemExit('chartran original FE_SB continuation missing at ' + str(original))
if '#line 497' not in lines:
    raise SystemExit('chartran original FE_SB else line missing')
print('PASS original chartran FE_SB guard continuations')

lines = (repo / 'src/mvdm-host/softpc.new/base/ccpu386/c_main.c').read_text(encoding='latin1').splitlines()
i = lines.index('#line 226')
j = guard_error_index(lines, i + 1)
if lines[i + 1] != '#if defined(SFELLOW)' or not lines[j].startswith('#error ') or lines[j + 1] != '#else' or lines[j + 2] != '#line 229' or '#line 231' not in lines[j + 3:j + 7]:
    raise SystemExit('c_main original SFELLOW condition or continuation missing')
print('PASS original c_main SFELLOW condition and continuations')

lines = (repo / 'src/mvdm-host/dpmi32/dpmiint.c').read_text(encoding='latin1').splitlines()
i = lines.index('#line 387')
if lines[i + 1] != '#ifdef i386' or not lines[i + 2].startswith('#error ') or lines[i + 4] != '#line 397':
    raise SystemExit('DPMI original i386 stack-publication boundary missing')
print('PASS original DPMI stack-publication guard and continuation')

lines = (repo / 'src/mvdm-host/dpmi32/dpmiint.c').read_text(encoding='latin1').splitlines()
i = lines.index('#line 88')
if lines[i + 1] != '#ifdef i386' or '#line 107' not in lines[i + 2:i + 9]:
    raise SystemExit('DPMI original i386 handler boundary or continuation missing')
print('PASS original DPMI i386 handler boundary and continuation')

lines = (repo / 'src/mvdm-host/softpc.new/base/ccpu386/c_main.c').read_text(encoding='latin1').splitlines()
for anchor in (537, 545, 4402, 4443, 4492, 4498):
    if '#line ' + str(anchor) not in lines:
        raise SystemExit('c_main original SFELLOW continuation missing: ' + str(anchor))
print('PASS original c_main SFELLOW continuation anchors')

lines = (repo / 'src/non-mvdm/base/win32/client/vdm.c').read_text(encoding='latin1').splitlines()
for original, selector, continuation in ((205, '#ifdef _X86_', 209), (259, '#ifdef _MIPS_', 272)):
    i = lines.index('#line ' + str(original))
    depth = 0
    for line in lines[:i]:
        directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
        if directive:
            depth += -1 if directive.group(1) == 'endif' else 1
    if depth or lines[i + 1] != selector or '#line ' + str(continuation) not in lines[i:i + 15]:
        raise SystemExit('original VDM classifier first-depth guard mismatch at ' + str(original))
if '#line 206' not in lines or '#line 269' not in lines:
    raise SystemExit('original VDM classifier selected branch anchors missing')
print('PASS original VDM classifier first-depth guards and continuations')

lines = (repo / 'src/non-mvdm/windows/core/ntuser/rtl/chartran.c').read_text(encoding='latin1').splitlines()
for anchor in (515, 520, 524, 530, 537, 543):
    if '#line ' + str(anchor) not in lines:
        raise SystemExit('chartran later original continuation missing: ' + str(anchor))
print('PASS original chartran later guard continuation anchors')
