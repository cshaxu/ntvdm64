"""Reconcile selected DOS and host input declaration roles."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit'); BASE = '130184f12'; PREFIX = 'selected-command-input-headers-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
groups = [('shard-2', 'dos-command-dem-abi'), ('shard-3', 'cga-herc-interface'), ('shard-3', 'mouse-host-interface')]
tables = []
for shard, name in groups:
    folder = A / shard; table = folder / (name + '-observations.json')
    assert json.loads(subprocess.check_output(['python', str(folder / (name + '-observer.py'))])) == json.loads(table.read_text(encoding='utf-8'))
    tables.append(dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table)))
contracts = {
    'inc/cmdsvc.h': ('shard-2/dos-command-dem-abi-observations.json', 'Selected original COMMAND service declarations'),
    'dos/dem/dosdef.h': ('shard-2/dos-command-dem-abi-observations.json', 'Selected original DOS error and attribute declarations'),
    'softpc.new/host/inc/nt_cga.h': ('shard-3/cga-herc-interface-observations.json', 'Selected original CGA geometry declarations'),
    'softpc.new/host/inc/nt_mouse.h': ('shard-3/mouse-host-interface-observations.json', 'Selected original mouse and IRQ declarations'),
}
deps = subprocess.check_output(['ninja', '-C', 'build/.ninja/msvc-x86-release/worker', '-t', 'deps'], text=True, encoding='utf-8', errors='replace').replace('\\', '/').lower()
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for path, (table, edge) in contracts.items():
    row, source = ledger[path], frozen[path]; assert row['status'] == 'pending'
    assert source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    assert '/src/mvdm/' + path.lower() in deps
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table, original_edge=edge,
        limits='Real guest dispatch, device behavior, reset, error paths and lifecycle remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: selected declaration and dependencies do not establish guest behavior', followup_stage='S12-S13'))
assert len(proofs) == 4
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'Four selected DOS and host input declaration roles remain partial; guest behavior remains in scope.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables,
    host_input_observations=[], scope='Selected declarations and dependency snapshots only; no guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
