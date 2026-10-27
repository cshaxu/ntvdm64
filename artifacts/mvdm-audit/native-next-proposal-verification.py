import collections, csv, hashlib, json, runpy
from pathlib import Path

A=Path('artifacts/mvdm-audit')
def rows(p):
    with p.open(encoding='utf-8-sig',newline='') as stream:return list(csv.DictReader(stream))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
ledger={r['relative_path']:r for r in rows(A/'combined-initial-status.csv')}
frozen={r['relative_path']:r for r in rows(A/'frozen-file-reconciliation.csv')}
helpers=runpy.run_path(str(A/'guest-source-role-checkpoint.py'))
baseline={r['relative_path']:r for r in helpers['original_rows'](A/'combined-initial-status.csv','ecd4a604a')}
seen=set();tables=[];counts=collections.Counter()
for shard in (1,2,3):
    path=A/f'shard-{shard}/native-pending-followup-2.csv'
    batch=rows(path);assert len(batch)==60
    proposal=rows(A/f'shard-{shard}/native-pending-owner-review.csv')
    cohort=sorted(r['relative_path'] for r in proposal if baseline[r['relative_path']]['status']=='pending')[30:90]
    assert sorted(r['relative_path'] for r in batch)==cohort
    tables.append(dict(path=path.relative_to(A).as_posix(),sha256=sha(path)))
    for row in batch:
        name=row['relative_path'];assert name not in seen;seen.add(name)
        assert ledger[name]['status']=='pending' and int(ledger[name]['shard'])==shard
        assert sha(Path('src/mvdm')/name)==row['current_sha256']==frozen[name]['current_sha256']
        assert sha(Path(row['original_path']))==row['original_sha256']
        assert row['proposal_status'] in ('pending','partial','excluded-file-role-only')
        assert row['original_function_or_contract'] and row['current_route_and_selection']
        assert row['concrete_remaining_gap'] and row['suggested_followup_owner']
        counts[row['proposal_status']]+=1
assert len(seen)==180 and counts=={'partial':47,'excluded-file-role-only':1,'pending':132}
followup=A/'candidate-runtime-followup'
observation=json.loads((followup/'observations.json').read_text(encoding='utf-8-sig'))
assert observation['publishable'] is False and not observation['production_source_changed']
for record in observation['records']:assert sha(followup/record['file'])==record['sha256']
assert 'modal dialog did not appear' in (followup/'cpl-options-result.txt').read_text()
for file in ('diagnostic-provider-r2.txt','diagnostic-provider-r3.txt','page-domain-options-result.txt'):
    assert 'PASS SOL Options command, modal dialog, close and parent recovery' in (followup/file).read_text()
for file in ('cpl-normal-cache-inputs.json','page-domain-normal-cache-inputs.json'):
    for record in json.loads((followup/file).read_text(encoding='utf-8-sig')):
        assert sha(Path(record['Path']))==record['Hash']
for name,package in (('cpl','cpl-comparison-r4-package'),('page-domain','page-domain-diagnostic-package')):
    manifest=json.loads((followup/f'{name}-package-inputs.json').read_text(encoding='utf-8-sig'))
    assert len(manifest)==len({r['Name'] for r in manifest})==10
    for row in manifest:
        path=Path('build/t473-s10/candidate-validation')/package/row['RelativePath'].replace('\\','/')
        assert sha(path)==row['Sha256']
negative=A/'creation-negative-only'
negative_observation=json.loads((negative/'observations.json').read_text(encoding='utf-8-sig'))
assert negative_observation['publishable'] is False
assert negative_observation['production_source_changed'] is False
assert negative_observation['guest_changed'] is False
assert negative_observation['failed_branch_records']==0
assert negative_observation['ordinary_candidate_qualified'] is False
for record in negative_observation['records']:
    assert sha(negative/record['file'])==record['sha256']
assert 'PASS SOL Options command, modal dialog, close and parent recovery' in (negative/'probe-result.txt').read_text()
assert json.loads((negative/'build-exit.json').read_text(encoding='utf-8-sig'))['exit']==0
assert json.loads((negative/'probe-exit.json').read_text(encoding='utf-8-sig'))['exit']==0
negative_manifest=json.loads((negative/'package-inputs.json').read_text(encoding='utf-8-sig'))
assert len(negative_manifest)==len({r['Name'] for r in negative_manifest})==10
for row in negative_manifest:
    image=Path('build/t473-s10/candidate-validation/creation-negative-only-package')/row['RelativePath'].replace('\\','/')
    assert sha(image)==row['Sha256']
result=dict(result='pass',paths=180,tables=tables,proposal_counts=dict(counts),
            ledger_changed=False,capabilities_accepted=False,
            diagnostic_inputs_checked=len(observation['records'])+len(negative_observation['records']),candidate_qualified=False,
            scope='Proposal source identity, status separation and recorded diagnostic outcomes; not behavior acceptance')
(A/'native-next-proposal-verification.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result))
