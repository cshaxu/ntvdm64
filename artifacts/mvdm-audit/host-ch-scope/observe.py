"""Snapshot four S10 shard CSVs into a provisional per-file C/H scope inventory.

Usage: python artifacts/mvdm-audit/host-ch-scope/observe.py
This writes only its own inventory/evidence outputs; input hashes bind the result
 to a particular shard state. It does not update or classify the source ledgers.
"""
import collections
import csv
import hashlib
import json
from pathlib import Path

root = Path('artifacts/mvdm-audit')
out = root / 'host-ch-scope'
inputs = ('shard-0/review.csv', 'shard-1/file-audit.csv',
          'shard-2/file-audit.csv', 'shard-3/file-review.csv')
status_fields = {'shard-0/review.csv': 'classification',
                 'shard-1/file-audit.csv': 'audit_status',
                 'shard-2/file-audit.csv': 'decision',
                 'shard-3/file-review.csv': 'review_class'}
tool_paths = ('vdmutils/', 'vdmexts/', 'bde/', 'ieuvddex/', 'v86/scaffold/')
roles = {
    'wow16/': 'guest, shared ABI or host use requires per-file role review',
    'dos/v86/': 'guest, build tool or shared ABI requires per-file role review',
}
rows = []
evidence = []
seen = set()
for name in inputs:
    path = root / name
    data = path.read_bytes()
    evidence.append({'path': str(path).replace('\\', '/'),
                     'sha256': hashlib.sha256(data).hexdigest().upper()})
    # Decode the bytes already hashed; do not silently mix different snapshots.
    import io
    for source in csv.DictReader(io.StringIO(data.decode('utf-8-sig'), newline='')):
        relative = source['relative_path'].replace('\\', '/')
        key = relative.casefold()
        if not key.endswith(('.c', '.h')):
            continue
        if key in seen:
            raise ValueError('duplicate path in shard inputs: ' + relative)
        seen.add(key)
        prefix = next((p for p in roles if key.startswith(p)), None)
        if prefix:
            scope, reason = 'role-pending', roles[prefix]
        elif key.startswith(tool_paths):
            scope = 'tool-or-scaffold-candidate'
            reason = 'standalone tool, debugger extension or scaffold; verify per-file ownership'
        else:
            scope = 'host-candidate'
            reason = 'host C/H candidate; selected runtime and ownership remain to verify'
        rows.append({'path': relative, 'shard': name,
                     'ledger_status': source[status_fields[name]],
                     'scope': scope, 'scope_limit': reason})
rows.sort(key=lambda x: x['path'].casefold())
out.mkdir(parents=True, exist_ok=True)
with (out / 'inventory.csv').open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, fieldnames=('path', 'shard', 'ledger_status', 'scope', 'scope_limit'))
    writer.writeheader()
    writer.writerows(rows)
summary = {'inputs': evidence, 'rows': len(rows),
           'scope_counts': dict(collections.Counter(x['scope'] for x in rows)),
           'host_candidate_ledger_status_counts': dict(collections.Counter(
               x['ledger_status'] for x in rows if x['scope'] == 'host-candidate')),
           'warning': 'Path-based candidate scope is not a final host/guest exclusion or proof of behavior.'}
(out / 'evidence.json').write_text(json.dumps(summary, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
print(json.dumps(summary, indent=2, ensure_ascii=True))
