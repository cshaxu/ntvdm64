"""Verify selected USER guard exclusions and their diagnostic line anchors."""

import argparse
import json
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
plan = json.loads((repo / 'artifacts/mvdm-audit/s13-other-feature-batch-plan.json').read_text())
checked = 0
for item in plan['files']:
    source = (repo / item['path']).read_text(encoding='latin1').split('\n')
    for guard in item['guards']:
        macro = guard['expression']
        marker = f'#error Excluded unsupported original {macro} branch'
        prefix = [f'#line {guard["original_line"]}', f'#ifdef {macro}']
        suffix = [marker, '#endif', f'#line {guard["continuation"]}']
        variants = [prefix + suffix,
                    prefix + [f'#line {guard["original_line"] + 1}'] + suffix]
        matches = [(i, expected) for expected in variants
                   for i in range(len(source) - len(expected) + 1)
                   if [line.strip() for line in source[i:i + len(expected)]] == expected]
        if len(matches) != 1:
            raise SystemExit(f'guard not unique: {item["path"]}: {macro}')
        index, expected = matches[0]
        probe = scratch / 'user-feature-guard.c'
        probe.write_text('\n'.join(source[index:index + len(expected)]) + '\n', encoding='ascii')
        command = ['cl.exe', '/nologo', '/EP', '/TC']
        normal = subprocess.run(command + [str(probe)], capture_output=True, text=True)
        if normal.returncode:
            raise SystemExit(f'normal profile failed: {macro}: {normal.stderr}')
        rejected = subprocess.run(command + [f'/D{macro}=1', str(probe)], capture_output=True, text=True)
        diagnostic = rejected.stdout + rejected.stderr
        if not rejected.returncode or f'({guard["original_line"] + 1})' not in diagnostic or marker[7:] not in diagnostic:
            raise SystemExit(f'rejection or original diagnostic line missing: {macro}: {diagnostic}')
        checked += 1
if checked != 8:
    raise SystemExit(f'expected eight guards, checked {checked}')
print(f'PASS {checked} USER guard normal, rejection and original diagnostic-line checks')
