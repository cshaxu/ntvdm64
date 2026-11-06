"""Pin the existing read-only audit identities; never infer product completion."""
import csv
import hashlib
import json
from collections import Counter
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
OUT = REPO / 'artifacts/mvdm-audit/t478-final'
ROOTS = {n: Path('O:/repos.hobby/ntvdm64-' + n + '-audit/management') for n in ('dos16', 'wow32')}
INPUTS = {'dos16': ['README.md', 'seam-scope.json', 'seam-work.json', 'seam-records.json', 'module-handoffs.json'],
          'wow32': ['README.md', 'SEAM-SCOPE.md', 'seams.csv', 'seam-sites.csv', 'seam-gaps.csv', 'IMPLEMENTATION-SEAMS.md']}

def main():
    pins = []
    for audit, names in INPUTS.items():
        for name in names:
            p = ROOTS[audit] / name
            b = p.read_bytes()
            pins.append({'audit': audit, 'path': str(p), 'bytes': len(b), 'sha256': hashlib.sha256(b).hexdigest()})
    rows = []
    records = json.loads((ROOTS['dos16'] / 'seam-records.json').read_text(encoding='utf-8-sig'))['records']
    for r in records:
        rows.append({'audit': 'dos16', 'id': r['id'], 'edition': 'OpenNT', 'module': r['module'],
                     'contract': r['name'], 'unit': r['unit'], 'audit_label': str(r.get('audit_closed')),
                     'validity': r.get('evidence_type', ''), 'reconciliation': 'pending-product-evidence', 'evidence': ''})
    with (ROOTS['wow32'] / 'seams.csv').open(encoding='utf-8-sig', newline='') as f:
        for r in csv.DictReader(f):
            rows.append({'audit': 'wow32', 'id': r['id'], 'edition': r['edition'], 'module': r['module'],
                         'contract': r['contract_identity'], 'unit': r['unit'], 'audit_label': r['audit_status'],
                         'validity': r['validity'], 'reconciliation': 'pending-product-evidence' if r['edition'] == 'OpenNT' else 'inactive-edition-evidence-only', 'evidence': ''})
    identities = [(r['audit'], r['id']) for r in rows]
    if len(set(identities)) != len(rows):
        raise ValueError('Duplicate stable identity')
    # Preserve manually established dispositions across input refreshes.
    target = OUT / 'inventory-crosswalk.csv'
    if target.exists():
        with target.open(encoding='utf-8-sig', newline='') as f:
            old = {(r['audit'], r['id']): r for r in csv.DictReader(f)}
        if set(old) != set(identities):
            raise ValueError('Input identity set changed; review delta explicitly')
        for row in rows:
            prior = old[(row['audit'], row['id'])]
            if any(row[k] != prior[k] for k in ('edition', 'module', 'contract', 'unit', 'audit_label', 'validity')):
                raise ValueError('Input contract metadata changed: ' + row['id'])
            row['reconciliation'], row['evidence'] = prior['reconciliation'], prior['evidence']
    with target.open('w', encoding='utf-8', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=list(rows[0])); writer.writeheader(); writer.writerows(rows)
    summary = {'inputs': pins, 'stable_rows': len(rows),
               'counts': dict(Counter(r['audit'] + '/' + r['edition'] for r in rows)),
               'dispositions': dict(Counter(r['reconciliation'] for r in rows)),
               'qualification': 'Identity accounting only; no inferred defect, product closure or new source-audit scope.'}
    (OUT / 'inventory-inputs.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({k: v for k, v in summary.items() if k != 'inputs'}, ensure_ascii=False))

if __name__ == '__main__':
    main()
