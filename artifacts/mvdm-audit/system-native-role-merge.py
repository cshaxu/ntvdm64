"""Record a complete original no-state leaf and bounded NT system integrations."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'eb218c706'
PREFIX = 'system-native-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = runpy.run_path(str(A / 'system-image-observer.py'))['inspect']()
assert observed == json.loads((A / 'system-image-observations.json').read_text())
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
contracts = {
    'at_dma.c': ('Original two-controller DMA/page-port registration, state and SAS transfer provider', 'Guest DMA status-port access does not prove transfer modes, masks, byte/word channels, decrement/page wrap, LIM ownership, failure and reset/retirement.', 'S13'),
    'cmosnt.c': ('Original selected NT CMOS/RTC provider, IRQ8 gate and callbacks; generic cmos.c is not a second owner', 'Fresh guest valid-RAM and masked periodic-publication checks plus real cached PIC callback fixture are bounded; alarm/update/format/time-change, concurrent register-C consumption and full IRQ8/BOP70 completion remain open.', 'S13'),
    'dummy_nt.c': ('Complete original selected dummy_int leaf: empty body, exact RET 0 code, no relocations/state/resources/callbacks; real BOP01 guest and wrong-oracle controls', 'Complete leaf only in the selected NT/CPU40 profile and canonical caller ABI; not whole interrupt decoding, firmware vectors, PIC, RTC or caller lifecycle.', 'S11-S13'),
    'timer.c': ('Original selected NT PIT modes, counter/latch and TimerEoiHook provider', 'Real guest BIOS ticks and native PIC cancellation assertions do not prove every PIT mode/gate/latch, coalescing, host scheduling overlap, time discontinuity or shutdown failure.', 'S13'),
    'timestrb.c': ('Original NT strobe video-disable threshold and refresh; non-NT timer/RTC work remains conditionally excluded by original code', 'Verify exactly nineteen disabled strobes, one clear, enabled refresh/reset and concurrent teardown; generic rtc_tick must not be wired beside the real NT RtcTick owner.', 'S13'),
    'unexp_nt.c': ('Original unexpected interrupt PIC ISR/mask/EOI and BIOS cause-byte provider', 'No new handler guest witness; verify software/no-ISR, master/slave, cascade preservation, actual callbacks and repeated/abnormal delivery together with D11.', 'S13'),
}
proofs = []
for row in observed['sources']:
    name = row['relative_path']
    chain, limits, stage = contracts[Path(name).name]
    full = name.endswith('/dummy_nt.c')
    proofs.append(dict(relative_path=name, shard=ledger[name]['shard'], sha256=row['sha256'], edition='OpenNT',
        original_path=row['original_path'], prior_status='pending', status='full' if full else 'partial',
        role='selected-native-leaf-full' if full else 'selected-native-source-partial',
        evidence_table='system-image-observations.json', original_edge=chain, limits=limits,
        original_text_retained_percent='100', estimated_restored_code_percent='100' if full else '',
        restored_estimate_status=('measured: complete selected original no-state/no-resource leaf; exact RET 0 and positive/negative real guest proof' if full else
                                 'unmeasurable: selected functions and bounded guest/callback cases do not measure whole device behavior, branches or lifecycle'), followup_stage=stage))
for row in observed['alternatives']:
    cmos = row['relative_path'].endswith('/cmos.c')
    proofs.append(dict(relative_path=row['relative_path'], shard=ledger[row['relative_path']]['shard'], sha256=row['sha256'],
        edition='OpenNT', original_path=row['original_path'], prior_status='pending', status='excluded',
        role='historical-alternative-nt-provider-source-only' if cmos else 'historical-superseded-source-revision-only',
        evidence_table='system-image-observations.json',
        original_edge=('Original NT sources selects cmosnt.c; it owns CMOS ports/RTC directly and intentionally uses static/post state rather than generic central CMOS files' if cmos else
                       'Original NT sources selects ica.c revision 1.38, not preserved ica.old revision 1.31; real current PIC owner and registered host correction remain selected'),
        limits='Physical alternative/revision only; CMOS/RTC/PIC capabilities and the selected owners remain independently accepted or pending. Do not link competing state owners.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: alternative or earlier original provider file role', followup_stage='S13'))
assert len(proofs) == 8
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
full_path = A / 'full-selected-leaf.csv'
full = H['rows'](full_path)
assert len(full) == 26 and all(r['relative_path'] != 'softpc.new/base/system/dummy_nt.c' for r in full)
dummy = next(r for r in observed['sources'] if r['relative_path'].endswith('/dummy_nt.c'))
new = dict.fromkeys(full[0], '')
new.update(relative_path=dummy['relative_path'], source_sha256=dummy['sha256'], classification='full',
    estimated_restored_code_pct='100', scope='Complete selected original no-state/no-resource dummy_int leaf; canonical NT/CPU40 caller ABI',
    guest_phase='BOP01', guest_test='tests/observation/verify-ccpu-sign-flags-guest.ps1 -Workload DummyInt',
    normal_case='system-integration-r5/dummy-baseline and dummy-candidate',
    negative_control='Wrong AX oracle rejected at phase D on both packages', binary_function_bytes='3',
    actual_decoder_targets=hex(dummy['functions'][0]['va']), evidence='system-native-role-review.md; system-image-observations.json',
    not_claimed=contracts['dummy_nt.c'][1])
H['write_csv_like_base'](full_path, full + [new], BASE)
H['apply_dispositions'](proofs, BASE, 'system-native-role-review.md',
    'Complete original dummy_int leaf accepted; five original NT system integrations partial; generic CMOS alternative and prior PIC source revision excluded as physical roles only; no guest or runtime image changes.')
observation = dict(result='pass', base=BASE, candidate_count=8,
    source_tables=[dict(path='system-image-observations.json', sha256=H['sha'](A / 'system-image-observations.json'))],
    host_input_observations=[], scope='One complete no-state leaf, five bounded selected integrations and two original provider-file alternatives; not complete device acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 8)
