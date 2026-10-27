import csv,pathlib,hashlib,json,collections,subprocess,io
A=pathlib.Path('artifacts/mvdm-audit');O=pathlib.Path('build/t473-s10/external-obligation-checkpoint')
def rows(p):
 with p.open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
E=rows(A/'frozen-external-audit-rows.csv');T=rows(A/'external-obligation-triage.csv');assert len(E)==len(T)==42963 and {r['id'] for r in E}=={r['id'] for r in T}
S=json.loads((A/'external-obligation-triage-summary.json').read_text());assert S['result']=='pass' and S['original_git_index_identities_verified']==42963 and S['original_working_content_rehashed']==9
assert collections.Counter(r['dependency_tier'] for r in T)==S['tiers'];assert collections.Counter(r['disposition'] for r in T)==S['dispositions'];assert S['dispositions']['excluded-provider-recipe-file-only']==9
for r in S['inputs']:assert sha(pathlib.Path(r['path']))==r['sha256']
P=json.loads((A/'dll-entry-return-result.json').read_text());assert P['result']=='pass' and len(P['runs'])==4 and all(r['exit']==0 for r in P['runs'])
assert 'false_attach=a55a0000' in P['actual'] and 'false_attach=00000000' in P['candidate'] and 'cleanups=2' in P['actual'] and 'cleanups=2' in P['candidate']
for r in P['inputs']:assert sha(pathlib.Path(r['Path']))==r['Hash']
I=json.loads((A/'dll-entry-image-result.json').read_text());assert I['result']=='pass' and I['function_bytes']==50
assert I['object_sha256']==sha(pathlib.Path('build/.ninja/msvc-x86-release/wow32/obj/wow32-dll-entry-bridge.obj')) and I['image_sha256']==sha(pathlib.Path('build/package-uuid/package-c/system32/WOW32.DLL'))
F=rows(A/'frozen-file-reconciliation.csv');M={r['relative_path']:r for r in rows(A/'combined-initial-status.csv')};assert len(F)==len(M)==4684
assert sha(A/'frozen-file-reconciliation.csv')=='F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196'
counts=dict(collections.Counter(r['status'] for r in M.values()));assert counts==dict(pending=2940,excluded=1466,partial=226,full=52)
old={r['relative_path']:r['status'] for r in csv.DictReader(io.StringIO(subprocess.check_output(['git','show','10bde6370:artifacts/mvdm-audit/combined-initial-status.csv']).decode('utf-8-sig')))}
changed=[p for p in M if M[p]['status']!=old[p]];assert set(changed)=={'softpc.new/base/video/v7_ports.c','softpc.new/base/video/vga_mode.c'} and all(old[p]=='pending' and M[p]['status']=='partial' for p in changed)
retention=rows(A/'live-candidate-source-retention.csv');assert {r['relative_path'] for r in retention}==set(changed)
for r in retention:assert sha(pathlib.Path('src/mvdm')/r['relative_path'])==r['observed_current_sha256']
drift=[]
for r in F:
 p=r['relative_path'];assert int.from_bytes(hashlib.sha256(p.encode()).digest()[:8],'big')%4==int(M[p]['shard'])
 if 'mirror-different' in r['current_identity']:assert M[p]['status']=='partial'
 src=pathlib.Path('src/mvdm')/p
 if not src.exists():assert not r['current_sha256'];continue
 h=sha(src)
 if h!=r['current_sha256']:drift.append(dict(relative_path=p,frozen=r['current_sha256'],current=h))
assert drift==json.loads((A/'initial-summary.json').read_text())['live_source_hash_drift']
full=rows(A/'full-selected-leaf.csv')+rows(A/'full-selected-declarations.csv');assert len(full)==52 and {r['relative_path'] for r in full}=={p for p in M if M[p]['status']=='full'}
for r in full:assert sha(pathlib.Path('src/mvdm')/r['relative_path'])==r['source_sha256']
result=dict(result='pass',mvdm_paths=4684,counts=counts,external_identities=42963,external_tiers=S['tiers'],external_recipe_file_exclusions=9,external_pending_contracts=42954,controlled_boolean_abi_witness=True,published_bridge_body_correspondence=True,live_candidate_partial_rows=changed,source_drift=drift,product_or_guest_changes_by_this_checkpoint=False,scope='Source triage and controlled ABI evidence; no real original heap-failure, complete loader acceptance or T closure')
(O/'result.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
