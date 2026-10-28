"""Integrate one selected resource source, with bounded current-image evidence."""
import csv
import json
import runpy
import subprocess
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '391cca934', 'wow32-resource-current-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'wow32-resource-current-observations.json'
evidence = json.loads(table.read_text(encoding='utf-8-sig'))
assert evidence == json.loads(subprocess.check_output([sys.executable, '-B', str(A / 'wow32-resource-current-observer.py')]))
path = 'wow32/wow32.rc'
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
assert ledger[path]['status'] in ('pending', 'partial')
proof = dict(relative_path=path, shard=ledger[path]['shard'], sha256=evidence['source_sha256'],
             edition='OpenNT', original_path='O:/repos.external/OpenNT/base/mvdm/' + path,
             prior_status='pending', status='partial', role='selected-native-source-partial',
             evidence_table=table.relative_to(A).as_posix(),
             original_edge='Original resource is selected by the observed WOW32 build; representative strings occur in the released image',
             limits='Resource ID/language mapping, missing-resource failure and cleanup remain unverified; source edge does not bind the complete released payload',
             original_text_retained_percent='100', estimated_restored_code_percent='',
             restored_estimate_status='unmeasurable: source identity and representative strings are not functional coverage', followup_stage='S16')
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proof), lineterminator='\n')
    writer.writeheader()
    writer.writerow(proof)
H['apply_dispositions']([proof], BASE, 'shard-1/wow32-resource-image-review.md',
                        'Selected WOW32 resource role is partial; resource behavior remains open.')
result = dict(result='pass', base=BASE, candidate_count=1,
              source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
              host_input_observations=[], scope='Resource source and bounded image strings only')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 1)
