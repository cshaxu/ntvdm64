"""Record the selected WGMAN file's empty behavioral source role."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '8b68648e0'
PREFIX = 'shard0-empty-gdi-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/wow-gdi-empty-owner-observations.json'
observed = json.loads(table.read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-0/wow-gdi-empty-owner-observer.py'))['inspect']() == observed
path = 'wow32/wgman.c'
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
row, source = ledger[path], frozen[path]
assert row['status'] == 'pending' and row['shard'] == '0'
assert source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256'] == observed['source_sha256']
proof = dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'], edition='OpenNT',
    original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='excluded',
    role='selected-empty-source-file-only', evidence_table=table.relative_to(A).as_posix(),
    original_edge='Selected original translation unit has no code section; standard linker directives are also supplied by wgdi.obj',
    limits='This file-only disposition does not exclude GDI capabilities, guest calls or resource and teardown obligations.',
    original_text_retained_percent='100', estimated_restored_code_percent='',
    restored_estimate_status='not-applicable: selected physical source has no executable behavioral body', followup_stage='S16')
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proof), lineterminator='\n')
    writer.writeheader()
    writer.writerow(proof)
H['apply_dispositions']([proof], BASE, 'shard-0/wow-gdi-empty-owner-review.md',
    'Selected WGMAN file has no behavioral code; all GDI functionality remains in scope.')
observation = dict(result='pass', base=BASE, candidate_count=1,
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Selected empty physical source role; no GDI capability exclusion')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 1)
