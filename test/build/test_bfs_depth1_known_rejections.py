"""Verify original single-arm exclusions restored in omitted source scopes."""

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
evidence = json.loads((repo / 'artifacts/mvdm-audit/s13-r429-known-rejected-frames.json').read_text())
if len(evidence['records']) != 48:
    raise SystemExit('expected 48 restored original boundaries')
probe = scratch / 'guard.c'
base = ['cl.exe', '/nologo', '/EP', '/TC']
for item in evidence['records']:
    lines = (repo / item['path']).read_text(encoding='latin1').splitlines()
    original = item['original_line']
    continuation = item['original_end'] + 1
    marker = f'#error Excluded unsupported original {item["macro"]} branch'
    selector = item.get('selector', item.get('directive')).strip()
    hits = [n + 1 for n in range(len(lines) - 1)
            if lines[n].strip() == f'#line {original}' and lines[n + 1].strip() == selector]
    if len(hits) != 1:
        raise SystemExit(f'original guard missing or ambiguous: {item["path"]}:{original}')
    i = hits[0]
    if (lines[i - 1].strip() != f'#line {original}' or lines[i].strip() != item['selector'].strip()
            or lines[i + 1].strip() != f'#line {original + 1}'
            or lines[i + 2].strip() != marker or lines[i + 3].strip() != item['endif'].strip()
            or lines[i + 4].strip() != f'#line {continuation}'):
        raise SystemExit(f'original guard or line mismatch: {item["path"]}:{original}')
    depth = 0
    for line in lines[:i]:
        directive = re.match(r'^\s*#\s*(if|ifdef|ifndef|endif)\b', line)
        if directive:
            depth += -1 if directive.group(1) == 'endif' else 1
    if depth:
        raise SystemExit(f'original guard not first-depth: {item["path"]}:{original}')
    probe.write_text('\n'.join(lines[i - 1:i + 5] + ['int after_guard = __LINE__;']) + '\n', encoding='ascii')
    allowed = subprocess.run(base + [f'/U{item["macro"]}', str(probe)], capture_output=True, text=True)
    rejected = subprocess.run(base + [f'/D{item["macro"]}=1', str(probe)], capture_output=True, text=True)
    if (allowed.returncode or f'int after_guard = {continuation};' not in allowed.stdout
            or not rejected.returncode or f'({original + 1})' not in rejected.stdout + rejected.stderr
            or marker[7:] not in rejected.stdout + rejected.stderr):
        raise SystemExit(f'original guard control failed: {item["path"]}:{original}')
print('PASS 48 original first-depth exclusions and diagnostic lines')
