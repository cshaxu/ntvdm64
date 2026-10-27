import csv,pathlib,hashlib,json,collections,subprocess,io,re
A=pathlib.Path('artifacts/mvdm-audit');O=pathlib.Path('build/t473-s10/transfer-checkpoint')
def rows(p):
 with p.open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
F=rows(A/'frozen-file-reconciliation.csv');M={r['relative_path']:r for r in rows(A/'combined-initial-status.csv')};assert len(F)==len(M)==4684
assert sha(A/'frozen-file-reconciliation.csv')=='F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196'
counts=dict(collections.Counter(r['status'] for r in M.values()));assert counts==dict(pending=2942,excluded=1466,partial=224,full=52)
old={r['relative_path']:r['status'] for r in csv.DictReader(io.StringIO(subprocess.check_output(['git','show','22528d16e:artifacts/mvdm-audit/combined-initial-status.csv']).decode('utf-8-sig')))}
changed=[p for p in M if M[p]['status']!=old[p]];assert set(changed)=={'softpc.new/base/ccpu386/'+n+e for n in ['not','xchg','lea'] for e in ['.c','.h']}
assert all(old[p]=='pending' and M[p]['status']=='full' for p in changed)
leaves=rows(A/'full-selected-leaf.csv');headers=rows(A/'full-selected-declarations.csv');assert len(leaves)==len(headers)==26
assert {r['relative_path'] for r in leaves+headers}=={p for p in M if M[p]['status']=='full'}
drift=[]
for r in F:
 p=r['relative_path'];assert int.from_bytes(hashlib.sha256(p.encode()).digest()[:8],'big')%4==int(M[p]['shard'])
 if 'mirror-different' in r['current_identity']:assert M[p]['status']=='partial'
 src=pathlib.Path('src/mvdm')/p
 if not src.exists():assert not r['current_sha256'];continue
 h=sha(src)
 if h!=r['current_sha256']:drift.append(dict(relative_path=p,current_sha256=h))
for r in leaves+headers:assert sha(pathlib.Path('src/mvdm')/r['relative_path'])==r['source_sha256'] and r['estimated_restored_code_pct']=='100'
G=json.loads((A/'ccpu-transfer-guest.json').read_text());I=json.loads((A/'ccpu-transfer-image.json').read_text());assert I['checker_sha256']==sha(pathlib.Path('tests/observation/verify_ccpu_transfer_image.py'));assert I['parser_sha256']==sha(pathlib.Path('tests/observation/verify_ccpu_sign_flag_image.py'))
assert I['image_sha256']==sha(pathlib.Path('build/package-uuid/package-c/system32/ntvdm.exe'));assert I['decoder_object_sha256']==sha(pathlib.Path('build/.ninja/msvc-x86-release/worker/obj/ccpu/c_main.obj'))
for f in I['functions']:assert f['object_sha256']==sha(pathlib.Path('build/.ninja/msvc-x86-release/worker/obj/ccpu')/(f['name']+'.obj'));assert f['decoder_references']
for c in G['cases']:
 assert c['runner_context']['integrity_sid']=='S-1-16-8192' and c['runner_context']['script_sha256']==sha(pathlib.Path('tests/observation/verify-ccpu-transfer-guest.ps1'))
 negative=c['case'].startswith('negative');assert 'result=exited' in c['observer'];assert ('exit=0x00000001' if negative else 'exit=0x00000000') in c['observer']
 assert ('CCPU TRANSFER FAIL phase=1' if negative else 'CCPU NOT XCHG LEA PASS checks=3276800') in c['console']
 for r in c['inputs']:assert sha(pathlib.Path(r['Path']))==r['Hash']
 for r in c['runtime']:assert sha(pathlib.Path('build/package-uuid/package-c/system32')/pathlib.Path(r['Path']).name)==r['Hash'];assert not pathlib.Path(r['Path']).exists()
for n in ['run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll']:
 assert sha(pathlib.Path('build/package-uuid/package-c/system32')/n)==sha(pathlib.Path('assets/release')/n)==sha(pathlib.Path('O:/winnt/system32')/n)
neg=json.loads((A/'ccpu-transfer-image-negative.json').read_text());assert neg['result']=='pass' and neg['exit']!=0 and 'len(matches) == 1' in neg['stderr']
result=dict(result='pass',paths=4684,counts=counts,new_full_files=changed,full_executable_leaves=26,full_declarations=26,actual_guest_checks=3276800,positive_and_negative_guest=True,image_and_wrong_image_control=True,owned_stages_absent=True,ten_images_unchanged=True,source_drift=drift,scope='Complete selected pure pointer leaf/declaration contracts; not all32bit inputs or whole decoder/CPU/lifecycle')
(O/'result.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
