"""Reconcile selected SHLD, GRAPHICS image and historical build tools."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit'); BASE = 'd7e55c0e1'; PREFIX = 'parallel-shld-tools-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
groups = [('shard-0', 'ccpu-shld-image'), ('shard-1', 'guest-graphics-owner'),
          ('shard-2', 'dos-v86-build-tools'), ('shard-3', 'redir-async')]
tables = []
for shard, name in groups:
    folder = A / shard; table = folder / (name + '-observations.json')
    assert json.loads(subprocess.check_output(['python', str(folder / (name + '-observer.py'))])) == json.loads(table.read_text(encoding='utf-8'))
    tables.append(dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table)))
contracts = {
    'softpc.new/base/ccpu386/shld.c': ('partial', 'selected-native-source-partial', 'shard-0/ccpu-shld-image-observations.json', 'Original instruction body and decoder references bind to released NTVDM'),
    'dos/v86/cmd/graphics/GRAPHICS.COM': ('partial', 'selected-guest-image-partial', 'shard-1/guest-graphics-owner-observations.json', 'Original guest image matches archive and installation'),
    **{p: ('excluded', 'historical-guest-build-input-only', 'shard-2/dos-v86-build-tools-observations.json', 'Original tool source consumed by a historical guest build') for p in (
        'dos/v86/tools/src/h2inc/h2inc.c', 'dos/v86/tools/src/stripdd/stripdd.c', 'dos/v86/tools/src/stripz/stripz.c')},
}
graphics = json.loads((A / 'shard-1/guest-graphics-owner-observations.json').read_text())['guest_image']
assert graphics['sha256'] == graphics['zip_sha256']
for graph in (Path('build/.ninja/msvc-x86-release/worker'), Path('build/.ninja/msvc-x86-release/wow32'),
              Path('build/.ninja/msvc-x64-release/frontend'), Path('build/.ninja/msvc-x64-release/service')):
    build = (graph / 'build.ninja').read_text().replace('$:', ':').replace('\\', '/').lower()
    deps = subprocess.check_output(['ninja', '-C', str(graph), '-t', 'deps'], text=True, encoding='utf-8', errors='replace').replace('\\', '/').lower()
    for path, (status, _, _, _) in contracts.items():
        if status == 'excluded':
            target = (Path('src/mvdm') / path).resolve().as_posix().lower()
            assert target not in build and target not in deps
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for path, (status, role, table, edge) in contracts.items():
    row, source = ledger[path], frozen[path]
    assert row['status'] == 'pending' and source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status=status,
        role=role, evidence_table=table, original_edge=edge,
        limits='Guest instruction or device behavior, image provenance, failures and lifecycle remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status=('unmeasurable: source or image identity does not establish runtime behavior' if status == 'partial'
                                  else 'not-applicable: historical guest build-tool source role only'), followup_stage='S11-S17'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'SHLD and GRAPHICS.COM remain partial; three historical tool source roles are excluded, not guest functionality.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables,
    host_input_observations=[], scope='Selected source, image identity and build-input roles; no guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
