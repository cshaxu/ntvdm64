"""Integrate reviewed original WOW owners, without claiming complete behavior."""
import csv
import json
import runpy
import subprocess
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'cc061741d'
PREFIX = 'execution-selected-interfaces-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
families = {
    'virtual-interrupt-interface': ('inc/vint.h',),
    'fastwow-header': ('wow32/fastwow.h',),
}
reports = {'ole-alias': 'ole-alias-header', 'control-message': 'control-message'}
ledger = {row['relative_path']: row for row in H['rows'](A / 'combined-initial-status.csv')}
proofs, tables = [], []
for family, names in families.items():
    folder = A / 'cross-369'
    observed = folder / (family + '-observations.json')
    data = json.loads(observed.read_text(encoding='utf-8-sig'))
    replay = json.loads(subprocess.check_output([sys.executable, '-B', str(folder / (family + '-observer.py'))]))
    assert replay == data, family
    tables.append(dict(path=observed.relative_to(A).as_posix(), sha256=H['sha'](observed)))
    report = 'cross-369/' + reports.get(family, family) + '-disposition.md'
    assert (A / report).is_file()
    for name in names:
        path = name
        identities = data['sha256'][path]
        assert identities['current'] == identities['opennt'] == identities['opennt45']
        assert ledger[path]['status'] in ('pending', 'partial')
        proofs.append(dict(relative_path=path, shard=ledger[path]['shard'], sha256=identities['current'],
            edition='OpenNT', original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path),
            prior_status='pending', status='partial', role='selected-native-source-partial',
            evidence_table=observed.relative_to(A).as_posix(),
            original_edge='Selected original execution interface; source and actual build dependency or object evidence in ' + report,
            limits='Selected source/interface only; functional, failure, reentry and retirement coverage remains bounded by ' + report,
            original_text_retained_percent='100', estimated_restored_code_percent='',
            restored_estimate_status='unmeasurable: source identity is not behavioral coverage', followup_stage='S11-S17'))
assert len(proofs) == 2
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'cross-369/function-owner-worklist.md',
    'Two selected original execution interface roles are partial; runtime acceptance remains open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables,
    host_input_observations=[], scope='Selected source roles only; no complete runtime capability assertion')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
