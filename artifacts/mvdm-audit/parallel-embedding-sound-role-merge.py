"""Reconcile the original embedding owner and delivered Win16 sound image."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '12ea7a5c1'
PREFIX = 'parallel-embedding-sound-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
contracts = {
    'wow32/mapembed.c': ('shard-1/wow-embedding', 'selected-native-source-partial', 'Six original embedding functions and registry binding edges match released WOW32', 'Guest profile calls, provider failures, guest buffer semantics, cross-task freshness and teardown remain open.', 'S16'),
    'wow16/drivers/sound/sound.drv': ('shard-2/win16-sound-guest', 'selected-guest-image-partial', 'Original sound driver matches packaged and installed image; DOBEEP source chain is traced', 'Guest selection/loading, actual sound, timing, other ordinals and source-to-image provenance remain open.', 'S16'),
}
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs, tables = [], []
for path, (stem, role, edge, limits, stage) in contracts.items():
    table = A / (stem + '-observations.json')
    assert runpy.run_path(str(A / (stem + '-observer.py')))['inspect']() == json.loads(table.read_text(encoding='utf-8'))
    row, source = ledger[path], frozen[path]
    assert row['status'] == 'pending' and source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    tables.append(dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table)))
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role=role, evidence_table=table.relative_to(A).as_posix(), original_edge=edge, limits=limits,
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: source/image identity does not prove guest execution or lifecycle', followup_stage=stage))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'Embedding source and Win16 sound image are partial; guest execution and host lifecycle remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables,
    host_input_observations=[], scope='Original source/image binding only; no guest behavior acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
