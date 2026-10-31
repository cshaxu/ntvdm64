import csv
import hashlib
from collections import Counter
from pathlib import Path

root = Path('src/mvdm')
with open('artifacts/mvdm-audit/host-ch-scope/inventory.csv', encoding='utf-8') as f:
    rows = list(csv.DictReader(f))
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
    pending_roles = Counter(r['scope'] for r in rows
                            if r['scope'] in ('role-pending', 'guest-target-observed',
                                              'tool-or-scaffold-candidate'))
    print('source roles requiring scoped interpretation', dict(sorted(pending_roles.items())))
with open('artifacts/mvdm-audit/frozen-file-reconciliation.csv', encoding='utf-8-sig') as f:
    frozen = {r['relative_path']: r for r in csv.DictReader(f)}
with open('artifacts/mvdm-audit/mirror-diff-code-retention.csv', encoding='utf-8-sig') as f:
    diff_rows = list(csv.DictReader(f))
    diffs = {r['relative_path']: r for r in diff_rows}
    if len(diffs) != len(diff_rows):
        raise SystemExit('duplicate path in mirror-difference table')
    diff_paths = {path.casefold() for path in diffs}
    diff_not_partial = sorted(
        r['path'] for r in rows
        if r['path'].casefold() in diff_paths
        and r['ledger_status'] not in ('partial', '部分复通'))
    print('recorded C/H mirror differences not classified partial',
          len(diff_not_partial))
    if diff_not_partial:
        raise SystemExit('recorded C/H mirror differences must be partial: ' +
                         str(diff_not_partial))
unlisted, missing, unexplained_missing, no_original, stale = [], [], [], [], []
compared = 0
other_compared = 0
for row in rows:
    path = row['path']
    record = frozen.get(path)
    current = root / path
    has_original = bool(record and (record['opennt_sha256'] or record['opennt45_sha256']))
    if not has_original:
        no_original.append(path)
    if not current.is_file():
        missing.append(path)
        if row['ledger_status'] not in ('excluded', '整体排除'):
            unexplained_missing.append(path)
        continue
    if not has_original:
        continue
    if row['scope'] == 'host-candidate':
        compared += 1
    else:
        other_compared += 1
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

print('additional non-host C/H comparisons', other_compared)
print('compared', compared, 'unlisted differences', len(unlisted),
      'stale estimates', len(stale), 'missing current', len(missing),
      'no frozen original', len(no_original))
print('unlisted:', unlisted)
print('stale estimates:', stale)
print('missing current files:', missing)
print('missing without exclusion:', unexplained_missing)
if unlisted or stale or no_original or unexplained_missing:
    raise SystemExit('host C/H differences, estimates or original identities require reconciliation')
