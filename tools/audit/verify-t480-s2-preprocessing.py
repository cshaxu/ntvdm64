"""Hash actual preprocessor text with logical locations for selected T480 consumers.

Launch in the matching VS environment in an owned background runner. Compiler
output is kept in memory per job, not as a multi-gigabyte duplicate source tree.
"""
import argparse
import concurrent.futures
import gzip
import hashlib
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]
NONEMPTY = re.compile(rb'(?m)^[ \t]*[^ \t\r\n][^\r\n]*')
DIRECTIVE = re.compile(rb'^#line\s+(\d+)(?:\s+"([^"]*)")?\s*$')
TOKENS = re.compile(rb'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|[A-Za-z_$][\w$]*|(?:\d[\w.]*|\.[0-9][\w.]*)|>>=|<<=|\.\.\.|\+\+|--|->|&&|\|\||==|!=|<=|>=|<<|>>|\+=|-=|\*=|/=|%=|&=|\^=|\|=|##|[^\s]')


def fingerprint(data, tokens=False):
    result = hashlib.sha256()
    logical, filename, previous, count = 1, b'', 0, 0
    for match in NONEMPTY.finditer(data):
        logical += data.count(b'\n', previous, match.start())
        text = match[0].strip()
        directive = DIRECTIVE.match(text)
        if directive:
            logical = int(directive[1]) - 1
            if directive[2] is not None:
                filename = directive[2]
        else:
            if tokens:
                text = b'\x1f'.join(TOKENS.findall(text))
            result.update(filename + b'\0' + str(logical).encode() + b'\0' + text + b'\n')
            count += 1
        previous = match.end()
    return result.hexdigest(), count


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('plan', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--arch', choices=['x86', 'x64'], required=True)
    args = parser.parse_args()
    output = args.output.resolve()
    if not any(output.is_relative_to(ROOT / ('build/' + task)) for task in ('t480-s2', 't480-s3')) or output.exists():
        raise SystemExit('require fresh output under build/t480-s2 or build/t480-s3')
    output.mkdir(parents=True)
    evidence = json.loads(gzip.decompress((ROOT / 'artifacts/mvdm-audit/t480-s1-build-evidence.json.gz').read_bytes()))
    changed = {v['path'] for v in json.loads(args.plan.read_text(encoding='utf-8'))['changes']}
    keys = {c['object'] for f in evidence['files'] if f['path'] in changed for c in f['consumers']}
    keys = sorted(k for k in keys if evidence['commands'][k]['graph'].startswith('msvc-' + args.arch))

    def capture(key):
        row = evidence['commands'][key]
        command = re.sub(r'(?<!\S)/c(?=\s)', '/E', row['command'], count=1)
        command = re.sub(r'\s+/Fo(?:"[^"]+"|\S+)', '', command).replace('/showIncludes', '')
        run = subprocess.run(command, shell=True, cwd=row['directory'], capture_output=True, timeout=180)
        if run.returncode:
            failure = hashlib.sha256(key.encode()).hexdigest()[:12]
            (output / (failure + '.stdout')).write_bytes(run.stdout)
            (output / (failure + '.stderr')).write_bytes(run.stderr)
            raise RuntimeError('preprocessing failed: ' + key)
        digest, lines = fingerprint(run.stdout)
        token_digest, token_lines = fingerprint(run.stdout, tokens=True)
        assert lines == token_lines
        return {'object': key, 'command_sha256': hashlib.sha256(command.encode()).hexdigest(),
                'semantic_location_sha256': digest, 'nonempty_lines': lines,
                'token_location_sha256': token_digest,
                'exit_code': run.returncode, 'stderr_sha256': hashlib.sha256(run.stderr).hexdigest()}

    records = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
        for row in pool.map(capture, keys):
            records.append(row)
            if len(records) % 50 == 0:
                print(f'{args.arch}: {len(records)}/{len(keys)}', flush=True)
    (output / 'result.json').write_text(json.dumps(records, indent=2) + '\n', encoding='utf-8')
    print(f'PASS {len(records)} actual consumer commands', flush=True)


if __name__ == '__main__':
    main()
