"""Freeze pending S10 file ownership into nine disjoint logical partitions."""
import csv
import hashlib
import io
import json
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '2bbd67135'


def committed(path):
    return subprocess.check_output(['git', 'show', f'{BASE}:{path.as_posix()}'])


def rows(data):
    return list(csv.DictReader(io.StringIO(data.decode('utf-8-sig'))))


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def main():
    names = ('combined-initial-status.csv', 'frozen-external-audit-rows.csv', 'external-obligation-triage.csv')
    inputs = {n: committed(A / n) for n in names}
    # Never silently partition a ledger that differs from the committed base.
    assert all((A / n).read_bytes() == data for n, data in inputs.items())
    ledger = rows(inputs[names[0]])
    external = {r['id']: r for r in rows(inputs[names[1]])}
    triage = rows(inputs[names[2]])
    assert len(ledger) == 4684 and len(external) == len(triage) == 42963
    assert len({r['relative_path'] for r in ledger}) == len(ledger)
    assert len({r['id'] for r in triage}) == len(triage) and set(external) == {r['id'] for r in triage}
    items = {
        'mvdm': [dict(kind='mvdm', key=r['relative_path'], path=r['relative_path'], original_shard=r['shard'],
                      tier='mvdm-pending') for r in ledger if r['status'] == 'pending'],
        'external': [dict(kind='external', key=r['id'], path=external[r['id']]['path'], original_shard='',
                          tier=r['dependency_tier']) for r in triage if r['disposition'] == 'pending-specific-contract-review'],
    }
    assert len(items['mvdm']) == 1079 and len(items['external']) == 42954
    assignments = []
    counts = {str(i): {'mvdm': 0, 'external': 0} for i in range(1, 10)}
    for kind, group in items.items():
        group.sort(key=lambda r: (r['path'].casefold(), r['key']))
        for index, item in enumerate(group):
            bucket = str(min(8, index * 9 // len(group)) + 1)
            item['bucket'] = bucket
            counts[bucket][kind] += 1
            assignments.append(item)
    assert len(assignments) == 44033
    assert len({(r['kind'], r['key']) for r in assignments}) == len(assignments)
    output = A / 'nine-way-ownership.csv'
    with output.open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, ('kind', 'key', 'path', 'original_shard', 'tier', 'bucket'), lineterminator='\n')
        writer.writeheader()
        writer.writerows(assignments)
    summary = dict(result='pass', base=BASE, rows=len(assignments), counts=counts,
                   inputs={name: sha(data) for name, data in inputs.items()}, ownership_sha256=sha(output.read_bytes()),
                   allocation='Sort pending paths independently by kind, casefolded path and unique key; divide each sorted list into nine contiguous ranks.',
                   scope='Frozen remaining file roles and external source-edition relevance obligations, not functionality acceptance or permission to import NT4 subsystems.')
    (A / 'nine-way-ownership-summary.json').write_text(json.dumps(summary, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(summary, indent=2))


if __name__ == '__main__':
    main()
