"""Check the three-file quick-event owner disposition and bounded fixture."""
import collections
import csv
import hashlib
import io
import json
import subprocess
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '308d2e0b7'


def rows(path):
    return list(csv.DictReader(path.open(encoding='utf-8-sig', newline='')))


def old_rows(path):
    data = subprocess.check_output(['git', 'show', f'{BASE}:{path.as_posix()}'])
    return list(csv.DictReader(io.StringIO(data.decode('utf-8-sig'))))


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def main(output):
    assert sha(A / 'frozen-file-reconciliation.csv') == 'F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196'
    frozen = {r['relative_path']: r for r in rows(A / 'frozen-file-reconciliation.csv')}
    dispositions = {r['relative_path']: r for r in rows(A / 'quick-event-owner-dispositions.csv')}
    expected = {'softpc.new/base/system/quick_ev.c': 'excluded',
                'softpc.new/base/system/qevnt.c': 'partial',
                'softpc.new/host/inc/host_qev.h': 'partial'}
    assert {p: r['status'] for p, r in dispositions.items()} == expected
    path = A / 'combined-initial-status.csv'
    old = {r['relative_path']: r for r in old_rows(path)}
    current = {r['relative_path']: r for r in rows(path)}
    assert len(current) == len(old) == 4684
    assert {p for p in current if current[p] != old[p]} == set(expected)
    for rel, row in dispositions.items():
        assert old[rel]['status'] == 'pending' and current[rel] == dict(old[rel], status=row['status'])
        assert sha(Path('src/mvdm') / rel) == row['sha256'] == frozen[rel]['current_sha256'] == frozen[rel]['opennt_sha256']
        assert sha(Path(row['original_path'])) == row['sha256']
        assert int.from_bytes(hashlib.sha256(rel.encode()).digest()[:8], 'big') % 4 == int(current[rel]['shard'])
    specs = [('shard-0/review.csv', 'classification'), ('shard-1/file-audit.csv', 'audit_status'),
             ('shard-2/file-audit.csv', 'decision'), ('shard-3/file-review.csv', 'review_class')]
    translations = {'待核验': 'pending', '整体排除': 'excluded', '部分复通': 'partial', '全复通': 'full', '全部复通': 'full'}
    for name, column in specs:
        previous = {r['relative_path']: r for r in old_rows(A / name)}
        for row in rows(A / name):
            rel = row['relative_path']
            assert translations.get(row[column], row[column]) == current[rel]['status']
            if rel not in expected: assert row == previous[rel]
    receipt = json.loads((A / 'quick-event-owner-fixture-result.json').read_text(encoding='utf-8-sig'))
    assert receipt['result'] == 'pass' and receipt['checks'] == 37 and receipt['callbacks'] == 8
    assert [(r['kind'], r['exit']) for r in receipt['runs']] == [('build', 0), ('negative-abi-compile', 2), ('fixture', 0)]
    for record in receipt['inputs']: assert sha(Path(record['path'])) == record['sha256']
    root = Path('build/t473-s10/qevnt-object-contract-r3')
    assert sha(root / 'qevnt-object-contract.exe') == receipt['fixture_sha256']
    assert 'QEVNT_OBJECT_CONTRACT checks=37 failures=0 callbacks=8' in (root / 'run.stdout.txt').read_text()
    assert 'C2118' in (A / 'quick-event-owner-abi-negative.txt').read_text()
    metrics = rows(A / 'quick-event-owner-code-retention.csv')
    assert {r['relative_path'] for r in metrics} == {p for p, status in expected.items() if status == 'partial'}
    for row in metrics:
        assert row['estimated_restored_code_pct'] == '100.00'
        assert row['retained_original_code_lines'] == row['original_code_lines']
        assert row['source_sha256'] == dispositions[row['relative_path']]['sha256'] == row['original_sha256']
    counts = dict(collections.Counter(r['status'] for r in current.values()))
    assert counts == dict(pending=2145, excluded=2257, partial=230, full=52)
    summary = json.loads((A / 'initial-summary.json').read_text())
    assert summary['counts'] == counts and summary['complete'] is False
    manifest = Path('src/mvdm/softpc.new/base/system/sources')
    assert sha(manifest) == frozen['softpc.new/base/system/sources']['current_sha256']
    assert manifest.read_bytes() == Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/base/system/sources').read_bytes()
    text = manifest.read_text()
    assert 'qevnt.c' in text and 'quick_ev.c' not in text
    graph = Path('build/.ninja/msvc-x86-release/worker/build.ninja').read_text()
    assert 'build obj/system/qevnt.obj: cc ' in graph and '/system/qevnt.c' in graph
    assert '/system/quick_ev.c' not in graph
    drift = []
    for rel, row in frozen.items():
        source = Path('src/mvdm') / rel
        if source.exists():
            current_hash = sha(source)
            if current_hash != row['current_sha256']:
                drift.append(dict(relative_path=rel, frozen=row['current_sha256'], current=current_hash))
    assert drift == summary['live_source_hash_drift']
    result = dict(result='pass', base=BASE, counts=counts, disposition_changes=3,
                  fixture_checks=37, wrong_width_abi_rejected=True,
                  scope='Actual cached object under controlled host boundaries; not guest/device clock, full provider or published-image acceptance')
    output.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result))


if __name__ == '__main__':
    main(Path(sys.argv[1]))
