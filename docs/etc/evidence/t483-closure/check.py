from pathlib import Path
import csv,json,hashlib,subprocess,collections
R=Path.cwd();B=R/'build/t483-s21-closure'
def load(p):return json.loads((R/p).read_text(encoding='utf-8-sig'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
baseline='9e7eb19c059a3ba8e55c73c6943842416932b8d2'
assert subprocess.check_output(['git','rev-parse','HEAD'],text=True).strip()==baseline
assert not subprocess.check_output(['git','diff',baseline,'--name-only','--','src','test','tools','assets'],text=True).strip()
e='artifacts/mvdm-audit/t483-b-c1-dependencies/p85-evidence/'
for p,h in load(e+'source-identities.json').items():assert sha(R/p)==h,p
m=load('assets/release/release-manifest.json');assert len(m)==11
hashes=[]
for dest in [R/'assets/release',Path('O:/winnt/system32')]:
 for row in m:
  p=dest/row['Name'];actual=sha(p);assert actual.upper()==row['Sha256'].upper(),str(p)
  hashes.append({'path':str(p),'sha256':actual})
full=load(e+'full-timings.json');assert len(full['Gates'])==20 and all(x['Passed'] for x in full['Gates'])
refs={n:f'docs/etc/evidence/m0-t483-s{n}-closure.md' for n in range(1,21)}
refs.update({9:'artifacts/t483-s9-non-mirror-audit/sequential.md',10:'docs/etc/evidence/m0-t483-s10-components.md',11:'docs/etc/evidence/m0-t483-s11-layer-boundaries.md',13:'docs/etc/evidence/m0-t483-s13-implementation-closure.md'})
for p in refs.values():assert (R/p).is_file(),p
ledgers={}
for n in ['pending-decisions.csv','review-register.csv']:
 p=R/'artifacts/mvdm-audit/t483-remaining-reaudit'/n
 with p.open(encoding='utf-8-sig',newline='') as f:rows=list(csv.DictReader(f))
 ids=[x['review_id'] for x in rows]
 ledgers[n]={'rows':len(rows),'unique_ids':len(set(ids)),'duplicate_ids':[i for i,c in collections.Counter(ids).items() if c>1], 'sha256':sha(p),'statuses':dict(collections.Counter(x.get('status',x.get('review_status','')) for x in rows))}
 # userk.h intentionally retains 21 distinct subcases under one grouped ID.
 if n=='pending-decisions.csv':
  assert all(i=='T483-R2658,R2666' for i in ledgers[n]['duplicate_ids'])
  keys=[(x['review_id'],x['path'],x['problem']) for x in rows]
  assert len(keys)==len(set(keys)),n
 else:assert len(ids)==len(set(ids)),n
inv={}
for k in ['c1','c2']:
 d=load(f'artifacts/mvdm-audit/t483-b-{k}-dependencies/file-inventory.json');inv[k]=d['counts'] if k=='c1' else d['progress']
report={'packet':'M0 T483 S21 P1','kind':'documentation-only closure verification; no rebuild/retest/republication','baseline':baseline,'product_source_build_test_asset_identity':'Unchanged from verified P85; recorded modified-source hashes rechecked individually.','publication':hashes,'package_ids':sorted({x['PackageId'] for x in m}),'Full_reused':{'gates':20,'passed':20,'elapsed_ms':full['ElapsedMs'],'scope':'Complete Windows Userenv; inherited agent environment limitation retained.'},'subtask_evidence':refs,'dependency_inventories':inv,'mirror_ledgers':ledgers,'count_limit':'Ledger rows are decision records, not current mirror hunks, file counts or all approved retentions. S8 census is historical; no fresh global mirror census performed.'}
(B/'result.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print(json.dumps({'verified_hashes':len(hashes),'subtasks':len(refs),'ledgers':ledgers,'inventories':inv},ensure_ascii=False))
