"""Check the frozen mirror-diff rows against the provisional host C/H ledger."""
import csv
from pathlib import Path

root = Path('artifacts/mvdm-audit')
with (root / 'host-ch-scope/inventory.csv').open(encoding='utf-8', newline='') as f:
    inventory = {r['path']: r for r in csv.DictReader(f)}
with (root / 'mirror-diff-code-retention.csv').open(encoding='utf-8-sig', newline='') as f:
    diffs = list(csv.DictReader(f))
selected = [r for r in diffs if r['relative_path'] in inventory and
            inventory[r['relative_path']]['scope'] == 'host-candidate']
bad = [(r['relative_path'], inventory[r['relative_path']]['ledger_status'])
       for r in selected if inventory[r['relative_path']]['ledger_status'] not in
       ('partial', '部分复通')]
print(f'host C/H mirror-diff rows: {len(selected)}; non-partial rows: {len(bad)}')
if bad:
    raise SystemExit(str(bad))
