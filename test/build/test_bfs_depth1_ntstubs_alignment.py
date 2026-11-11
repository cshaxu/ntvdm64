"""Check the original x86 alignment condition and its macro lifetime."""

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
source = (repo / 'src/non-mvdm/windows/core/ntuser/kernel/ntstubs.c').read_text(encoding='latin1')
start = source.index('#line 7261\n#if defined(_X86_)\n')
end = source.index('#line 7315', start)
fragment = source[start:end]
for expected in ('#define DATAALIGN sizeof(BYTE)', '#define CHARALIGN sizeof(BYTE)',
                 '#undef DATAALIGN', '#undef CHARALIGN'):
    if expected not in fragment:
        raise SystemExit('missing original alignment or lifetime: ' + expected)
probe = scratch / 'alignment.c'
probe.write_text(fragment + '\n#if defined(DATAALIGN) || defined(CHARALIGN)\n'
                 '#error alignment macros escaped\n#endif\n', encoding='ascii')
base = ['cl.exe', '/nologo', '/EP', '/TC']
normal = subprocess.run(base + ['/D_X86_=1', str(probe)], capture_output=True, text=True)
rejected = subprocess.run(base + ['/U_X86_', str(probe)], capture_output=True, text=True)
diagnostic = rejected.stdout + rejected.stderr
if normal.returncode or not rejected.returncode:
    raise SystemExit('incorrect selected or rejected alignment configuration')
if '(7265)' not in diagnostic or 'non-x86 class-registration alignment' not in diagnostic:
    raise SystemExit('incorrect original non-x86 diagnostic: ' + diagnostic)
print('PASS original x86 alignment, macro lifetime and non-x86 rejection')

import json

evidence = json.loads((repo / 'artifacts/mvdm-audit/s13-r427-ntstubs-x86-exclusions.json').read_text())
if len(evidence['items']) != 51:
    raise SystemExit('expected 51 original receive-stub boundaries')
lines = source.splitlines()
for item in evidence['items']:
    original = item['original_condition_line']
    alternate = item['original_else_line']
    continuation = item['original_endif_line'] + 1
    expected = [f'#line {original}', '#if defined(_X86_)', f'#line {original + 1}',
                '#else', f'#line {alternate + 1}',
                '#error Excluded unsupported original non-x86 receive-stub branch',
                '#endif', f'#line {continuation}']
    matches = [i for i in range(len(lines) - len(expected) + 1)
               if [line.strip() for line in lines[i:i + len(expected)]] == expected]
    if len(matches) != 1:
        raise SystemExit(f'original receive-stub boundary {original} missing or duplicated')
    probe.write_text('\n'.join(expected + ['int after_guard = __LINE__;']) + '\n', encoding='ascii')
    selected = subprocess.run(base + ['/D_X86_=1', str(probe)], capture_output=True, text=True)
    rejected = subprocess.run(base + ['/U_X86_', str(probe)], capture_output=True, text=True)
    if (selected.returncode or f'int after_guard = {continuation};' not in selected.stdout
            or not rejected.returncode or f'({alternate + 1})' not in rejected.stdout + rejected.stderr
            or 'non-x86 receive-stub branch' not in rejected.stdout + rejected.stderr):
        raise SystemExit(f'original receive-stub boundary {original} controls failed')
print('PASS 51 original first-depth receive-stub exclusions and diagnostic lines')
