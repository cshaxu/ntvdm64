#!/usr/bin/env python3
"""Compare frozen MVDM selection claims with current product Ninja inputs.

This records build dependencies, not linker extraction or behavioral proof.
"""
import collections
import csv
import hashlib
import json
import subprocess
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
AUDIT = REPO / 'artifacts/mvdm-audit'
TARGETS = (
    ('ntvdm', 'build/.ninja/msvc-x86-release/worker', 'ntvdm.exe'),
    ('wow32', 'build/.ninja/msvc-x86-release/wow32', 'wow32.dll'),
    ('vdmredir', 'build/.ninja/msvc-x86-release/worker', 'VDMREDIR.dll'),
)


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def main():
    source = AUDIT / 'frozen-file-reconciliation.csv'
    with source.open(encoding='utf-8', newline='') as stream:
        rows = list(csv.DictReader(stream))
    prefix = (REPO / 'src/mvdm').as_posix().lower() + '/'
    seen = collections.defaultdict(set)
    identities = []
    for name, relative, target in TARGETS:
        graph = REPO / relative / 'build.ninja'
        output = subprocess.check_output(
            ['ninja', '-C', str(REPO / relative), '-t', 'inputs', '-E', target],
            text=True, encoding='utf-8')
        count = 0
        for item in output.splitlines():
            normalized = item.replace('\\', '/')
            if normalized.lower().startswith(prefix):
                seen[normalized[len(prefix):].lower()].add(name)
                count += 1
        identities.append({'target': name, 'graph': relative + '/build.ninja',
                           'graph_sha256': digest(graph),
                           'transitive_mvdm_inputs': count})
    fields = ['relative_path', 'frozen_product_link_selection',
              'current_transitive_inputs', 'observation', 'next_proof']
    discrepancies = []
    for row in rows:
        owners = sorted(seen.get(row['relative_path'].lower(), set()))
        if owners and row['product_link_selection'] == 'not-product-selected':
            discrepancies.append({
                'relative_path': row['relative_path'],
                'frozen_product_link_selection': row['product_link_selection'],
                'current_transitive_inputs': ';'.join(owners),
                'observation': 'Frozen inventory says not-product-selected; current Ninja target closure includes source. Compilation is not linker extraction or execution.',
                'next_proof': 'Inspect actual object/map, selected preprocessor, source role, and runtime evidence.',
            })
    with (AUDIT / 'graph-selection-discrepancies.csv').open(
            'w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        writer.writerows(discrepancies)
    identity = {'targets': identities, 'discrepancy_count': len(discrepancies),
                'definition': 'transitive build input mismatch only; not evidence of linked or executed code'}
    (AUDIT / 'graph-selection-identity.json').write_text(
        json.dumps(identity, indent=2) + '\n', encoding='utf-8')
    print(f'Checked {len(rows)} frozen rows: {len(discrepancies)} build-input discrepancies.')


if __name__ == '__main__':
    main()
