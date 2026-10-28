"""Merge seven bounded disk integrations; retain three guarded profiles pending."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '2d9a08911'
PREFIX = 'disk-native-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = runpy.run_path(str(A / 'disk-image-observer.py'))['inspect']()
assert observed == json.loads((A / 'disk-image-observations.json').read_text())
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
contracts = {
    'diskbios.c': ('BIOS BOP13 -> disk_io -> original 22-slot table -> host fixed-disk services',
                   'Fresh no-media INT13 reset and wrong carry oracle are bounded. Valid geometry, read/write/verify/format, DMA-boundary, status clear and failed host I/O/retirement remain unproved.'),
    'fdisk.c': ('Original task-file byte/word port controller, physical attach, ROM geometry and disk IRQ state machine',
                'Actual 21 mapped functions are selected; full command/status/data sequences, byte/word and page boundaries, deferred IRQ, attach/detach/reset and host transfer failure remain open.'),
    'fla.c': ('Original FLA port registration, controller command/result state, DMA/non-DMA and delayed completion -> GFI',
              'Actual eight selected functions do not prove all conditional non-DMA paths, interrupt/cancel/reset overlap, incomplete command/result phases, buffer lifetime or retirement.'),
    'floppy_i.c': ('BIOS BOP40 -> diskette_io -> 25-slot floppy function table; diskette_int saves registers across original INT15 callback',
                   'Bound image tables are not guest coverage of valid/invalid functions/drives, POST, DMA, seek-status/EOI, nested INT15 host_simulate or failure/teardown.'),
    'floppy.c': ('Original floppy BIOS secondary operations, media geometry, DMA setup and NEC command/result translation',
                 'Actual 33 mapped functions and 25 initializer slots do not prove media-change, all formats/transfers/boundaries, retries, nested waits, command rejection or provider retirement.'),
    'gfi.c': ('Original host configuration -> table publication; ordinary commands dispatch one drive, SPECIFY fans out to all four',
              'Actual-object controlled fixture covers initialized empty slots, all 32 command values and reinitialization; host media calls are recording seams, not real-media, concurrent reconfiguration or actual worker-retirement proof.'),
    'gfi_mpty.c': ('Original absent-drive provider -> seven callback slots, genuine failure results and abnormal recalibration completion',
                   'Actual-object fixture checks four drives/all 32 opcodes, status/canaries and reinitialization. Published-image dispatch selection, BIOS timeout/EOI and provider changes under concurrency remain open; synthetic no-media success must not replace real media.'),
}
proofs = []
for row in observed['sources']:
    if row['disposition'] != 'partial-candidate':
        assert ledger[row['relative_path']]['status'] == 'pending'
        continue
    chain, limits = contracts[Path(row['relative_path']).name]
    proofs.append(dict(relative_path=row['relative_path'], shard=ledger[row['relative_path']]['shard'], sha256=row['sha256'],
                       edition='OpenNT', original_path=row['original_path'], prior_status='pending', status='partial',
                       role='selected-native-source-partial', evidence_table='disk-image-observations.json', original_edge=chain,
                       limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
                       restored_estimate_status='unmeasurable: mapped functions, bounded original-object and no-media guest cases do not measure whole device behavior/branches/lifecycle',
                       followup_stage='S12/S13'))
assert len(proofs) == 7
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'disk-native-role-review.md',
    'Seven original disk source integrations partial with bounded object/guest evidence; three guarded remote/diagnostic profiles remain pending. No production source, guest or runtime image changed.')
observation = dict(result='pass', base=BASE, candidate_count=7,
    source_tables=[dict(path='disk-image-observations.json', sha256=H['sha'](A / 'disk-image-observations.json'))],
    host_input_observations=[], scope='Seven bounded original disk integrations; no whole real-media/controller or lifetime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 7)
