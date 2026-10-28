"""Reconcile reviewed guest image and historical build inputs, not guest behavior."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '6a7bf31bf'
PREFIX = 'parallel-gdi-ctl3d-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
groups = [('shard-1', 'guest-toolhelp-owner'), ('shard-2', 'win16-gdi-resource'),
          ('shard-2', 'win16-ctl3d-guest'), ('shard-3', 'graphics-input')]
tables = []
for shard, name in groups:
    folder = A / shard
    table = folder / (name + '-observations.json')
    actual = json.loads(subprocess.check_output(['python', str(folder / (name + '-observer.py'))]))
    assert actual == json.loads(table.read_text(encoding='utf-8'))
    tables.append(dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table)))
gdi = json.loads((A / 'shard-2/win16-gdi-resource-observations.json').read_text())
assert gdi['image_sha256'] == gdi['source_image_sha256'] == gdi['installed_sha256']
contracts = {
    'wow16/gdi/gdi.rcv': ('excluded', 'shard-2/win16-gdi-resource-observations.json', 'Historical GDI version resource input'),
    'wow16/gdi/GDI.EXE': ('partial', 'shard-2/win16-gdi-resource-observations.json', 'Original image matches archive and installed image'),
    'wow16/ctl3dv2/readme.txt': ('excluded', 'shard-2/win16-ctl3d-guest-observations.json', 'Historical reference-source documentation'),
    'wow16/ctl3dv2/ctl3d.doc': ('excluded', 'shard-2/win16-ctl3d-guest-observations.json', 'Historical CTL3D documentation'),
    'wow16/ctl3dv2/ctl3d.c': ('excluded', 'shard-2/win16-ctl3d-guest-observations.json', 'Historical guest debugging reference source'),
    'wow16/ctl3dv2/ctl3d.h': ('excluded', 'shard-2/win16-ctl3d-guest-observations.json', 'Historical guest debugging reference header'),
    'wow16/ctl3dv2/common.ver': ('excluded', 'shard-2/win16-ctl3d-guest-observations.json', 'Historical guest reference resource input'),
    'dos/v86/cmd/graphics/GRAPHICS.EXC': ('excluded', 'shard-3/graphics-input-observations.json', 'Original linker intermediate for GRAPHICS.COM'),
    'dos/v86/cmd/graphics/graphics.tag': ('excluded', 'shard-3/graphics-input-observations.json', 'Historical graphics change log'),
}
assert len(contracts) == 9
for name in ('ctl3d.c', 'ctl3d.h', 'common.ver'):
    for graph in ('worker', 'wow32'):
        text = Path(f'build/.ninja/msvc-x86-release/{graph}/build.ninja').read_text().replace('\\', '/').lower()
        assert 'wow16/ctl3dv2/' + name not in text
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for path, (status, table, edge) in contracts.items():
    row, source = ledger[path], frozen[path]
    assert row['status'] == 'pending'
    assert source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    edition = 'OpenNT'; original = Path('O:/repos.external/OpenNT/base/mvdm') / path
    role = 'selected-guest-image-partial' if status == 'partial' else 'historical-guest-build-input-only'
    limits = 'Guest execution, image lineage, feature behavior, failures and lifecycle remain unproved.'
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'], edition=edition,
        original_path=str(original), prior_status='pending', status=status, role=role, evidence_table=table,
        original_edge=edge, limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status=('unmeasurable: image identity does not establish guest behavior' if status == 'partial'
                                  else 'not-applicable: historical guest input file role only'), followup_stage='S12-S17'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'One guest image is partial and eight historical guest input roles are excluded, not their features.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables,
    host_input_observations=[], scope='Guest image identity and input roles only; no guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
