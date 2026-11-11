"""Require every frozen S2 consumer to preserve emitted text and locations.

Use after the audit capture tool has run before and after the restoration.
This validates the exact consumer set, successful compilers, identical commands,
and location-sensitive output fingerprints, not merely a matching file count.
"""
import argparse
import gzip
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('run_root', type=Path)
    args = parser.parse_args()
    run = args.run_root.resolve()
    if not run.is_relative_to(ROOT / 'build/t480-s2'):
        raise SystemExit('require S2 evidence root')
    plan = json.loads((run / 'apply-r1/result.json').read_text(encoding='utf-8'))
    frozen = json.loads(gzip.decompress((ROOT / 'artifacts/mvdm-audit/t480-s1-build-evidence.json.gz').read_bytes()))
    paths = {row['path'] for row in plan['changes']}
    expected = {c['object'] for row in frozen['files'] if row['path'] in paths for c in row['consumers']}
    assert len(expected) == 819 and len(paths) == 30
    assert plan['applied'] and plan['candidate_count'] == 146
    assert plan['dispositions'] == {'restore': 53, 'retain': 93}
    for row in plan['changes']:
        assert row['path'].startswith(('src/mvdm-host/', 'src/non-mvdm/'))
        assert hashlib.sha256((ROOT / row['path']).read_bytes()).hexdigest() == row['after_sha256'], row['path']
    all_rows = {}
    for phase in ('before', 'after'):
        rows = []
        for arch in ('x86', 'x64'):
            rows += json.loads((run / f'pp-{phase}-{arch}/result.json').read_text(encoding='utf-8'))
        assert len(rows) == len(expected), phase
        indexed = {row['object']: row for row in rows}
        assert set(indexed) == expected, phase
        assert all(row['exit_code'] == 0 for row in rows), phase
        all_rows[phase] = indexed
    differences = []
    for key in sorted(expected):
        for field in ('command_sha256', 'semantic_location_sha256', 'nonempty_lines'):
            if all_rows['before'][key][field] != all_rows['after'][key][field]:
                differences.append({'object': key, 'field': field})
    result = {'expected_consumers': len(expected), 'verified_source_files': len(paths),
              'differences': differences, 'passed': not differences}
    destination = run / 'preprocessing-test.json'
    if destination.exists():
        raise SystemExit('do not overwrite test receipt')
    destination.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    assert not differences, differences
    print('PASS 819 exact consumer commands, emitted text and logical locations; 30 source hashes')


if __name__ == '__main__':
    main()
