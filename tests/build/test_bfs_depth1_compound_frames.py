"""Verify first-depth exclusions whose actual compile contexts were probed."""

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
evidence = json.loads((repo / 'artifacts/mvdm-audit/s13-r457-frames.json').read_text())
if len(evidence['records']) != 2:
    raise SystemExit('expected two boundaries')
probe = scratch / 'guard.c'
base = ['cl.exe', '/nologo', '/EP', '/TC']
for item in evidence['records']:
    lines = (repo / item['path']).read_text(encoding='latin1').splitlines()
    original, continuation = item['original_line'], item['original_end'] + 1
    marker = f'#error Unsupported {item["macro"]} original branch'
    expected = [f'#line {original}', item['selector'].strip(), f'#line {original + 1}', marker]
    if item['original_else_line']:
        expected += [item['else'].strip(), f'#line {item["original_else_line"] + 1}']
    expected += [item['endif'].strip(), f'#line {continuation}']
    matches = [start for start in range(len(lines) - len(expected) + 1)
               if [line.strip() for line in lines[start:start + len(expected)]] == expected]
    if len(matches) != 1:
        raise SystemExit(f'original guard missing or ambiguous: {item["path"]}:{original}')
    i = matches[0] + 1
    depth = 0
    for line in lines[:i]:
        directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
        if directive:
            depth += -1 if directive.group(1) == 'endif' else 1
    if depth:
        raise SystemExit(f'original guard not first-depth: {item["path"]}:{original}')
    probe.write_text('\n'.join(expected + ['int after_guard = __LINE__;']) + '\n', encoding='ascii')
    allowed = subprocess.run(base + [f'/U{item["macro"]}', str(probe)], capture_output=True, text=True)
    rejected = subprocess.run(base + (['/U_USERK_', '/UWOW'] if item['macro'] == 'DEBUG' else []) + [f'/D{item["macro"]}=1', str(probe)], capture_output=True, text=True)
    if (allowed.returncode or f'int after_guard = {continuation};' not in allowed.stdout
            or not rejected.returncode or f'({original + 1})' not in rejected.stdout + rejected.stderr
            or marker[7:] not in rejected.stdout + rejected.stderr):
        raise SystemExit(f'original guard control failed: {item["path"]}:{original}')
print('PASS two original first-depth exclusions and diagnostic lines')
