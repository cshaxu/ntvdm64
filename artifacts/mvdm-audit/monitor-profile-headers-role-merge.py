"""Reconcile inactive historical monitor and HUNTER header roles."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'ecc093f7b'
PREFIX = 'monitor-profile-headers-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
stem = 'shard-3/monitor-legacy-interfaces'
table = A / (stem + '-observations.json')
observed = json.loads(subprocess.check_output(['python', str(A / (stem + '-observer.py'))]))
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
additional = ('shard-3/video-diagnostics', 'shard-2/dos-host-root-headers')
for name in additional:
    live = json.loads(subprocess.check_output(['python', str(A / (name + '-observer.py'))]))
    assert live == json.loads((A / (name + '-observations.json')).read_text(encoding='utf-8-sig'))
contracts = {
    'softpc.new/host/inc/monsim32.h': ('MONITOR-only guest memory interface',
        'Selected Sim32 memory behavior, flushing and lifetime remain in scope.'),
    'softpc.new/host/inc/nt_hunt.h': ('HUNTER-only debug interface',
        'Selected error handling and debugging functionality remain in scope.'),
    'softpc.new/base/inc/egatrace.h': ('EGA_DUMP historical trace format',
        'Selected EGA/VGA behavior, rendering and debugging remain in scope.'),
    'softpc.new/base/inc/hg_stat.h': ('Non-PROD GISP statistics declarations',
        'Selected CPU and debugging functionality remain in scope.'),
}
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for path, (edge, limits) in contracts.items():
    row, source = ledger[path], frozen[path]
    assert row['status'] in ('pending', 'excluded')
    assert source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    if path.startswith('softpc.new/host/inc/'):
        assert not observed['deps']['matches'][Path(path).name]
    source_table = stem if path.startswith('softpc.new/host/inc/') else additional[0]
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'],
        edition='OpenNT', original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path),
        prior_status='pending', status='excluded', role='historical-profile-header-source-only',
        evidence_table=source_table + '-observations.json', original_edge=edge,
        limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: inactive profile file role, not excluded capability',
        followup_stage='S11-S17'))
for path, edge in {
    'softpc.new/host/inc/host_bop.h': 'Selected BOP host declarations',
    'softpc.new/host/inc/nt_reset.h': 'Selected host reset declarations',
    'softpc.new/host/inc/nt_thred.h': 'Selected host thread declarations',
}.items():
    row, source = ledger[path], frozen[path]
    assert row['status'] in ('pending', 'partial')
    assert source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'],
        edition='OpenNT', original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path),
        prior_status='pending', status='partial', role='selected-native-source-partial',
        evidence_table=additional[1] + '-observations.json', original_edge=edge,
        limits='Guest BOP routing, reset ordering, thread ownership, errors and lifecycle remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: selected declarations do not prove runtime behavior',
        followup_stage='S11-S17'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'monitor-profile-headers-role-review.md',
    'Four inactive diagnostic/profile headers excluded and three selected host declarations partial; capabilities remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=name + '-observations.json', sha256=H['sha'](A / (name + '-observations.json'))) for name in (stem, *additional)],
    host_input_observations=[], scope='Profile-specific file roles only; no capability exclusion')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
