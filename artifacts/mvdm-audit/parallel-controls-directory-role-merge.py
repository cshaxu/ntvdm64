"""Merge independently checked controls and DOS directory source roles."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '9e1885d75', 'parallel-controls-directory-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
tables = ('shard-2/wow-scrollbar-image-observations.json', 'shard-3/wow-caret-image-observations.json',
          'shard-1/dem-directory-evidence.json')
scripts = ('shard-2/wow-scrollbar-image-observer.py', 'shard-1/dem-directory-observer.py')
for name, script in zip((tables[0], tables[2]), scripts):
    assert runpy.run_path(str(A / script))['inspect']() == json.loads((A / name).read_text(encoding='utf-8'))
subprocess.run(['python', '-B', str(A / 'shard-3/wow-caret-durable-check.py')], check=True)
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
contracts = {
    'wow32/wmsgsbm.c': ('Original forward/reverse scrollbar conversion tables bind to release', 'Guest interaction, signed values, reentry and context lifetimes remain open.', tables[0]),
    'wow32/wucaret.c': ('Eight original caret thunks and selected USER calls bind to release', 'Blink, focus, bitmap failures and task lifetime remain open.', tables[1]),
    'wow32/wutbl2.h': ('Eight initial original caret USER ordinal entries bind to release', 'The remainder of the USER table and guest execution remain open.', tables[1]),
    'dos/dem/demdir.c': ('Original directory functions and selected dispatch/caller edges bind to release', 'Guest pointers, drive failures, OEM paths and rollback remain open.', tables[2]),
    'dos/dem/demlabel.c': ('Original label functions and selected dispatch/caller edges bind to release', 'Guarded NUL overread, guest reachability and isolated-volume outcomes remain open.', tables[2]),
}
proofs = []
for path, (edge, limits, evidence) in contracts.items():
    row, f = ledger[path], frozen[path]
    assert row['status'] == 'pending' and f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=f['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending',
        status='partial', role='selected-native-source-partial', evidence_table=evidence,
        original_edge=edge, limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: linked ownership does not establish complete behavior or lifecycle',
        followup_stage='S12/S15'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'parallel-controls-directory-role-review.md',
    'Original scrollbar, caret and DOS directory owners are partial; behavior and lifecycle remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=n, sha256=H['sha'](A / n)) for n in tables], host_input_observations=[],
    scope='Selected original image and dispatch edges; no full guest behavior acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
