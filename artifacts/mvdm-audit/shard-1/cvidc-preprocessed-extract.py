"""Replay exact, hash-bound MSVC line-marker excerpts from retained outputs."""
import argparse
import hashlib
import json
import re
from pathlib import Path

HERE = Path(__file__).resolve().parent
BUILD = Path('build/t473-s10/parallel8-cvidc-preprocess-r1')
BASELINE = json.loads((HERE / 'parallel-cvidc-evidence-correction.json').read_text(encoding='utf-8'))
CASES = {
    'cpu_c.h': ('host_nt_cpu.i', 'softpc.new/base/cvidc/cpu_c.h', 83),
    'pigger_c.h': ('cvidc_accessfn.i', 'softpc.new/base/cvidc/pigger_c.h', 113),
}


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def extract(blob, expected):
    result, current, count = [], False, 0
    for line in blob.splitlines(keepends=True):
        match = re.match(rb'#line\s+\d+\s+"([^"]+)"', line)
        if match:
            path = re.sub(r'[/\\]+', '/', match.group(1).decode('latin1')).lower()
            current = path.endswith('/src/mvdm/' + expected)
            if current:
                result.append(line)
        elif current:
            result.append(line)
            if line.strip():
                count += 1
    return b''.join(result), count


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--check', action='store_true', help='compare with existing excerpts without writing')
    parser.add_argument('--verify-retained', action='store_true', help='verify excerpts without temporary compiler outputs')
    args = parser.parse_args()
    if args.verify_retained:
        manifest = json.loads((HERE / 'cvidc-preprocessed-excerpts.json').read_text(encoding='utf-8'))
        assert manifest['result'] == 'pass' and len(manifest['records']) == len(CASES)
        for record in manifest['records']:
            blob = (HERE / record['excerpt']).read_bytes()
            excerpt, count = extract(blob, record['source'])
            assert excerpt == blob and sha(blob) == record['excerpt_sha256']
            assert count == record['nonblank_lines'] and len(blob) == record['excerpt_bytes']
            assert CASES[Path(record['source']).name][2] == count
        print('PASS', len(manifest['records']), 'retained excerpts')
        return
    hashes = {Path(i['path']).name: i['sha256'] for i in BASELINE['inputs']}
    sources = {i['relative_path']: i for i in BASELINE['sources']}
    records = []
    for name, (unit, relative, expected_count) in CASES.items():
        blob = (BUILD / unit).read_bytes()
        assert sha(blob) == hashes[unit]
        excerpt, count = extract(blob, relative)
        assert count == expected_count and excerpt
        assert sources[relative]['preprocessed_nonblank_lines'][unit] == count
        path = HERE / ('cvidc-preprocessed-' + name + '.txt')
        if args.check:
            assert path.read_bytes() == excerpt
        else:
            path.write_bytes(excerpt)
        records.append(dict(source=relative, unit=unit, unit_sha256=sha(blob),
                            excerpt=path.name, excerpt_sha256=sha(excerpt),
                            nonblank_lines=count, excerpt_bytes=len(excerpt)))
    manifest = (json.dumps(dict(result='pass', records=records,
        scope='Exact preprocessor excerpts only; no runtime behavior claim'), indent=2) + '\n').encode('utf-8')
    path = HERE / 'cvidc-preprocessed-excerpts.json'
    if args.check:
        assert path.read_bytes() == manifest
    else:
        path.write_bytes(manifest)
    print('PASS', len(records), 'hash-bound exact excerpts')


if __name__ == '__main__':
    main()
