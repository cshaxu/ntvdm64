"""Apply the seven bounded NT host dispositions to the S10 ledger."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '147e75626', 'host-tail-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = json.loads((A / 'host-tail-image-observations.json').read_text())
assert runpy.run_path(str(A / 'host-tail-image-observer.py'))['inspect']() == observed
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
contracts = {
    'fprt.c': ('partial', 'selected-native-source-partial',
        'Original PROD printf and redirected fprintf diagnostics reduce to two linked three-byte no-op bodies',
        'Audit production callers and diagnostic expectations; non-PROD debug pipe is not active product output.'),
    'nt_det.c': ('partial', 'selected-native-source-partial',
        'Both original registration bodies link and call the project-owned public RegisterConsoleVDM boundary',
        'Prove frontend identity, initialization errors, teardown and reentry, not only the relocation target.'),
    'nt_ntfun.c': ('partial', 'selected-native-source-partial',
        'Eleven original NT serial IOCTL, line-control and completion functions are present in the image',
        'Prove actual external peer, CTS/XOFF, immediate and pending failures, cancellation and handle lifecycle.'),
    'nt_wcom.c': ('partial', 'selected-native-source-partial',
        'Three original UART entries link and the original WOW32 COM handle and shadow exports exist',
        'Prove callback installation and real guest UART input/output, errors and lifetimes; symbol existence does not prove wiring.'),
    'nt_hunt.c': ('excluded', 'historical-debug-profile-host-source-only',
        'HUNTER trapping UI is disabled and omitted by the selected original NT recipe',
        'Exclude only the debug profile, not guest debug or error capabilities.'),
    'nt_vflop.c': ('excluded', 'historical-alternative-virtual-floppy-source-only',
        'The unselected OS/2-era fixed-path virtual floppy is omitted by NT recipe; original GFI and real-floppy entries are linked',
        'Keep image-backed floppy, actual media behavior and error handling separately open; this excludes only the old alternative.'),
    'nt_yoda.c': ('excluded', 'historical-debug-profile-host-source-only',
        'YODA is disabled, its compiled PROD stub is empty and the PROD startup object has no caller',
        'Exclude only the debugging profile, not guest exception or debugger contracts owned elsewhere.'),
}
proofs = []
for row in observed['sources']:
    status, role, edge, limits = contracts[Path(row['relative_path']).name]
    proofs.append(dict(relative_path=row['relative_path'], shard=ledger[row['relative_path']]['shard'],
        sha256=row['sha256'], edition='OpenNT', original_path=row['original_path'],
        prior_status='pending', status=status, role=role,
        evidence_table='host-tail-image-observations.json', original_edge=edge, limits=limits,
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status=('unmeasurable: original text is retained; behavior and lifecycle coverage are incomplete'
            if status == 'partial' else 'not-applicable: excluded historical profile, not its selected product capability'),
        followup_stage='S11/S12/S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'host-tail-role-review.md',
    'Seven original NT host roles: four selected partial and three bounded historical-profile exclusions; lifecycle and real serial proof remain open.')
observation = dict(result='pass', base=BASE, candidate_count=7,
    source_tables=[dict(path='host-tail-image-observations.json', sha256=H['sha'](A / 'host-tail-image-observations.json'))],
    host_input_observations=[], scope='Source identity and linked entries only, not complete serial, display or frontend behavior')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 7)
