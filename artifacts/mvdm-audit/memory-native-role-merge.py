"""Merge one partial UMB owner and two physical alternative memory roles."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '4d0f2d2c6', 'memory-native-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = runpy.run_path(str(A / 'memory-image-observer.py'))['inspect']()
assert observed == json.loads((A / 'memory-image-observations.json').read_text())
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
contracts = {
    'nt_umb.c': ('Original config/EMM/XMS/VDD -> original UMB list/split/reserve/release -> selected CPU40 SAS mapping and XMS notification',
                 'Actual-object 4240 normal assertions pass. Six direct invalid-helper ranges are accepted, but public VDD entrypoints have additional count and hook bounds; no production defect is established from invalid private inputs. Allocation failures, hook/list lifetime, nesting, real guest memory faults and reset/retirement remain S13.'),
    'nt_sas.c': ('Original HOST_SAS/SUN_VA alternative host table is inactive; selected original CPU40 ccpusas4 supplies sas_init/sas_term/c_sas_connect_memory',
                 'Exclude this physical alternate host-table file only. CPU40 address/data/fault/A20/video mapping and SAS lifecycle remain at selected partial owners; absence of this optional table is not exclusion of memory capability.'),
    'nt_sec.c': ('Original fixed-address NT4 physical-video alias helpers are unextracted in selected non-MONITOR/non-X86GFX CPU40; original CCPU SAS and host-owned DIBs supply emulated memory/display resources',
                 'Exclude this physical NT4 hardware-mapping helper only, under the sole supported CCPU40 profile. Do not map modern host memory at guest physical numeric addresses. Real SAS/video mode, buffer and guest-fault lifetimes remain at partial owners; NT4 kernel V86/X86GFX is not a supported second backend.'),
}
proofs = []
for row in observed['sources']:
    leaf = Path(row['relative_path']).name
    chain, limits = contracts[leaf]
    alternative = row['disposition'] == 'alternative-profile-file'
    proofs.append(dict(relative_path=row['relative_path'], shard=ledger[row['relative_path']]['shard'], sha256=row['sha256'],
        edition='OpenNT', original_path=row['original_path'], prior_status='pending', status='excluded' if alternative else 'partial',
        role='historical-alternative-memory-provider-source-only' if alternative else 'selected-native-source-partial',
        evidence_table='memory-image-observations.json', original_edge=chain, limits=limits, original_text_retained_percent='100',
        estimated_restored_code_percent='', restored_estimate_status=(
            'not-applicable: physical alternative HOST_SAS/NT4 hardware mapping role; selected original CPU40 memory capability remains partial' if alternative else
            'unmeasurable: all seven mapped UMB functions and original text are retained, but bounded list tests do not measure real guest hooks, allocation failure or retirement'),
        followup_stage='S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'memory-native-role-review.md',
    'One original UMB owner partial with bounded native evidence; two physical alternate memory profiles excluded through selected original CPU40 ownership. Direct helper boundary failure is not a demonstrated production defect. No original source or image changed.')
observation = dict(result='pass', base=BASE, candidate_count=3,
    source_tables=[dict(path='memory-image-observations.json', sha256=H['sha'](A / 'memory-image-observations.json'))],
    host_input_observations=[], scope='UMB integration and original profile alternatives; no complete guest memory or release qualification')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 3)
