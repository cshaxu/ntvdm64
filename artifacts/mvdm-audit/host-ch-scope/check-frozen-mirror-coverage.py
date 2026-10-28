import csv
import hashlib
from pathlib import Path

root = Path('src/mvdm')
with open('artifacts/mvdm-audit/host-ch-scope/inventory.csv', encoding='utf-8') as f:
    rows = list(csv.DictReader(f))
    inventory = {r['path']: r for r in rows if r['scope'] == 'host-candidate'}
    all_paths = [r['path'].casefold() for r in rows]
    if len(all_paths) != len(set(all_paths)):
        raise SystemExit('duplicate case-insensitive inventory path')
    recorded = set(all_paths)
    actual = {p.relative_to(root).as_posix().casefold() for p in root.rglob('*')
              if p.is_file() and p.suffix.casefold() in ('.c', '.h')}
    unlisted_sources = sorted(actual - recorded)
    print('current C/H paths', len(actual), 'missing from inventory', len(unlisted_sources))
    if unlisted_sources:
        raise SystemExit('C/H paths missing from inventory: ' + str(unlisted_sources))
with open('artifacts/mvdm-audit/frozen-file-reconciliation.csv', encoding='utf-8-sig') as f:
    frozen = {r['relative_path']: r for r in csv.DictReader(f)}
with open('artifacts/mvdm-audit/mirror-diff-code-retention.csv', encoding='utf-8-sig') as f:
    diff_rows = list(csv.DictReader(f))
    diffs = {r['relative_path']: r for r in diff_rows}
    if len(diffs) != len(diff_rows):
        raise SystemExit('duplicate path in mirror-difference table')
unlisted, missing, unexplained_missing, no_original, stale = [], [], [], [], []
compared = 0
for path in inventory:
    record = frozen.get(path)
    current = root / path
    has_original = bool(record and (record['opennt_sha256'] or record['opennt45_sha256']))
    if not has_original:
        no_original.append(path)
    if not current.is_file():
        missing.append(path)
        if inventory[path]['ledger_status'] not in ('excluded', '整体排除'):
            unexplained_missing.append(path)
        continue
    if not has_original:
        continue
    compared += 1
    actual = hashlib.sha256(current.read_bytes()).hexdigest().upper()
    original = (record['opennt_sha256'] or record['opennt45_sha256']).upper()
    if actual != original:
        if path not in diffs:
            unlisted.append(path)
        elif (diffs[path]['current_sha256'].upper() != actual or
              diffs[path]['original_sha256'].upper() != original or
              not diffs[path]['text_retention_percent']):
            stale.append(path)
    elif path in diffs:
        stale.append(path)
print('compared', compared, 'unlisted differences', len(unlisted),
      'stale estimates', len(stale), 'missing current', len(missing),
      'no frozen original', len(no_original))
print('unlisted:', unlisted)
print('stale estimates:', stale)
print('missing current files:', missing)
print('missing without exclusion:', unexplained_missing)
if unlisted or stale or no_original or unexplained_missing:
    raise SystemExit('host C/H differences, estimates or original identities require reconciliation')
