"""Read-only shard-2 pending inventory and selected DOS host evidence."""
import csv
import hashlib
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
OUT = A / 'shard-2'
BUILD = Path('build/.ninja/msvc-x86-release/worker')
M = Path('src/mvdm')
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm'),
             Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
NATIVE = {
    'dos/command/cmddisp.c': [('_CmdDispatch', 'original-mvdm-command:cmddisp.obj', 'obj/command/cmddisp.obj')],
    'dos/dem/demdasd.c': [('_demAbsRead', 'original-mvdm-dem:demdasd.obj', 'obj/dem/demdasd.obj'),
                          ('_demAbsWrite', 'original-mvdm-dem:demdasd.obj', 'obj/dem/demdasd.obj')],
    'dos/dem/demioctl.c': [('_demIOCTL', 'original-mvdm-dem:demioctl.obj', 'obj/dem/demioctl.obj')],
    'dos/dem/demlock.c': [('_demLockOper', 'original-mvdm-dem:demlock.obj', 'obj/dem/demlock.obj')],
}


def rows(path):
    with Path(path).open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream))


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    ledger = [r for r in rows(A / 'combined-initial-status.csv') if r['shard'] == '2' and r['status'] == 'pending']
    frozen = {r['relative_path']: r for r in rows(A / 'frozen-file-reconciliation.csv')}
    reviewed = {r['relative_path']: r for r in rows(OUT / 'file-audit.csv')}
    assert len(ledger) == 330
    records = []
    for item in ledger:
        path = item['relative_path']
        f, r = frozen[path], reviewed[path]
        actual = []
        for root, field in ((M, 'current_sha256'), (ORIGINALS[0], 'opennt_sha256'), (ORIGINALS[1], 'opennt45_sha256')):
            candidate = root / path
            value = sha(candidate) if candidate.is_file() else ''
            assert value == f[field], (path, field, value, f[field])
            actual.append(value)
        records.append(dict(path=path, hashes=actual, identity=f['current_identity'], origin=f['upstream_origin'],
            dos_catalog_rows=f['dos_catalog_rows'], wow_catalog_rows=f['wow_catalog_rows'],
            prior_chain=r['reviewed_call_chain'], prior_limits=r['review_limits'],
            prior_composition=r['composition_evidence'], prior_tests=r['reviewed_test_evidence']))
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    image = h['Image'](Path('assets/release/ntvdm.exe'))
    syms = h['symbols']((BUILD / 'ntvdm.exe.map').read_text())
    graph = (BUILD / 'build.ninja').read_text().replace('$:', ':')
    bindings = {}
    for path, targets in NATIVE.items():
        assert frozen[path]['current_sha256'] == frozen[path]['opennt_sha256'] == frozen[path]['opennt45_sha256']
        hits = []
        for name, owner, obj in targets:
            edge = next(line for line in graph.splitlines() if line.startswith('build ' + obj + ':'))
            assert (M / path).resolve().as_posix().lower() in edge.lower()
            symbol = next(s for s in syms if s['name'] == name and s['owner'] == owner and s['function'])
            hit = h['bind'](image, h['Coff'](BUILD / obj), symbol)
            hit['object_sha256'] = sha(BUILD / obj)
            hits.append(hit)
        bindings[path] = hits
    return dict(result='pass', base='f472b0926', count=len(records), records=records, native_bindings=bindings,
        inputs=[dict(path=str(p), sha256=sha(p)) for p in (A / 'combined-initial-status.csv', A / 'frozen-file-reconciliation.csv',
            OUT / 'file-audit.csv', BUILD / 'build.ninja', BUILD / 'ntvdm.exe.map', Path('assets/release/ntvdm.exe'))],
        scope='Read-only identity inventory and selected object/image bindings; no runtime or guest behavior claim')


if __name__ == '__main__':
    result = inspect()
    (OUT / 'parallel-review-evidence.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print('pass', len(result['records']), len(result['native_bindings']))
