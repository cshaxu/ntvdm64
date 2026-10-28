import csv
import hashlib
from pathlib import Path

root = Path('src/mvdm')
with open('artifacts/mvdm-audit/host-ch-scope/inventory.csv', encoding='utf-8') as f:
    inventory = {r['path']: r for r in csv.DictReader(f) if r['scope'] == 'host-candidate'}
with open('artifacts/mvdm-audit/frozen-file-reconciliation.csv', encoding='utf-8-sig') as f:
    frozen = {r['relative_path']: r for r in csv.DictReader(f)}
with open('artifacts/mvdm-audit/mirror-diff-code-retention.csv', encoding='utf-8-sig') as f:
    diffs = {r['relative_path'] for r in csv.DictReader(f)}
unlisted, missing, no_original = [], [], []
compared = 0
for path in inventory:
    record = frozen.get(path)
    current = root / path
    if not current.is_file():
        missing.append(path)
        continue
    if not record or not (record['opennt_sha256'] or record['opennt45_sha256']):
        no_original.append(path)
        continue
    compared += 1
    actual = hashlib.sha256(current.read_bytes()).hexdigest().upper()
    original = (record['opennt_sha256'] or record['opennt45_sha256']).upper()
    if actual != original and path not in diffs:
        unlisted.append(path)
print('compared', compared, 'unlisted differences', len(unlisted),
      'missing current', len(missing), 'no frozen original', len(no_original))
print('unlisted:', unlisted)
print('missing current files:', missing)
if unlisted:
    raise SystemExit('current host C/H differences missing from retention table')
