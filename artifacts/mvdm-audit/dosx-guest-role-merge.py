"""Reconcile DOSX image and historical guest-build input roles."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit'); BASE = 'b29a6fbab'; PREFIX = 'dosx-guest-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-2/dosx-guest-build-observations.json'
observed = json.loads(table.read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-2/dosx-guest-build-observer.py'))['inspect']() == observed
assert observed['source_image_sha256'] == observed['installed_sha256'] == observed['archive_basename_matches'][0]['sha256']
paths = ('dpmi/486/dxintr.asm', 'dpmi/dxini.asm', 'dpmi/dxmmgr.asm', 'dpmi/dxnetbio.asm',
         'dpmi/dxoem.asm', 'dpmi/ntnpxem.asm', 'dpmi/usa/dxmsg.asm', 'dpmi/mk.cmd', 'dpmi/migrate.bat')
for graph in (Path('build/.ninja/msvc-x86-release/worker'), Path('build/.ninja/msvc-x86-release/wow32'),
              Path('build/.ninja/msvc-x64-release/frontend'), Path('build/.ninja/msvc-x64-release/service')):
    build = (graph / 'build.ninja').read_text().replace('$:', ':').replace('\\', '/').lower()
    deps = subprocess.check_output(['ninja', '-C', str(graph), '-t', 'deps'], text=True, encoding='utf-8', errors='replace').replace('\\', '/').lower()
    for path in paths:
        absolute = (Path('src/mvdm') / path).resolve().as_posix().lower()
        assert absolute not in build and absolute not in deps
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for path in (*paths, 'dpmi/486/dosx.exe'):
    row, source = ledger[path], frozen[path]; assert row['status'] == 'pending'
    is_image = path.endswith('dosx.exe')
    edition = 'OpenNT-4.5' if is_image else 'OpenNT'
    root = 'O:/repos.external/OpenNT-4.5/nt/private/mvdm' if is_image else 'O:/repos.external/OpenNT/base/mvdm'
    assert source['current_sha256'] == source['opennt45_sha256']
    if not is_image: assert source['current_sha256'] == source['opennt_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'], edition=edition,
        original_path=str(Path(root) / path), prior_status='pending', status='partial' if is_image else 'excluded',
        role='selected-guest-image-partial' if is_image else 'historical-guest-build-input-only',
        evidence_table=table.relative_to(A).as_posix(),
        original_edge='Original DOSX guest image matches archive and installation' if is_image else 'Historical DOSX guest build input or script',
        limits='Guest mode transitions, errors, callback lifetime and image lineage remain unverified.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: image identity does not prove runtime behavior' if is_image else 'not-applicable: historical guest build input only', followup_stage='S11-S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'DOSX image is partial and nine historical guest inputs are excluded, not guest functionality.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))], host_input_observations=[],
    scope='DOSX image and historical input roles; no guest execution acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
