import csv,pathlib,hashlib,json,collections,subprocess,io,re
A=pathlib.Path('artifacts/mvdm-audit');O=pathlib.Path('build/t473-s10/leaf-header-checkpoint')
def rows(p):
 with p.open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
F=rows(A/'frozen-file-reconciliation.csv');M={r['relative_path']:r for r in rows(A/'combined-initial-status.csv')};assert len(F)==len(M)==4684
assert sha(A/'frozen-file-reconciliation.csv')=='F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196'
counts=dict(collections.Counter(r['status'] for r in M.values()));assert counts==dict(pending=2948,excluded=1466,partial=224,full=46)
old={r['relative_path']:r['status'] for r in csv.DictReader(io.StringIO(subprocess.check_output(['git','show','a7e2fd6a2:artifacts/mvdm-audit/combined-initial-status.csv']).decode('utf-8-sig')))}
changed=[p for p in M if M[p]['status']!=old[p]];headers=rows(A/'full-selected-declarations.csv');leaves=rows(A/'full-selected-leaf.csv');assert len(changed)==len(headers)==len(leaves)==23
assert {r['relative_path'] for r in headers}==set(changed) and all(old[p]=='pending' and M[p]['status']=='full' for p in changed)
assert {r['relative_path'] for r in headers+leaves}=={p for p in M if M[p]['status']=='full'}
drift=[]
for r in F:
 p=r['relative_path'];assert int.from_bytes(hashlib.sha256(p.encode()).digest()[:8],'big')%4==int(M[p]['shard'])
 if 'mirror-different' in r['current_identity']:assert M[p]['status']=='partial'
 src=pathlib.Path('src/mvdm')/p
 if not src.exists():assert not r['current_sha256'];continue
 h=sha(src)
 if h!=r['current_sha256']:drift.append(dict(relative_path=p,current_sha256=h))
for r in headers:
 p=pathlib.Path('src/mvdm')/r['relative_path'];assert sha(p)==r['source_sha256'];assert r['estimated_restored_code_pct']=='100';assert M[r['provider']]['status']=='full'
 body=re.sub(r'/\*.*?\*/','',p.read_text(),flags=re.S).strip();assert body.startswith('IMPORT VOID ') and body.count('IMPORT')==1 and body.endswith(';')
abi=json.loads((A/'ccpu-leaf-header-abi-result.json').read_text());assert abi['result']=='pass' and {r['case']:r['exit'] for r in abi['cases']}==dict(normal=0,negative=2)
assert 'C4113' in abi['negative_compile_output'] and 'differs in parameter lists' in abi['negative_compile_output']
assert not re.search(r'(?:error|warning) C\d+',abi['normal_compile_output'])
for r in abi['inputs']:assert sha(pathlib.Path(r['Path']))==r['Hash']
for r in headers:
 path=r['relative_path'].replace('/','\\').lower();text=abi['normal_compile_output'].replace('/','\\').lower();assert path in text
result=dict(result='pass',paths=4684,counts=counts,new_full_declarations=changed,full_executable_leaves=23,full_single_declarations=23,source_drift=drift,compile_normal=True,compile_incompatible_signature_rejected=True,product_or_guest_changed=False,scope='Declaration ABI closure backed by prior real provider execution; not new instruction or whole CPU coverage')
(O/'result.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
