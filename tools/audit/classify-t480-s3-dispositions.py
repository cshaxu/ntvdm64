"""Materialize S3's conservative dispositions, not semantic equivalence proof.

Stable IDs and original ranges are from S1. Confirmed edits have separate
compiler/runtime receipts. Other rows explicitly retain the existing boundary;
registration and reachability alone never establish minimality.
"""
import collections
import csv
import gzip
import hashlib
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]
ART = ROOT / 'artifacts/mvdm-audit'
WORK = ROOT / 'build/t480-s3'


def read(path):
    return json.loads(path.read_text(encoding='utf-8-sig'))


def owner(path):
    if '/non-mvdm/' in path:
        return 'USER' if '/ntuser/' in path else 'non-MVDM services and declarations'
    if '/wow32/' in path:
        return 'WOW32'
    if '/vdmredir/' in path:
        return 'redirector'
    if any('/' + part + '/' in path for part in ('dos', 'dpmi32', 'xms.486')):
        return 'DOS COMMAND DPMI XMS'
    return 'SoftPC CPU devices and host'


def main():
    inventory = {r['path']: r for r in csv.DictReader(
        (ART / 't480-s1-files.csv').open(encoding='utf-8-sig'))}
    categories = {r['id']: r['category'] for r in read(ART / 't480-s1-hunk-classification.json')}
    hunks = [h for h in read(ART / 't480-s1-hunks.json') if categories[h['id']] in (
        'product-input-substantive-review', 'contains-existing-exclusion-review-mixed-content')]
    assert len(hunks) == 5812
    evidence = json.loads(gzip.decompress((ART / 't480-s1-build-evidence.json.gz').read_bytes()))
    consumers = {f['path']: [c['object'] for c in f['consumers']] for f in evidence['files']}
    restored = {r['id']: 'expression-object-contract' for r in read(WORK / 'restorations-r1.json')}
    restored.update({r['id']: 'spelling-token-location-contract' for r in read(WORK / 'spelling-restorations.json')['changes']})
    names = sorted({h['path'] for h in hunks})
    raw = subprocess.check_output(['git', 'cat-file', '--batch'], cwd=ROOT,
        input=''.join('d08e0ffc7:' + p + '\n' for p in names).encode())
    sources = {}
    pos = 0
    for path in names:
        end = raw.index(b'\n', pos)
        size = int(raw[pos:end].split()[-1])
        pos = end + 1
        sources[path] = raw[pos:pos + size].decode('latin1').splitlines()
        pos += size + 1
    rows = []
    for h in hunks:
        path = h['path']
        lines = sources[path]
        new = '\n'.join(lines[h['current_start'] - 1:h['current_start'] - 1 + h['current_count']])
        content = re.sub(r'(?m)^\s*#(?:line|error)\b[^\n]*', '', new).strip()
        action = 'retain-no-safe-reversion-proved'
        basis = 'Preserve existing owner boundary; no all-consumer equivalence proof for a further edit.'
        if not content:
            action = 'retain-existing-exclusion-or-layout'
            basis = 'Frozen changed-side slice has only error/line directives or no content; excluded original bodies are outside restoration scope.'
        if h['id'] in restored:
            action = 'restore-reviewed-fragment'
            basis = restored[h['id']]
        if path == 'src/non-mvdm/base/ntos/rtl/time.c':
            action = 'minimize-owner-preserve-exclusions'
            basis = 'Original 1970 conversion owner restored; seven calendar cases. Other selected time behavior and excluded branches unchanged.'
        if path == 'src/non-mvdm/base/win32/client/vdm.c' and h['current_start'] <= 210 and h['current_start'] + h['current_count'] > 38:
            action = 'minimize-owner-preserve-native-width'
            basis = 'Original environment function structure restored; six actual COFF consumers equal; keep OPENNT-HOST-065.'
        row = {k: h[k] for k in ('id', 'path', 'original_start', 'original_count', 'current_start', 'current_count')}
        row.update(owner=owner(path), disposition=action, basis=basis,
            original_sha256=inventory[path]['original_sha256'],
            delivered_source_sha256=hashlib.sha256((ROOT / path).read_bytes()).hexdigest(),
            prior_evidence=inventory[path]['prior_evidence'],
            consumers=consumers[path],
            registration_ids_in_file=sorted(set(re.findall(r'(?:MVDM-HOST-DIV-|OPENNT-HOST-|DIV-)\d+', '\n'.join(lines)))),
            limitation='File-level registrations and consumer references are context, not proof that every retained edit is necessary or globally minimal.')
        assert row['consumers']
        rows.append(row)
    result = dict(task='T480 S3', baseline='d08e0ffc7',
        counts=dict(collections.Counter(r['disposition'] for r in rows)), rows=rows,
        limitation='Conservative finite disposition; retain-no-safe-reversion-proved is not an equivalence PASS or a proof of minimum diff.')
    (ART / 't480-s3-dispositions.json.gz').write_bytes(gzip.compress(
        (json.dumps(result, indent=2) + '\n').encode(), mtime=0))
    print(json.dumps(result['counts'], indent=2))


if __name__ == '__main__':
    main()
