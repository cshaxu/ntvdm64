"""Merge bounded original input/NLS owners and retired generic table roles."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '010952cdd'
PREFIX = 'input-native-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = runpy.run_path(str(A / 'input-image-observer.py'))['inspect']()
assert observed == json.loads((A / 'input-image-observations.json').read_text())
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
contracts = {
    'nt_input.c': ('Original KEYBDFUNCS publication -> nt_kb_prepare or nt_kb_light_on -> SyncToggleKeys; ToggleKeyState authority stays in nt_event',
                   'Actual-object 2048 initial-state/pattern combinations prove bounded LED policy with a recording callback. Real queue synchronization, guest device/BIOS state, nested input, reset and retirement remain open; four empty table callbacks are original, not new project success stubs.'),
    'nt_keycd.c': ('Original normal/enhanced set-1 tables -> KeyMsgToKeyCode -> nt_event key up/down; BiosKeyToInputRecord -> public OEM/layout services',
                   'Controlled source-object one-past reads at normal 133/enhanced 84 are established and remain unfixed S13 work. Other scan selection and eight BIOS cases pass, not complete mapping/OEM/DBCS/Alt-numpad/host failure/guest input lifecycle proof.'),
    'nt_nls.c': ('Original nls_init/host_nls_get_msg -> production string resources and public USER/system formatting; host_error supplies positive EHS_MSG_LEN',
                 'Actual object/resource fixture checks initialization and capacities 1..260 for present/missing strings. Missing initialization resources/noncontinuable failure, locales and real error/dialog/worker lifecycle remain untested; no zero/negative-capacity caller is established.'),
}
proofs = []
for row in observed['sources']:
    leaf = Path(row['relative_path']).name
    legacy = row['disposition'] == 'legacy-generic-error-table'
    if legacy:
        chain = 'Original NT host recipe omits this generic severity/string array; selected nt_error uses original nt_nls resource lookup and original obj.vdm/resource.rc instead'
        limits = 'Physical obsolete generic error-table role only; localized messages/error handling remain at selected partial NT owners. Generic table-driven ErrData/severity behavior is not silently installed over original NT behavior; dialog, failure and lifetime proof remain independent.'
    else:
        chain, limits = contracts[leaf]
    proofs.append(dict(relative_path=row['relative_path'], shard=ledger[row['relative_path']]['shard'], sha256=row['sha256'],
        edition='OpenNT', original_path=row['original_path'], prior_status='pending', status='excluded' if legacy else 'partial',
        role='historical-superseded-nt-error-table-source-only' if legacy else 'selected-native-source-partial',
        evidence_table='input-image-observations.json', original_edge=chain, limits=limits,
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status=('not-applicable: generic table superseded by original NT resource/error owner' if legacy else
            'unmeasurable: selected code and bounded source-object cases do not measure complete guest input/locale/error behavior and lifecycle'),
        followup_stage='S13'))
assert len(proofs) == 5
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'input-native-role-review.md',
    'Three original input/NLS integrations partial with an actual-object scan-bound defect retained for S13; two generic error-table files excluded through original NT resource ownership. No guest or runtime image changed.')
observation = dict(result='pass', base=BASE, candidate_count=5,
    source_tables=[dict(path='input-image-observations.json', sha256=H['sha'](A / 'input-image-observations.json'))],
    host_input_observations=[], scope='Three bounded integrations and two original generic-table roles; known scan-bound defect is not repaired or waived')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 5)
