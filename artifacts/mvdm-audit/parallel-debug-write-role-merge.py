"""Reconcile historical DEBUG variant and WRITE input file roles."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit'); BASE = '168ea6b87'; PREFIX = 'parallel-debug-write-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
groups = [('shard-1', 'guest-debug-variant'), ('shard-2', 'win16-write-guest'), ('shard-3', 'dos-skeletons')]
tables = []
for shard, name in groups:
    folder = A / shard; table = folder / (name + '-observations.json')
    assert json.loads(subprocess.check_output(['python', str(folder / (name + '-observer.py'))])) == json.loads(table.read_text(encoding='utf-8'))
    tables.append(dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table)))
contracts = {
    'dos/v86/cmd/debug/sysver.fal': ('shard-1/guest-debug-variant-observations.json', 'Original DOS-I/O guest build variant'),
    'dos/v86/cmd/debug/sysver.tru': ('shard-1/guest-debug-variant-observations.json', 'Original BIOS-I/O guest build variant'),
    'wow16/write/kmake.bat': ('shard-2/win16-write-guest-observations.json', 'Historical guest build/setup script'),
    'wow16/write/state.rst': ('shard-2/win16-write-guest-observations.json', 'Historical editor state'),
    'wow16/write/WRITE.RES': ('shard-2/win16-write-guest-observations.json', 'Historical guest resource build artifact'),
}
graphs = [Path('build/.ninja/msvc-x86-release/worker'), Path('build/.ninja/msvc-x86-release/wow32'),
          Path('build/.ninja/msvc-x64-release/frontend'), Path('build/.ninja/msvc-x64-release/service')]
for graph in graphs:
    content = (graph / 'build.ninja').read_text().replace('$:', ':').replace('\\', '/').lower()
    deps = subprocess.check_output(['ninja', '-C', str(graph), '-t', 'deps'], text=True, encoding='utf-8', errors='replace').replace('\\', '/').lower()
    for path in contracts:
        absolute = (Path('src/mvdm') / path).resolve().as_posix().lower()
        assert absolute not in content and absolute not in deps
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for path, (table, edge) in contracts.items():
    row, source = ledger[path], frozen[path]
    assert row['status'] == 'pending' and source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='excluded',
        role='historical-guest-build-input-only', evidence_table=table, original_edge=edge,
        limits='Guest loading, image provenance, actual feature behavior, errors and lifecycle remain open.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: historical build input or editor-state file role only', followup_stage='S12-S17'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'Five historical DEBUG/WRITE input roles are excluded, not guest functionality.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables,
    host_input_observations=[], scope='Guest build input and editor-state roles only; no runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
