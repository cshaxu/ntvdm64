"""Check current source mirror differences against the S10 disposition ledger."""
import collections
import csv
import hashlib
import json
from pathlib import Path

A = Path('artifacts/mvdm-audit')
with (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig', newline='') as f:
    frozen = {r['relative_path']: r for r in csv.DictReader(f)}
with (A / 'combined-initial-status.csv').open(encoding='utf-8-sig', newline='') as f:
    ledger = list(csv.DictReader(f))
counts = collections.Counter()
differences = []
drift = []
for row in ledger:
    path = row['relative_path']
    recorded = frozen[path]
    source = Path('src/mvdm') / path
    originals = [recorded['opennt_sha256'], recorded['opennt45_sha256']]
    if not source.is_file() or not any(originals):
        continue
    current = hashlib.sha256(source.read_bytes()).hexdigest().upper()
    if current != recorded['current_sha256']:
        drift.append(path)
    if current not in originals:
        counts[row['status']] += 1
        differences.append({'path': path, 'status': row['status'], 'current_sha256': current})
print(json.dumps({'counts': dict(counts), 'differences': differences, 'current_source_drift': drift,
                  'scope': 'Current mirror bytes versus frozen original hashes; excludes absent/current-only files and does not prove functional coverage'}, indent=2))
