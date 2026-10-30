"""Verify bounded host-input roles without closing guest/runtime obligations."""
import csv
import hashlib
import json
import pathlib
import subprocess

ROOT = pathlib.Path('artifacts/mvdm-audit')
SCOPE = ROOT / 'host-ch-scope'
TABLES = (
    'dos-pending-host-dispositions.csv',
    'write-pending-host-dispositions.csv',
    'ddeml-mmsystem-host-dispositions.csv',
    'ole-pending-host-dispositions.csv',
    'user-toolhelp-regedit-host-dispositions.csv',
    'remaining-guest-pending-host-dispositions.csv',
)


def read_csv(path):
    with path.open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream))


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def main():
    pending = json.loads((SCOPE / 'pending-scope-breakdown.json').read_text())
    expected = {r['path'] for r in pending['pending_rows']}
    assert len(expected) == pending['pending_count'] == 195
    rows = [row for table in TABLES for row in read_csv(SCOPE / table)]
    paths = [row['path'] for row in rows]
    assert len(paths) == len(set(paths)) and set(paths) == expected
    statuses = {r['relative_path']: r['status'] for r in read_csv(ROOT / 'combined-initial-status.csv')}
    differences = {r['relative_path'] for r in read_csv(ROOT / 'mirror-diff-code-retention.csv')}
    assert not (set(paths) & differences)
    graphs = {}
    for name in ('worker', 'wow32'):
        for action in ('commands', 'deps'):
            data = subprocess.check_output(['ninja', '-C',
                'build/.ninja/msvc-x86-release/' + name, '-t', action])
            graphs[(name, action)] = data.decode(errors='replace').replace('\\', '/').lower()
    for name, action, control in (
        ('worker', 'commands', 'src/mvdm/softpc.new/base/ccpu386/c_xfer.c'),
        ('worker', 'deps', 'src/mvdm/softpc.new/base/inc/sas.h'),
        ('wow32', 'commands', 'src/mvdm/wow32/wow32.c'),
        ('wow32', 'deps', 'src/mvdm/inc/wowfax.h'),
    ):
        assert control in graphs[(name, action)], (name, action, control)
    for row in rows:
        path = row['path']
        assert row['host_source_status'] == 'excluded'
        assert statuses[path] == 'pending'  # Scoped host role is not whole-file closure.
        assert sha256(pathlib.Path('src/mvdm') / path) == row['original_sha256']
        assert sha256(pathlib.Path('O:/repos.external/OpenNT/base/mvdm') / path) == row['original_sha256']
        token = ('src/mvdm/' + path).lower()
        assert all(token not in data for data in graphs.values()), path
    result = {
        'result': 'pass', 'host_scoped_rows': len(rows),
        'mixed_ledger_still_pending': len(rows), 'mirror_difference_rows': 0,
        'tables': {table: sha256(SCOPE / table) for table in TABLES},
        'limitation': 'Selected x86 host input only; no claim for other profiles or runtime behavior.',
    }
    (SCOPE / 'guest-pending-crosswalk-verification.json').write_text(
        json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
