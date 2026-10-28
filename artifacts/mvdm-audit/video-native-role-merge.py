"""Record selected video owners without accepting whole rendering behavior."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '1b506b719'
PREFIX = 'video-native-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = runpy.run_path(str(A / 'video-image-observer.py'))['inspect']()
assert observed == json.loads((A / 'video-image-observations.json').read_text())
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
contracts = {
    'ega_dmmy.c': ('Original selected get_ega_switch_setting returns zero to ega_ipstat0_inb switch-sense field',
                   'Original fixed switch value is not a project success stub. Actual port/adapter initialization and switch-sense observation are not exercised here; no whole hardware/adapter contract is accepted.'),
    'ega_mode.c': ('Original EGA mode selection computes offset/wrap/split/index, publishes update/host paint routines and refreshes',
                   'Two actual functions are image-bound, not all munged modes, transient CGA dimensions, split/wrap, font changes, paint reconfiguration, reset/retirement or failure proof.'),
    'gvi.c': ('Original adapter selection, recalc_screen_params and old-adapter termination before soft-reset initialization',
              'Three actual functions are image-bound. Valid/fallback adapter choice, byte/word/dword/Video7 pitch, reset ordering, host clear/init and repeat retirement remain open.'),
    'vga_vide.c': ('Original VGA DAC/palette, character/dot/scroll, display info and selective video-state save/restore',
                   'Fourteen mapped functions do not prove valid guest palette/font/pixel/scroll, display-information memory layout, state buffer limits or restoration/failure semantics. Invalid requests do not reach these functions.'),
    'video_io.c': ('Original BOP10 validates AH range, disables incompatible stream I/O and selects the original video function table',
                   '908 actual invalid generic requests per package preserve asserted registers/flags/stack; valid entries, stream ownership, display/presentation and lifecycle remain unproved. BOP42 EGA rejection separately sets carry.'),
}
proofs = []
for row in observed['sources']:
    leaf = Path(row['relative_path']).name
    if leaf == 'ega_trcr.c':
        assert ledger[row['relative_path']]['status'] == 'pending'
        continue
    legacy = leaf == 'ga_glue.c'
    if legacy:
        chain = 'Original NT video recipe comments out ga_glue.c; selected CPU40/C_VID read/write initializers resolve nine pointers to original-softpc-cvidc:ev_glue.obj instead of the older a2cpu/2.0 glue'
        limits = 'Physical alternative CPU-interface file only, not read/write/fill/move capability exclusion; selected EVID owners stay partial, including direction, address units, RAM/video provenance, faults and lifecycle. EGA_DUMP diagnostic scope remains pending separately.'
    else:
        chain, limits = contracts[leaf]
    proofs.append(dict(relative_path=row['relative_path'], shard=ledger[row['relative_path']]['shard'], sha256=row['sha256'],
        edition='OpenNT', original_path=row['original_path'], prior_status='pending', status='excluded' if legacy else 'partial',
        role='historical-alternative-cpu-video-interface-source-only' if legacy else 'selected-native-source-partial',
        evidence_table='video-image-observations.json', original_edge=chain, limits=limits, original_text_retained_percent='100',
        estimated_restored_code_percent='', restored_estimate_status=('not-applicable: alternative original CPU/video interface physical file' if legacy else
            'unmeasurable: selected code/initializers and bounded invalid guest requests do not measure whole mode/rendering/branch/lifecycle behavior'),
        followup_stage='S11/S13'))
assert len(proofs) == 6
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'video-native-role-review.md',
    'Five original video integrations partial; legacy CPU/video glue excluded only as an alternative physical interface, with selected EVID behavior retained; diagnostic tracing remains pending. No production/guest image change.')
observation = dict(result='pass', base=BASE, candidate_count=6,
    source_tables=[dict(path='video-image-observations.json', sha256=H['sha'](A / 'video-image-observations.json'))],
    host_input_observations=[], scope='Five bounded integrations and one original CPU-interface file role; no full rendering or lifecycle acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 6)
