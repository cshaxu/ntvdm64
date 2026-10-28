"""Record seven selected original BIOS integrations as partial, never full."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'e0b7e8a42'
PREFIX = 'bios-native-role-'
helpers = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = runpy.run_path(str(A / 'bios-image-observer.py'))['inspect']()
assert observed == json.loads((A / 'bios-image-observations.json').read_text())
contracts = {
    'bios.c': ('Original 256-entry BIOS dispatch table and ten virtualized mouse wrappers; every initializer target is image-bound', 'Original CCPU BOP register/error witness covers control, illegal and invalid EMS dispatch only. Verify remaining per-entry providers, virtual state ownership, callbacks and nested retirement.', 'S11-S17'),
    'cmos_bis.c': ('Original CMOS read/write port ordering and NMI/index restoration; two real functions linked', 'Verify actual callers, all address/NMI combinations, port failures and RTC/calendar/reset effects. Non-NT set_tod is not the selected NT tick owner.', 'S13'),
    'cntlbop.c': ('Original base/host table lookup and wrong-host rejection; actual selected tables are empty sentinels', '836-case guest includes 768 control calls over all AL values for generic, NT and one wrong host; preserve explicit unsupported errors. Nonselected optional host entries and wider callback routes are not accepted.', 'S11-S12'),
    'emm_fncs.c': ('Original EMS initialization, 30-slot function table and 33 mapped functions; all table targets image-bound', 'Only four invalid-function returns are newly tested. Verify valid INT67/BOP66-68 paths, handles/pages/maps, guest buffer bounds, call/jump unwind, allocation/failure/partial mutation, access keys and reset/retirement.', 'S12'),
    'ill_bop.c': ('Original PROD illegal-BOP no-op; 64 real guest calls preserve defined flags and full register/stack state', 'This intentional original no-op is not a new success stub. Non-PROD diagnostic/environment/YODA behavior and other profile obligations are not accepted.', 'S11-S17'),
    'rom_basc.c': ('Original ROM BASIC unsupported-error report through host_error; real function linked', 'Verify actual error provider, reporting and continue/retirement outcomes. No ROM BASIC interpreter or successful BASIC execution is claimed.', 'S13'),
    'slave_bs.c': ('Original unused-interrupt, IRQ9 redirect and x87 IRQ acknowledgement bodies; three real functions linked', 'Resolve existing D11 slave-port finding with real PIC state, ISR/mask/EOI callbacks, AEOI/no-ISR/master-only paths and actual guest route before any fix or complete acceptance.', 'S13'),
}
ledger = {r['relative_path']: r for r in helpers['rows'](A / 'combined-initial-status.csv')}
proofs = []
for record in observed['sources']:
    path = record['relative_path']
    chain, limits, stage = contracts[Path(path).name]
    proofs.append(dict(relative_path=path, shard=ledger[path]['shard'], sha256=record['source_sha256'],
        edition='OpenNT', original_path=record['original_path'], prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table='bios-image-observations.json', original_edge=chain,
        limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: mapped function bytes, dispatch pointers and selected guest cases do not measure whole-file behavior or branch/lifecycle coverage',
        followup_stage=stage))
assert len(proofs) == 7
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader(); writer.writerows(proofs)
helpers['apply_dispositions'](proofs, BASE, 'bios-native-role-review.md',
    'Seven original BIOS integrations partial: 52 mapped functions, 256 BIOS and 30 EMS pointers bound; two real-package 836-case guest witnesses plus oracle negatives; no full/source/runtime image change.')
observation = dict(result='pass', base=BASE, candidate_count=7,
    source_tables=[dict(path='bios-image-observations.json', sha256=helpers['sha'](A / 'bios-image-observations.json'))],
    host_input_observations=[], scope='Selected original BIOS integration and bounded real guest contracts, not complete BIOS/EMS/device acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
helpers['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 7)
