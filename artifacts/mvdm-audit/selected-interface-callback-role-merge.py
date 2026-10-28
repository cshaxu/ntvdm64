"""Reconcile selected original interfaces, resource and callback source."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'f1b3042c6', 'selected-interface-callback-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
stems = ('shard-2/vdmredir-host-roles', 'shard-2/dos-host-platform-headers',
         'shard-2/dos-video-profile-headers', 'shard-2/wcall32-release')
tables = []
for stem in stems:
    path = A / (stem + '-observations.json')
    live = json.loads(subprocess.check_output(['python', str(A / (stem + '-observer.py'))]))
    assert live == json.loads(path.read_text(encoding='utf-8-sig'))
    tables.append(dict(path=stem + '-observations.json', sha256=H['sha'](path)))
resources = json.loads((A / 'shard-0/vdmredir-resource-observations.json').read_text(encoding='utf-8-sig'))
cached = next(r for r in resources['cached_resources'] if r['path'] == [16, 1, 1033])
release = next(r for r in resources['release_resources'] if r['path'] == [16, 1, 1033])
assert cached['sha256'] == release['sha256']
contracts = {
    'vdmredir/dlstruct.h': (stems[0], 'Selected down-level redirector structures'),
    'vdmredir/vrica.h': (stems[0], 'Selected redirector interrupt declarations'),
    'vdmredir/vrputil.h': (stems[0], 'Selected redirector utility declarations'),
    'vdmredir/vdmredir.rc': (stems[0], 'Selected redirector version resource input'),
    'softpc.new/host/inc/host_inc.h': (stems[1], 'Selected host interface declarations'),
    'softpc.new/host/inc/nt_ega.h': (stems[2], 'Selected host EGA declarations'),
    'wow32/wcall32.c': (stems[3], 'Four original callback bodies bind to released WOW32 code'),
}
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for path, (stem, edge) in contracts.items():
    row, source = ledger[path], frozen[path]
    assert row['status'] in ('pending', 'partial') and row['shard'] == '2'
    assert source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=stem + '-observations.json', original_edge=edge,
        limits='Actual guest invocation, ABI, failures, nesting and lifecycle remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: source selection and samples do not establish full behavior', followup_stage='S13-S17'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'selected-interface-callback-role-review.md',
    'Seven selected interface, resource and callback roles partial; behavior remains open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables,
              host_input_observations=[], scope='Selected inputs and samples; not complete runtime behavior')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
