import csv,pathlib,hashlib,json,re,collections,subprocess,io
A=pathlib.Path('artifacts/mvdm-audit');O=pathlib.Path('build/t473-s10/ccpu-sign-flags-checkpoint')
def rows(p):
 with p.open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
F=rows(A/'frozen-file-reconciliation.csv');M={r['relative_path']:r for r in rows(A/'combined-initial-status.csv')};assert len(F)==len(M)==4684
assert sha(A/'frozen-file-reconciliation.csv')=='F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196'
counts=dict(collections.Counter(r['status'] for r in M.values()));assert counts==dict(pending=3823,excluded=626,partial=224,full=11)
drift=[]
for r in F:
 p=r['relative_path'];assert int.from_bytes(hashlib.sha256(p.encode()).digest()[:8],'big')%4==int(M[p]['shard'])
 if 'mirror-different' in r['current_identity']:assert M[p]['status']=='partial'
 src=pathlib.Path('src/mvdm')/p
 if not src.exists():assert not r['current_sha256'];continue
 h=sha(src)
 if h!=r['current_sha256']:drift.append(dict(relative_path=p,current_sha256=h))
old={r['relative_path']:r['status'] for r in csv.DictReader(io.StringIO(subprocess.check_output(['git','show','3982b669e:artifacts/mvdm-audit/combined-initial-status.csv']).decode('utf-8-sig')))}
changed=[p for p in M if M[p]['status']!=old[p]];full=rows(A/'full-selected-leaf.csv');assert len(full)==len(changed)==11 and set(changed)=={r['relative_path'] for r in full};assert all(old[p]=='pending' and M[p]['status']=='full' for p in changed)
assert not set(changed)&{r['relative_path'] for r in drift}
G=json.loads((A/'ccpu-sign-flags-guest.json').read_text());I=json.loads((A/'ccpu-sign-flags-image.json').read_text());assert I['checker_sha256']==sha(pathlib.Path('tests/observation/verify_ccpu_sign_flag_image.py'));assert I['image_sha256']==sha(pathlib.Path('build/package-uuid/package-c/system32/ntvdm.exe'))
for f in I['functions']:assert f['object_sha256']==sha(pathlib.Path('build/.ninja/msvc-x86-release/worker/obj/ccpu')/(f['name']+'.obj'));assert f['decoder_calls']
assert I['decoder_object_sha256']==sha(pathlib.Path('build/.ninja/msvc-x86-release/worker/obj/ccpu/c_main.obj'))
for r in full:assert sha(pathlib.Path('src/mvdm')/r['relative_path'])==r['source_sha256'];assert r['estimated_restored_code_pct']=='100'
for case in G['cases']:
 assert case['runner_context']['integrity_sid']=='S-1-16-8192';assert case['runner_context']['script_sha256']==sha(pathlib.Path('tests/observation/verify-ccpu-sign-flags-guest.ps1'))
 assert 'result=exited' in case['observer'];assert ('exit=0x00000000' if not case['case'].startswith('negative') else 'exit=0x00000001') in case['observer']
 for r in case['input_hashes']:
  if not r['Path'].endswith('EMSPROBE.COM'):assert sha(pathlib.Path(r['Path']))==r['Hash']
 for r in case['runtime_hashes']:
  name=pathlib.Path(r['Path']).name;assert sha(pathlib.Path('build/package-uuid/package-c/system32')/name)==r['Hash']
  assert not pathlib.Path(r['Path']).exists(), 'Owned isolated stage not removed'
image_names=['run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll']
for n in image_names:assert sha(pathlib.Path('build/package-uuid/package-c/system32')/n)==sha(pathlib.Path('assets/release')/n)==sha(pathlib.Path('O:/winnt/system32')/n)
result=dict(result='pass',paths=4684,counts=counts,full_leaf_paths=changed,source_drift=drift,coherent_ten_images_unchanged=True,owned_stages_absent=True,scope='Source/ledger and retained actual guest/image evidence; not full product regression')
(O/'result.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
