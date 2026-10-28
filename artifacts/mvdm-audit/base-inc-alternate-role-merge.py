"""Record bounded alternative-host header dispositions."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'cb592628d', 'base-inc-alternate-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = json.loads((A / 'base-inc-alternate-observations.json').read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'base-inc-alternate-observer.py'))['inspect']() == observed
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
contracts = {
    'host_fio.h': ('Unix host file I/O interface', 'DOS file I/O through DEM remains separately open.'),
    'unix_asy.h': ('Unix asynchronous host interface', 'NT asynchronous operations and lifecycle remain open.'),
    'unix_cnf.h': ('Unix host configuration interface', 'Selected NT configuration behavior remains open.'),
    'unix_loc.h': ('Unix host local definitions', 'Selected NT host dependencies remain open.'),
    'swinhelp.h': ('SoftWindows HyperHelp interface', 'Guest help remains separately in scope.'),
    'swinmgre.h': ('SoftWindows CPU management interface', 'Selected NT CPU execution remains separately in scope.'),
}
proofs = []
for row in observed['sources']:
    edge, limits = contracts[Path(row['relative_path']).name]
    proofs.append(dict(relative_path=row['relative_path'], shard=ledger[row['relative_path']]['shard'],
        sha256=row['sha256'], edition='OpenNT', original_path=row['original_path'],
        prior_status='pending', status='excluded', role='historical-alternate-host-header-source-only',
        evidence_table='base-inc-alternate-observations.json', original_edge=edge, limits=limits,
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: historical alternate host interface only; selected NT capabilities remain open',
        followup_stage='S11/S12/S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'base-inc-alternate-role-review.md',
    'Six unselected alternate host interfaces; selected NT capabilities remain open.')
observation = dict(result='pass', base=BASE, candidate_count=6,
    source_tables=[dict(path='base-inc-alternate-observations.json', sha256=H['sha'](A / 'base-inc-alternate-observations.json'))],
    host_input_observations=[], scope='Alternate interfaces only; no runtime behavior claim')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 6)
