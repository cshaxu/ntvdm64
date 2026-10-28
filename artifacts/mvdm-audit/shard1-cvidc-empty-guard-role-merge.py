"""Classify only declaration-less generated headers, retaining CPU features."""
import csv
import json
import runpy
import subprocess
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '063cb8712', 'shard1-cvidc-empty-guard-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
family_path = A / 'shard-1/cvidc-header-family-evidence.json'
guard_path = A / 'shard-1/cvidc-empty-guard-observations.json'
family = json.loads(subprocess.check_output([sys.executable, '-B', str(A / 'shard-1/cvidc-header-family-observer.py')]))
guard = runpy.run_path(str(A / 'shard-1/cvidc-empty-guard-observer.py'))['inspect']()
assert family == json.loads(family_path.read_text(encoding='utf-8-sig'))
assert guard == json.loads(guard_path.read_text(encoding='utf-8-sig'))
guard_rows = {x['name']: x for x in guard['files']}
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for source in family['files']:
    if not source['guard_only']:
        continue
    name = source['name']
    path = 'softpc.new/base/cvidc/' + name
    assert source['reference_hashes'] == [source['sha256'], source['sha256']]
    assert not source['direct_includes'] and not source['valid_worker_objects']
    assert ledger[path]['shard'] == '1' and ledger[path]['status'] in ('pending', 'excluded')
    refs = guard_rows[name]['other_references']
    assert len(refs) == 3
    copies = {entry.rsplit(':', 1)[0] for entry in refs}
    assert len(copies) == 1
    assert (Path('src/mvdm') / path).read_bytes() == Path(next(iter(copies))).read_bytes()
    proofs.append(dict(relative_path=path, shard='1', sha256=source['sha256'].upper(), edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='excluded',
        role='historical-empty-declaration-only', evidence_table=family_path.relative_to(A).as_posix(),
        original_edge='Original file is a three-line empty guard; current graph has no selected dependency and only identical MIPS copy references its guard',
        limits='File-only exclusion does not exclude CPU/video features, generated variants or external profiles.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: no declarations or executable code', followup_stage='S11-S13'))
assert len(proofs) == 11
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-1/cvidc-header-family-review.md',
    'Eleven empty generated declaration file roles excluded; all CPU and video features remain in scope.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=p.relative_to(A).as_posix(), sha256=H['sha'](p)) for p in (family_path, guard_path)],
    host_input_observations=[], scope='Empty file roles only; not CPU or video feature exclusion')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
