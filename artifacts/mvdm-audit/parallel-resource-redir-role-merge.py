"""Merge independently replayed WOW resource and redirector roles."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'fee15c70f', 'parallel-resource-redir-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
tables = ('shard-1/wow-cursor-evidence.json', 'shard-2/wow-resource-owner-observations.json',
          'shard-3/redir-image-observations.json')
scripts = ('shard-1/wow-cursor-observer.py', 'shard-2/wow-resource-owner-observer.py')
for name, script in zip(tables, scripts):
    assert runpy.run_path(str(A / script))['inspect']() == json.loads((A / name).read_text(encoding='utf-8'))
subprocess.run(['python', '-B', str(A / 'shard-3/redir-durable-check.py')], check=True)
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
contracts = {
    'wow32/wucursor.c': ('Original cursor dispatch and selected conversion calls bind to release', 'Guest cursor creation, errors and teardown remain open.', tables[0]),
    'wow32/wcuricon.c': ('Original cursor/icon alias bodies and task cleanup edge bind to release', 'Alias identity, concurrent use and lifetime remain open.', tables[0]),
    'wow32/wres16.c': ('Original resource bodies and eleven caller sites bind to release', 'Guest resource ownership, reentry, errors and teardown remain open.', tables[1]),
    'wow32/wres16.h': ('Original resource definitions are a selected compile dependency', 'ABI representation, all consumers and ownership remain open.', tables[1]),
    'vdmredir/vrdll.c': ('Original DLL lifecycle body and selected adapter edges bind to release', 'Thread-notification result and teardown semantics remain open.', tables[2]),
    'vdmredir/vrnetb.c': ('Original NetBIOS dispatch slot and five bodies bind to release', 'Real peer, asynchronous completion and guest teardown remain open.', tables[2]),
    'vdmredir/vrdlcbuf.c': ('Eight original DLC buffer bodies and selected callers bind to release', 'Real provider, buffer bounds, exhaustion and callback lifetimes remain open.', tables[2]),
}
proofs = []
for path, (edge, limits, evidence) in contracts.items():
    row, f = ledger[path], frozen[path]
    assert row['status'] == 'pending' and f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=f['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending',
        status='partial', role='selected-native-source-partial', evidence_table=evidence,
        original_edge=edge, limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: linked ownership does not establish complete guest behavior or lifetime',
        followup_stage='S14/S15/S16'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'parallel-resource-redir-role-review.md',
    'Seven original WOW resource and redirector roles are partial; runtime lifetimes remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=n, sha256=H['sha'](A / n)) for n in tables], host_input_observations=[],
    scope='Selected original image and call edges; no guest or external provider acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
