"""Exclude four physical host predecessors through actual original NT owners."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'de734bc9c', 'retired-host-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = runpy.run_path(str(A / 'retired-host-image-observer.py'))['inspect']()
assert observed == json.loads((A / 'retired-host-image-observations.json').read_text())
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
contracts = {
    'nt_term.c': ('Original NT recipe chooses nt_reset, whose host_applClose retains the five core old cleanup operations in order and whose host_terminate/TerminateVDM own final exit',
                  'Exclude this duplicate predecessor file only. Current nt_reset remains partial with registered lifecycle changes; normal/nested/failure/service loss, device cleanup and original return-code semantics remain S11/S13.'),
    'nt_smenu.c': ('Original NT recipe omits the old DosBox modal dialog; selected host sources have no nt_settings_menu/DosDlgProc/bKillFlag consumer, and original/selected resources have no DosBox template',
                   'Exclude this obsolete application-local settings/kill dialog only. Its fullscreen/DGBOX branches are already empty in the original. Current NTSRV-coordinated management, frontend selection and mouse/menu lifecycle remain independent partial obligations; do not resurrect an unused kill flag.'),
    'nt_hfx.c': ('Original NT recipe omits the host HFX stub set; actual original DEM create/open/read/write/close/directory functions and apfnSVC slots supply NT DOS file services',
                 'Exclude the unselected generic HFX stub file, not DOS filesystem capability. No original successful no-op file stub is imported. DEM files/handles/paths/errors/redirected and network cases remain S12 and VDMREDIR owners.'),
    'stf_conf.c': ('Original NT recipe selects config.c instead of the generic SoftPC.rez configuration panel; actual NT runtime query/set and config_inquire providers own the selected configuration API',
                   'Exclude this generic predecessor panel/resource format only, not NT configuration. Runtime settings, PIF/environment/profile handling, device propagation, failure and retirement remain selected partial NT owners; do not claim generic panel persistence was tested or recreate it as a new feature.'),
}
proofs = []
for row in observed['sources']:
    chain, limits = contracts[Path(row['relative_path']).name]
    proofs.append(dict(relative_path=row['relative_path'], shard=ledger[row['relative_path']]['shard'], sha256=row['sha256'],
        edition='OpenNT', original_path=row['original_path'], prior_status='pending', status='excluded',
        role='historical-superseded-nt-host-source-only', evidence_table='retired-host-image-observations.json',
        original_edge=chain, limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: superseded physical predecessor; selected original NT capabilities retain their own partial integration and behavior obligations',
        followup_stage='S11/S12/S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'retired-host-role-review.md',
    'Four unchanged physical host predecessors excluded through original NT recipe/callers, 15 actual selected functions, 73 DEM slots and cleanup order. Their selected capabilities remain partial; no original source or image changed.')
observation = dict(result='pass', base=BASE, candidate_count=4,
    source_tables=[dict(path='retired-host-image-observations.json', sha256=H['sha'](A / 'retired-host-image-observations.json'))],
    host_input_observations=[], scope='Four physical predecessor exclusions, not complete selected files/configuration/menu/termination acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 4)
