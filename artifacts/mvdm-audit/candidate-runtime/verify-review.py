import csv, hashlib, json, struct
from pathlib import Path

root = Path('artifacts/mvdm-audit/candidate-runtime')
index = json.loads((root / 'evidence-index.json').read_text(encoding='utf-8-sig'))
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest().upper()
for record in index['records']:
    assert sha(root / record['file']) == record['sha256'], record['file']
for record in index['sources']:
    assert sha(Path(record['Path'])) == record['Hash']
manifest = json.loads((root / 'candidate-manifest.json').read_text(encoding='utf-8-sig'))
assert len(manifest) == 10 and len({r['Name'] for r in manifest}) == 10
assert {r['PackageId'] for r in manifest} == {'d1be0f65-adb4-4ee4-ae16-f301ad3f3c90'}
for row in manifest:
    assert sha(Path('build/t473-s10/candidate-validation/package') / row['RelativePath'].replace('\\','/')) == row['Sha256']
full = json.loads((root / 'full-timings.json').read_text(encoding='utf-8-sig'))
assert full['Suite'] == 'Full' and all(g['Passed'] for g in full['Gates'])
for name in ('candidate-options-r1.txt','candidate-options-r2.txt'):
    assert 'modal dialog did not appear' in (root / name).read_text()
for name in ('baseline-options-r1.txt','baseline-options-r2.txt','diagnostic-options.txt'):
    assert 'PASS SOL Options command, modal dialog, close and parent recovery' in (root / name).read_text()
assert not json.loads((root / 'bios-r2-result.json').read_text(encoding='utf-8-sig'))['passed']
assert json.loads((root / 'bios-r3-result.json').read_text(encoding='utf-8-sig'))['passed']
assert 'VIDEO7_MODE_FLAGS_PASS modes=32 preserve=1 clear=1 invalid=2' in (root / 'bios-r3-console.txt').read_text()
ledger={r['relative_path']: r for r in csv.DictReader(Path('artifacts/mvdm-audit/combined-initial-status.csv').open(encoding='utf-8-sig', newline=''))}
followups=[]
for shard in (1,2,3):
    path=Path(f'artifacts/mvdm-audit/shard-{shard}/shared-guest-include-followup.csv')
    for row in csv.DictReader(path.open(encoding='utf-8-sig', newline='')):
        name=row['relative_path']
        assert ledger[name]['status']=='pending' and ledger[name]['shard']==str(shard)
        expected=row.get('sha256') or row['current_sha256']
        assert sha(Path('src/mvdm')/name)==expected
        assert any(Path(p.strip()).is_file() and sha(Path(p.strip()))==expected for p in row['original_path'].split(';'))
        followups.append(name)
assert len(followups)==len(set(followups))==50
result={'result':'pass','evidence_files':len(index['records']),'source_identities':len(index['sources']),
        'followup_paths':len(followups),'candidate_publication_qualified':False,'S10_complete':False,
        'scope':'Integrity and consistency of recorded outcomes; not a substitute for their behavior assertions'}
(root/'review-verification.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result))
