"""Reconcile empty C-VID guard files without excluding capabilities."""
import csv
import json
import re
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit'); BASE = 'a1cd20ec4'; PREFIX = 'shard3-empty-declaration-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-3/cvidc-empty-guard-observations.json'
observed = json.loads(table.read_text(encoding='utf-8'))
assert json.loads(subprocess.check_output(['python', str(A / 'shard-3/cvidc-empty-guard-observer.py')])) == observed
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
guards = []; proofs = []
for name, item in observed.items():
    if name.startswith('_'): continue
    path = 'softpc.new/base/cvidc/' + name; row = ledger[path]; source = frozen[path]
    assert row['status'] == 'pending' and row['shard'] == '3' and item['guard_only'] and not item['normalized_dep_lines']
    assert item['current'].upper() == item['opennt'].upper() == item['opennt45'].upper() == source['current_sha256']
    lines = [line.strip() for line in (Path('src/mvdm') / path).read_text().splitlines() if line.strip()]
    match = re.fullmatch(r'#ifndef\s+(\w+)', lines[0]); assert match
    guard = match.group(1); assert re.fullmatch(r'#define\s+' + re.escape(guard), lines[1]) and lines[2].startswith('#endif')
    guards.append(guard)
    proofs.append(dict(relative_path=path, shard='3', sha256=source['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='excluded',
        role='historical-empty-declaration-only', evidence_table=table.relative_to(A).as_posix(),
        original_edge='Original header contains only an empty include guard',
        limits='Physical empty file role only; CPU, video, generated code, callbacks and guest behavior remain in scope.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: no behavioral declarations or implementation', followup_stage='S11-S13'))
assert len(proofs) == len(guards) == 12
args = ['rg', '-n', '-F']
for guard in guards: args.extend(('-e', guard))
args.extend(('-g', '*.[ch]', 'src/mvdm'))
uses = subprocess.check_output(args, text=True).splitlines(); assert uses
for line in uses:
    assert re.fullmatch(r'#(?:ifndef|define)\s+\w+|#endif\s*/\*.*\*/', line.rsplit(':', 1)[-1].strip()), line
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-3/cvidc-empty-guard-role-review.md',
    'Twelve empty original guard file roles are excluded; CPU and video functionality remain in scope.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))], host_input_observations=[],
    guard_reference_lines=len(uses), scope='Empty physical guard roles only; no feature or guest behavior exclusion')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
