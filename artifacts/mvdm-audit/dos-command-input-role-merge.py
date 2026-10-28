"""Reconcile only source-proved DOS command guest build-input file roles."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'bba17e229'
PREFIX = 'dos-command-input-role-'
helpers = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = runpy.run_path(str(A / 'dos-command-input-observer.py'))['observe']()
assert observed == json.loads((A / 'dos-command-input-observations.json').read_text())
assert observed['reviewed_count'] == 85 and observed['source_chain_count'] == 78 and observed['resolved_role_count'] == 85
proofs = []
for row in observed['records']:
    if row['result'] == 'unresolved':
        continue
    role = {'source-chain-observed': 'historical-guest-build-source-file-only',
            'message-tool-input-observed': 'historical-guest-build-tool-source-only',
            'obsolete-source-observed': 'historical-obsolete-guest-source-only',
            'source-history-observed': 'historical-source-note-only'}[row['result']]
    proofs.append(dict(relative_path=row['relative_path'], shard=row['shard'], sha256=row['sha256'],
        edition=row['original_edition'], original_path=row['original_path'], prior_status='pending',
        status='excluded', role=role,
        evidence_table='dos-command-input-observations.json', original_edge=' -> '.join(row['chain']),
        limits='Only this historical guest-build source role is excluded from host implementation inputs. Retain branch conditions, immutable guest/image provenance and DOS/COMMAND/utility host-boundary acceptance in S12-S17.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: historical guest build-input file role; no guest feature or runtime capability excluded',
        followup_stage='S12-S17'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
helpers['apply_dispositions'](proofs, BASE, 'dos-command-input-review.md',
    'All 85 remaining DOS command source roles reconciled: 78 source/include inputs, two historical message-tool inputs, four source-proved obsolete modules/message bindings and one change-history note. Guest functionality and image provenance remain independent obligations.')
prior = json.loads((A / 'ccpu-header-macro-role-observations.json').read_text())
host_inputs = [r for r in prior['host_input_observations'] if r['graph'] == 'wow32']
for name in ('commands', 'deps'):
    path = A / 'ccpu-remaining-header-source' / (name + '.stdout.txt')
    host_inputs.append(dict(path=str(path), sha256=helpers['sha'](path), graph='worker', kind=name))
observation = dict(result='pass', base=BASE, candidate_count=85,
    source_tables=[dict(path='dos-command-input-observations.json', sha256=helpers['sha'](A / 'dos-command-input-observations.json'))],
    host_input_observations=host_inputs, scope='Original source-input ownership, not guest image derivation or runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
helpers['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 85)
