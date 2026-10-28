"""Reconcile selected directory, interrupt and device source roles."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'bb3c8f460'
PREFIX = 'directory-interrupt-headers-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
contracts = {
    'inc/curdir.h': ('shard-2/curdir-abi', 'DEM and WOW current-directory declarations'),
    'dpmi32/precomp.h': ('shard-2/dpmi-interrupt-interface', 'DPMI compilation declarations'),
    'inc/intapi.h': ('shard-2/dpmi-interrupt-interface', 'DPMI interrupt-handler declarations'),
    'softpc.new/base/keymouse/mouse.c': ('shard-2/dos-mouse-ppi-device', 'Selected bus mouse port registration'),
    'softpc.new/base/keymouse/ppi.c': ('shard-2/dos-mouse-ppi-device', 'Selected PPI port registration'),
}
tables = []
for stem in sorted({v[0] for v in contracts.values()}):
    table = A / (stem + '-observations.json')
    observed = json.loads(subprocess.check_output(['python', str(A / (stem + '-observer.py'))]))
    assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
    tables.append(dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table)))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for path, (stem, role) in contracts.items():
    row, source = ledger[path], frozen[path]
    assert row['status'] in ('pending', 'partial')
    assert source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'],
        edition='OpenNT', original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path),
        prior_status='pending', status='partial', role='selected-native-source-partial',
        evidence_table=stem + '-observations.json', original_edge=role,
        limits='Runtime guest ABI, port I/O, interrupt nesting, failure recovery and lifecycle remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: source selection does not prove runtime behavior',
        followup_stage='S12-S17'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'Three declarations and two selected port device roles are partial; runtime obligations remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables, host_input_observations=[],
                   scope='Selected declarations and device sources; no runtime or guest ABI acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
