"""Verify all actual consumers for the six S5 same-line comment restorations."""
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
    assert run.is_relative_to(ROOT / 'build/t480-s5')
    def read(name):
        return json.loads((run / name).read_text(encoding='utf-8-sig'))
    plan = read('restorations.json')
    paths = {r['path'] for r in plan['changes']}
    assert len(paths) == len(plan['changes']) == 6
    frozen = json.loads(gzip.decompress((ROOT / 'artifacts/mvdm-audit/t480-s1-build-evidence.json.gz').read_bytes()))
    expected = {c['object'] for f in frozen['files'] if f['path'] in paths for c in f['consumers']}
    phases = {}
    for phase in ('before', 'after'):
        rows = sum([read(f'pp-{phase}-{arch}/result.json') for arch in ('x86', 'x64')], [])
        assert len(rows) == len(expected) and {r['object'] for r in rows} == expected
        phases[phase] = {r['object']: r for r in rows}
    for key in expected:
        a, b = phases['before'][key], phases['after'][key]
        assert a['exit_code'] == b['exit_code'] == 0
        for field in ('command_sha256', 'semantic_location_sha256', 'token_location_sha256', 'nonempty_lines'):
            assert a[field] == b[field], (key, field)
    for row in plan['changes']:
        assert hashlib.sha256((ROOT / row['path']).read_bytes()).hexdigest() == row['after_sha256']
    result = dict(passed=True, files=6, consumers=len(expected),
                  contract='Identical complete emitted text, tokens and logical locations for every frozen actual consumer')
    target = run / 'equivalence.json'
    assert not target.exists()
    target.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print('PASS', result)


if __name__ == '__main__':
    main()
