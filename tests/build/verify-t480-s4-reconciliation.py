"""Reconcile T480 S2/S3 immutable receipts with their revisions and current files.

Read-only acceptance audit. No compiler, guest, runtime or release mutation.
Use a fresh --output below build/t480-s4 through the background supervisor.
"""
import argparse
import gzip
import hashlib
import json
from pathlib import Path
import re
import subprocess
import zipfile

ROOT = Path(__file__).resolve().parents[2]
ART = ROOT / 'artifacts/mvdm-audit'


def digest(data):
    return hashlib.sha256(data).hexdigest()


def read(path):
    return json.loads(path.read_text(encoding='utf-8-sig'))


def blob(revision, path):
    return subprocess.check_output(['git', 'show', revision + ':' + path], cwd=ROOT)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    out = args.output.resolve()
    assert out.is_relative_to(ROOT / 'build/t480-s4') and not out.exists()
    out.mkdir(parents=True)
    s2 = read(ART / 't480-s2-restorations.json')
    deliveries = {s: read(ART / f't480-{s}-delivery.json') for s in ('s2', 's3')}
    archives = {}
    for stage, delivery in deliveries.items():
        path = ROOT / delivery['archive']['path']
        assert digest(path.read_bytes()) == delivery['archive']['sha256']
        archives[stage] = zipfile.ZipFile(path)
        assert len(archives[stage].namelist()) == delivery['archive']['files']
        assert all(r['result']['exitCode'] == 0 for r in delivery['runs'].values())
        assert len(delivery['full']['Gates']) == 16
        assert all(g['Passed'] for g in delivery['full']['Gates'])
    def receipt(stage, name):
        return json.loads(archives[stage].read(name).decode('utf-8-sig'))
    paths2 = {r['path']: r for r in s2['changes']}
    assert len(paths2) == 30
    for path, row in paths2.items():
        assert digest(blob('b35effafd', path)) == row['after_sha256'], path
    paths3 = {r['path']: r for r in receipt('s3', 'restorations-r1.json')}
    paths3.update({r['path']: r for r in receipt('s3', 'spelling-restorations.json')['files']})
    for name in ('time-restoration.json', 'vdm-restoration.json'):
        row = receipt('s3', name)
        paths3[row['path']] = row
    correction = receipt('s3', 'vdm-line-ending-correction.json')
    environment = 'src/non-mvdm/base/win32/client/vdm.c'
    assert paths3[environment]['after_sha256'] == correction['before_sha256']
    paths3[environment]['after_sha256'] = correction['after_sha256']
    assert len(paths3) == 13
    for path, row in paths3.items():
        assert digest(blob('ae65e175b', path)) == row['before_sha256'], path
        assert digest(blob('511f23fc0', path)) == row['after_sha256'], path
    frozen = json.loads(gzip.decompress((ART / 't480-s1-build-evidence.json.gz').read_bytes()))
    consumers = {f['path']: [c['object'] for c in f['consumers']] for f in frozen['files']}
    expected = {c for p in paths2 for c in consumers[p]}
    assert len(expected) == 819
    phases = {}
    for phase in ('before', 'after'):
        rows = sum([receipt('s2', f'pp-{phase}-{arch}/result.json') for arch in ('x86', 'x64')], [])
        assert len(rows) == len(expected) and {r['object'] for r in rows} == expected
        phases[phase] = {r['object']: r for r in rows}
    for key in expected:
        a, b = phases['before'][key], phases['after'][key]
        assert a['exit_code'] == b['exit_code'] == 0
        for field in ('command_sha256', 'semantic_location_sha256', 'nonempty_lines'):
            assert a[field] == b[field], (key, field)
    for arch in ('x86', 'x64'):
        rows = receipt('s3', f'vdm-final-{arch}/result.json')
        assert len(rows) == 3 and all(r['source_sha256'] == correction['after_sha256'] for r in rows)
    time = receipt('s3', 'time-replay-r1/result.json')
    assert all(s['exit_code'] == 0 for s in time['steps'])
    entries = []
    for path in sorted(paths2.keys() | paths3.keys()):
        final = paths3.get(path, paths2.get(path))['after_sha256']
        assert digest((ROOT / path).read_bytes()) == final, path
        assert consumers[path]
        root = ROOT / '/'.join(path.split('/')[:2])
        registry = root / 'README.md'
        source = (ROOT / path).read_text(encoding='latin1')
        refs = [{'line': n, 'text': line} for n, line in enumerate(registry.read_text(encoding='utf-8-sig').splitlines(), 1)
                if Path(path).name in line]
        s3kind = ('environment-structure-with-native-width-retained' if path == environment else
                  'original-rtl-time-owner' if path.endswith('/rtl/time.c') else
                  'proved-expression-or-spelling') if path in paths3 else 'S2-whitespace-only'
        entries.append(dict(path=path, stages=[s for s, paths in [('S2', paths2), ('S3', paths3)] if path in paths],
            current_sha256=final, current_revision='511f23fc0',
            S2_hash=paths2.get(path, {}).get('after_sha256'),
            disposition=s3kind, registration_policy='Existing behavioral registrations retained; only the measured restored fragment is narrowed.',
            inline_ids=sorted(set(re.findall(r'(?:MVDM-HOST-DIV-|OPENNT-HOST-|DIV-)\d+', source))),
            registry=registry.relative_to(ROOT).as_posix(), registry_references=refs,
            consumers=consumers[path]))
    manifest = read(ROOT / 'assets/release/release-manifest.json')
    published = deliveries['s3']['publication']['images']
    assert len(manifest) == len(published) == 11
    assert {r['Name']: r['Sha256'].lower() for r in manifest} == {r['Name']: r['Sha256'].lower() for r in published}
    for row in manifest:
        assert digest((ROOT / 'assets/release' / row['Name']).read_bytes()) == row['Sha256'].lower()
    # Product, guest, fixtures and earlier receipts must remain byte-identical.
    unchanged = ['src', 'assets', 'test', 'tests/build/verify-t480-s2-preprocessor-equivalence.py',
                 'tests/build/verify-t480-s3-environment.py', 'tests/build/verify-t480-s3-time.py']
    changed = subprocess.check_output(['git', 'diff', 'b754ee0ea', '--name-only', '--', *unchanged], cwd=ROOT, text=True).splitlines()
    assert all(p in ('src/mvdm-host/README.md', 'src/non-mvdm/README.md') for p in changed), changed
    result = dict(passed=True, task='T480 S4', files=len(entries), overlap=len(paths2.keys() & paths3.keys()),
        S2_consumers=819, final_environment_consumers=6, release_images=11, rows=entries,
        archives={s: d['archive'] for s, d in deliveries.items()},
        limits=['Registry filename matches are context, not proof of every retained adaptation.',
                'S2 replay requires its S2 source revision; S3 superseding hashes are explicit.',
                'This verifies historical evidence and current identity, not a new runtime pass.'])
    (out / 'reconciliation.json').write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print(f'PASS {len(entries)} host files; {result["overlap"]} overlaps; 819 historical consumers; six final environment identities; eleven release hashes')


if __name__ == '__main__':
    main()
