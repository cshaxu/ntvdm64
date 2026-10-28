"""Reconcile historical guest inputs without excluding their capabilities."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '426b1ffbd'
PREFIX = 'parallel-ddeml-wfwnet-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
groups = [('shard-1', 'guest-ddeml-tests'), ('shard-2', 'win16-wfwnet-guest'), ('shard-3', 'dos-messages')]
tables = []
for shard, name in groups:
    folder = A / shard; table = folder / (name + '-observations.json')
    actual = json.loads(subprocess.check_output(['python', str(folder / (name + '-observer.py'))]))
    assert actual == json.loads(table.read_text(encoding='utf-8'))
    tables.append(dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table)))
paths = {
    **{p: ('shard-1/guest-ddeml-tests-observations.json', 'Historical DDEML guest import or test build input') for p in (
        'wow16/ddeml/STRIPPED.DEF', 'wow16/ddeml/tests/ddestrs/ddestrs.c', 'wow16/ddeml/tests/ddestrs/ddestrs.rc',
        'wow16/ddeml/tests/ddestrs/globals.h', 'wow16/ddeml/tests/ddestrs/mk4dos.cmd', 'wow16/ddeml/tests/src/client/ddemlcl.h')},
    **{p: ('shard-2/win16-wfwnet-guest-observations.json', 'Historical WFWNET guest build input') for p in (
        'wow16/wfwnet/bseerr.h', 'wow16/wfwnet/libentry.asm', 'wow16/wfwnet/wfwnet.c',
        'wow16/wfwnet/wfwnet.rcv', 'wow16/wfwnet/WFWNET.RES')},
    'dos/v86/messages/state.rst': ('shard-3/dos-messages-observations.json', 'Historical editor state, not a message catalog'),
}
assert len(paths) == 12
graphs = [Path('build/.ninja/msvc-x86-release/worker'), Path('build/.ninja/msvc-x86-release/wow32'),
          Path('build/.ninja/msvc-x64-release/frontend'), Path('build/.ninja/msvc-x64-release/service')]
for graph in graphs:
    content = (graph / 'build.ninja').read_text().replace('$:', ':').replace('\\', '/').lower()
    deps = subprocess.check_output(['ninja', '-C', str(graph), '-t', 'deps'], text=True, encoding='utf-8', errors='replace').replace('\\', '/').lower()
    for path in paths:
        absolute = (Path('src/mvdm') / path).resolve().as_posix().lower()
        assert absolute not in content and absolute not in deps
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for path, (table, edge) in paths.items():
    row, source = ledger[path], frozen[path]
    assert row['status'] == 'pending'
    assert source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='excluded',
        role='historical-guest-build-input-only', evidence_table=table, original_edge=edge,
        limits='Guest feature, image provenance, callbacks, failures and lifecycle remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: historical input or editor-state file role only', followup_stage='S12-S17'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'Twelve historical guest input or editor-state file roles are excluded, not their guest features.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables,
    host_input_observations=[], scope='Guest input and editor-state file roles; no guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
