"""Reconcile individual historical DDEML test/sample input roles."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'feae187ad'
PREFIX = 'shard0-ddeml-test-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/ddeml-test-input-observations.json'
observed = json.loads(table.read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-0/ddeml-test-input-observer.py'))['inspect']() == observed
assert len(observed['sources']) == 14
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for source in observed['sources']:
    path = source['relative_path']
    row = ledger[path]
    assert row['status'] == 'pending' and row['shard'] == '0'
    recipe = path.endswith('/mk4dos.cmd')
    group = 'DDESTRS stress application' if '/ddestrs/' in path else 'DDEML client/server or DDEMO sample'
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='excluded',
        role='historical-build-recipe-file-only' if recipe else 'historical-guest-build-source-file-only',
        evidence_table=table.relative_to(A).as_posix(), original_edge='Original independent ' + group + ' build input',
        limits='Physical test/sample file role only; retain DDE capability, guest execution, conversations, callbacks, teardown and future tests.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: historical independent test/sample build input, no runtime capability excluded', followup_stage='S15/S16'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/ddeml-test-input-review.md',
    'Fourteen historical DDEML test/sample input roles are excluded only as product owners; DDE behavior and future tests remain in scope.')
host = []
for name in ('worker', 'wow32'):
    graph = A / ('shard-0/ddeml-inputs-' + name + '.txt')
    live = Path('build/.ninja/msvc-x86-release') / name / 'build.ninja'
    expected = '\n'.join(line for line in live.read_text(encoding='utf-8').splitlines() if line.startswith('build ')) + '\n'
    assert graph.read_text(encoding='utf-8') == expected
    assert H['sha'](live) == next(v['sha256'] for v in observed['graph_inputs'] if Path(v['path']) == live)
    host.append(dict(path=str(graph), sha256=H['sha'](graph), graph=name, kind='build edges'))
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=host, scope='Historical test/sample file roles only; no DDE capability or behavior exclusion')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
