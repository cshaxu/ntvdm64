"""Merge independently checked original USER client and SOUND roles."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'cae58ad80', 'parallel-user-sound-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
tables = ('shard-2/wow-sound-image-observations.json', 'shard-3/wow-usercli-image-observations.json')
assert runpy.run_path(str(A / 'shard-2/wow-sound-image-observer.py'))['inspect']() == json.loads((A / tables[0]).read_text(encoding='utf-8'))
subprocess.run(['python', '-B', str(A / 'shard-3/wow-usercli-durable-check.py')], check=True)
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
contracts = {
    'wow32/wsman.c': ('Original DoBeep body and USER32 import bind to released WOW32', 'Actual guest invocation, audible result, errors and other SOUND functionality remain open.', tables[0]),
    'wow32/wstbl2.h': ('All 19 original SOUND table entries bind to release', 'Eighteen slots select original unimplemented handlers; guest capability implications remain open.', tables[0]),
    'wow32/wusercli.c': ('Thirty original USER client bodies and 32 initial slots bind to release', 'Guest local versus thunk route, object identity, reentry and errors remain open.', tables[1]),
    'wow32/wusercli.h': ('Original USER client declarations are selected by actual host preprocessing', 'PMODE32 guest route, shared state and lifecycle remain open.', tables[1]),
}
proofs = []
for path, (edge, limits, evidence) in contracts.items():
    row, f = ledger[path], frozen[path]
    assert row['status'] == 'pending' and f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=f['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending',
        status='partial', role='selected-native-source-partial', evidence_table=evidence,
        original_edge=edge, limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: initial table or declaration selection does not establish guest behavior',
        followup_stage='S14/S15/S16'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'parallel-user-sound-role-review.md',
    'Original USER client and SOUND roles are partial; actual guest behavior remains open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=n, sha256=H['sha'](A / n)) for n in tables], host_input_observations=[],
    scope='Original initial table and selected source; no guest execution or audio acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
