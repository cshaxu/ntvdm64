import csv,hashlib,json,pathlib,collections,subprocess,io,re
A=pathlib.Path('artifacts/mvdm-audit')
def read(p):
 with p.open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))
frozen=read(A/'frozen-file-reconciliation.csv'); merged=read(A/'combined-initial-status.csv'); M={r['relative_path']:r for r in merged}
assert len(frozen)==len(merged)==len(M)==4684
assert {r['relative_path'] for r in frozen}==set(M)
assert hashlib.sha256((A/'frozen-file-reconciliation.csv').read_bytes()).hexdigest().upper()=='F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196'
drift=[]; expected_absent=[]
for r in frozen:
 p=r['relative_path']; assert int.from_bytes(hashlib.sha256(p.encode()).digest()[:8],'big')%4==int(M[p]['shard'])
 if 'mirror-different' in r['current_identity']:assert M[p]['status']=='partial'
 source=pathlib.Path('src/mvdm')/p
 if not source.exists():
  assert not r['current_sha256'], p
  expected_absent.append(p); continue
 h=hashlib.sha256(source.read_bytes()).hexdigest().upper()
 if h!=r['current_sha256']:drift.append(dict(relative_path=p,frozen=r['current_sha256'],current=h))
counts=dict(collections.Counter(r['status'] for r in merged));assert counts==dict(pending=3834,partial=224,excluded=626)
old={r['relative_path']:r for r in csv.DictReader(io.StringIO(subprocess.check_output(['git','show','c6628af46:artifacts/mvdm-audit/combined-initial-status.csv']).decode('utf-8-sig')))}
changes=[(p,old[p]['status'],M[p]['status']) for p in M if old[p]['status']!=M[p]['status']]
assert len(changes)==125 and all(o=='pending' and n=='excluded' and re.match(r'^softpc.new/host/inc/(mips|ppc)/(prod|pig)/',p) for p,o,n in changes)
assert collections.Counter(n for p,o,n in changes)==dict(excluded=125)
assert not ({r['relative_path'] for r in drift} & {p for p,o,n in changes})
extra=read(A/'partial-build-carriers.csv');assert len(extra)==19 and all(M[r['relative_path']]['status']=='partial' for r in extra)
result=dict(result='pass',source_paths=4684,counts=counts,classification_changes=125,nonselected_architecture_variants=125,partial_build_manifests_unchanged=19,live_source_hash_drift=drift,expected_unmirrored_paths=expected_absent,scope='Static source/platform-variant review; no runtime acceptance')
pathlib.Path('build/t473-s10/architecture-header-checkpoint/ledger-result.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result))
