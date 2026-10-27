"""Verify the six-file IOS checkpoint, not whole-provider acceptance."""
import collections
import csv
import hashlib
import io
import json
import subprocess
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'e94f3fb48'


def rows(path):
    return list(csv.DictReader(path.open(encoding='utf-8-sig', newline='')))


def old_rows(path):
    data = subprocess.check_output(['git', 'show', f'{BASE}:{path.as_posix()}'])
    return list(csv.DictReader(io.StringIO(data.decode('utf-8-sig'))))


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def verify_fixture(name, contract, checks, callbacks):
    receipt = json.loads((A / name).read_text(encoding='utf-8-sig'))
    assert receipt['result'] == 'pass' and receipt['contract'] == contract
    assert (receipt['checks'], receipt['callbacks']) == (checks, callbacks)
    assert [(r['kind'], r['exit']) for r in receipt['runs']] == [
        ('build', 0), ('negative-abi-compile', 2), ('fixture', 0)]
    for record in receipt['inputs']:
        assert sha(Path(record['path'])) == record['sha256'], record['path']
    root = Path(receipt['build_root'])
    assert sha(Path(receipt['fixture_path'])) == receipt['fixture_sha256']
    marker = f'{contract.upper()}_OBJECT_CONTRACT checks={checks} failures=0 callbacks={callbacks}'
    assert marker in (root / 'run.stdout.txt').read_text()
    assert 'C2118' in (root / 'negative-abi.stdout.txt').read_text()
    assert not (root / 'build.stderr.txt').read_text().strip()
    assert 'warning C' not in (root / 'build.stdout.txt').read_text()


def main(output):
    assert sha(A / 'frozen-file-reconciliation.csv') == 'F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196'
    frozen = {r['relative_path']: r for r in rows(A / 'frozen-file-reconciliation.csv')}
    dispositions = {r['relative_path']: r for r in rows(A / 'ios-owner-dispositions.csv')}
    expected = {'softpc.new/base/support/ios.c', 'softpc.new/base/inc/ios.h',
                'softpc.new/base/ccpu386/in.c', 'softpc.new/base/ccpu386/in.h',
                'softpc.new/base/ccpu386/out.c', 'softpc.new/base/ccpu386/out.h'}
    assert set(dispositions) == expected
    path = A / 'combined-initial-status.csv'
    old = {r['relative_path']: r for r in old_rows(path)}
    current = {r['relative_path']: r for r in rows(path)}
    assert len(current) == len(old) == len(frozen) == 4684
    assert {p for p in current if current[p] != old[p]} == expected
    for rel, row in dispositions.items():
        assert row['prior_status'] == old[rel]['status'] == 'pending'
        assert row['status'] == 'partial'
        assert current[rel] == dict(old[rel], status='partial')
        assert sha(Path('src/mvdm') / rel) == row['sha256'] == frozen[rel]['current_sha256'] == frozen[rel]['opennt_sha256']
        assert sha(Path(row['original_path'])) == row['sha256']
    specs = [('shard-0/review.csv', 'classification'), ('shard-1/file-audit.csv', 'audit_status'),
             ('shard-2/file-audit.csv', 'decision'), ('shard-3/file-review.csv', 'review_class')]
    translations = {'待核验': 'pending', '整体排除': 'excluded', '部分复通': 'partial', '全复通': 'full', '全部复通': 'full'}
    seen = set()
    for shard, (name, column) in enumerate(specs):
        previous = {r['relative_path']: r for r in old_rows(A / name)}
        for row in rows(A / name):
            rel = row['relative_path']
            assert rel not in seen
            seen.add(rel)
            assert int.from_bytes(hashlib.sha256(rel.encode()).digest()[:8], 'big') % 4 == shard
            assert translations.get(row[column], row[column]) == current[rel]['status']
            if rel not in expected:
                assert row == previous[rel]
    assert seen == set(current)
    verify_fixture('ios-owner-fixture-result.json', 'ios', 70, 78)
    verify_fixture('ios-owner-qev-control-result.json', 'qevnt', 37, 8)
    assert 'C2118' in (A / 'ios-owner-abi-negative.txt').read_text()
    metrics = rows(A / 'ios-owner-code-retention.csv')
    assert {r['relative_path'] for r in metrics} == expected
    for row in metrics:
        assert row['estimated_restored_code_pct'] == '100.00'
        assert row['retained_original_code_lines'] == row['original_code_lines']
        assert row['source_sha256'] == row['original_sha256'] == dispositions[row['relative_path']]['sha256']
    counts = dict(collections.Counter(r['status'] for r in current.values()))
    assert counts == dict(pending=2139, excluded=2257, partial=236, full=52)
    summary = json.loads((A / 'initial-summary.json').read_text())
    assert summary['counts'] == counts and summary['complete'] is False
    graph = Path('build/.ninja/msvc-x86-release/worker/build.ninja').read_text()
    for obj, source, rule in [('support/ios', 'support/ios', 'cc'), ('ccpu/in', 'ccpu386/in', 'cc_ccpu'), ('ccpu/out', 'ccpu386/out', 'cc_ccpu')]:
        line = next(line for line in graph.splitlines() if line.startswith(f'build obj/{obj}.obj: {rule} '))
        assert f'/base/{source}.c' in line
    assert '/DSFELLOW' not in graph and '-DSFELLOW' not in graph
    for leaf in ['in', 'out']:
        source = Path(f'src/mvdm/softpc.new/base/ccpu386/{leaf}.c').read_text()
        assert '#ifdef SFELLOW' in source and f'{leaf.upper()}16' in source
    drift = []
    for rel, row in frozen.items():
        source = Path('src/mvdm') / rel
        if source.exists():
            current_hash = sha(source)
            if current_hash != row['current_sha256']:
                drift.append(dict(relative_path=rel, frozen=row['current_sha256'], current=current_hash))
            if current[rel]['status'] == 'full':
                assert current_hash == row['current_sha256']
    assert drift == summary['live_source_hash_drift']
    result = dict(result='pass', base=BASE, counts=counts, disposition_changes=6,
                  ios_checks=70, qev_control_checks=37, wrong_width_abi_rejected=True,
                  scope='Three actual cached objects with controlled CPU boundary; not guest privilege/device lifecycle or published-image acceptance')
    output.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result))


if __name__ == '__main__':
    main(Path(sys.argv[1]))
