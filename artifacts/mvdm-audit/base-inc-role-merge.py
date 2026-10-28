"""Classify five original base headers selected in production preprocessing."""
import csv
import json
import runpy
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
multi = sys.argv[1:] == ['--multi']
if sys.argv[1:] and not multi:
    raise SystemExit('Use no arguments or --multi')
BASE, PREFIX = ('11c1c5abf', 'base-inc-multi-role-') if multi else ('6c0df5292', 'base-inc-role-')
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = 'base-inc-multi-observations.json' if multi else 'base-inc-selection-observations.json'
observer = 'base-inc-multi-observer.py' if multi else 'base-inc-selection-observer.py'
review = 'base-inc-multi-role-review.md' if multi else 'base-inc-role-review.md'
observed = json.loads((A / table).read_text())
assert runpy.run_path(str(A / observer))['inspect']() == observed
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
limits = {
    'bios.h': 'Prove BIOS state layout, callers, reset and guest-visible behavior across consumers.',
    'config.h': 'Prove configuration values, consumer ABI, mutation, error handling and lifetimes.',
    'cpu4.h': 'Prove CPU and hardware-interrupt entry ordering, ABI and failure behavior.',
    'timer.h': 'Prove timer/EOI layout, ownership, reset, concurrency and guest timing.',
    'xt.h': 'Prove shared CPU types and effective-address ABI, callers, fault ordering and lifetime.',
}
if multi:
    groups = {
        'Prove BIOS/RTC state, selected CPU/BOP contracts, mode changes, errors and guest behavior.':
            'cmos.h cntlbop.h cpu.h rtc_bios.h virtual.h',
        'Prove register and plane layout, selected providers, dirty updates, rendering, mode changes and teardown.':
            'cga.h egagraph.h egamode.h egaports.h egaread.h ga_defs.h ga_mark.h gvi.h vgaports.h video.h',
        'Prove drive/media identity, floppy commands, errors and real/virtual media boundaries; declarations do not enable test mode.':
            'fla.h gfi.h gfitest.h',
        'Prove input/IRQ ordering, scan and callback state, errors, reset and cleanup.':
            'host_nls.h keyba.h keyboard.h mouse.h mouse_io.h ppi.h',
        'Prove idle transitions, scheduler/timer ordering, reentry, cancellation and timing.':
            'idetect.h quick_ev.h timestmp.h tmstrobe.h',
        'Prove real serial peer and lifecycle; independently verify diagnostic guards and actual consumers.':
            'rs232.h trace.h',
    }
    limits = {name: boundary for boundary, names in groups.items() for name in names.split()}
    assert len(limits) == 30
proofs = []
for row in observed['sources']:
    name = row['relative_path']
    contexts = ', '.join(s['translation_unit'] for s in row['selections'])
    proofs.append(dict(relative_path=name, shard=ledger[name]['shard'], sha256=row['sha256'],
        edition='OpenNT', original_path=row['original_path'], prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table,
        original_edge='Byte-identical original declarations emitted by selected production preprocessing in ' + contexts,
        limits=limits[Path(name).name], original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: selected declarations do not establish runtime behavior or ABI coverage',
        followup_stage='S11/S12/S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, review,
    f'{len(proofs)} byte-identical original common headers selected by production preprocessing are partial; other contexts and behavioral contracts remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table, sha256=H['sha'](A / table))],
    host_input_observations=[], scope='Selected declaration roles, not full ABI or product behavior')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
