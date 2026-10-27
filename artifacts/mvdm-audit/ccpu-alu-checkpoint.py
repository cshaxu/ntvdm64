import csv,pathlib,hashlib,json,collections,subprocess,io,struct
A=pathlib.Path('artifacts/mvdm-audit');B=pathlib.Path('build/t473-s10');O=B/'alu-guest'
def rows(p):
 with p.open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
F=rows(A/'frozen-file-reconciliation.csv');M={r['relative_path']:r for r in rows(A/'combined-initial-status.csv')};assert len(F)==len(M)==4684
assert sha(A/'frozen-file-reconciliation.csv')=='F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196'
counts=dict(collections.Counter(r['status'] for r in M.values()));assert counts==dict(pending=3811,excluded=626,partial=224,full=23)
drift=[]
for r in F:
 p=r['relative_path'];assert int.from_bytes(hashlib.sha256(p.encode()).digest()[:8],'big')%4==int(M[p]['shard'])
 if 'mirror-different' in r['current_identity']:assert M[p]['status']=='partial'
 src=pathlib.Path('src/mvdm')/p
 if not src.exists():assert not r['current_sha256'];continue
 h=sha(src)
 if h!=r['current_sha256']:drift.append(dict(relative_path=p,current_sha256=h))
old={r['relative_path']:r['status'] for r in csv.DictReader(io.StringIO(subprocess.check_output(['git','show','2031978ca:artifacts/mvdm-audit/combined-initial-status.csv']).decode('utf-8-sig')))}
changed=[p for p in M if M[p]['status']!=old[p]];assert len(changed)==12 and all(old[p]=='pending' and M[p]['status']=='full' for p in changed)
full=rows(A/'full-selected-leaf.csv');assert len(full)==23 and {r['relative_path'] for r in full}=={p for p in M if M[p]['status']=='full'}
for r in full:assert sha(pathlib.Path('src/mvdm')/r['relative_path'])==r['source_sha256'] and r['estimated_restored_code_pct']=='100'
G=json.loads((A/'ccpu-alu-guest.json').read_text());I=json.loads((A/'ccpu-alu-image.json').read_text());assert I['checker_sha256']==sha(pathlib.Path('tests/observation/verify_ccpu_alu_image.py'));assert I['parser_sha256']==sha(pathlib.Path('tests/observation/verify_ccpu_sign_flag_image.py'));assert I['image_sha256']==sha(pathlib.Path('build/package-uuid/package-c/system32/ntvdm.exe'))
for f in I['functions']:assert f['object_sha256']==sha(pathlib.Path('build/.ninja/msvc-x86-release/worker/obj/ccpu')/(f['name']+'.obj'));assert f['decoder_references']
assert I['decoder_object_sha256']==sha(pathlib.Path('build/.ninja/msvc-x86-release/worker/obj/ccpu/c_main.obj'))
for case in G['cases']:
 assert case['runner_context']['integrity_sid']=='S-1-16-8192'
 assert case['runner_context']['script_sha256']==sha(pathlib.Path('tests/observation/verify-ccpu-alu-guest.ps1'))
 assert 'result=exited' in case['observer'];negative=case['case'].startswith('negative')
 assert ('exit=0x00000001' if negative else 'exit=0x00000000') in case['observer']
 assert ('CCPU ALU FAIL phase=01 form=01 index=0000 reason=V' if negative else 'CCPU ALU PASS phases=48 records=1086848 instructions=3260544') in case['console']
 for r in case['inputs']:assert sha(pathlib.Path(r['Path']))==r['Hash']
 for r in case['runtime']:
  assert sha(pathlib.Path('build/package-uuid/package-c/system32')/pathlib.Path(r['Path']).name)==r['Hash'];assert not pathlib.Path(r['Path']).exists()
manifest=json.loads((A/'ccpu-alu-records.json').read_text());total=0
known={'ADD8.BIN':(0,0,0,0x44),'ADC1_8.BIN':(0,0,1,0),'DEC0_8.BIN':(0,0,255,0x94),'DEC1_8.BIN':(0,0,255,0x95),'INC1_8.BIN':(0,0,1,1),'NEG8.BIN':(0,0,0,0x44),'TEST8.BIN':(0,0,0,0x44)}
for r in manifest['files']:
 p=B/'alu-guest-r3/data'/r['name'];assert sha(p)==r['sha256'];data=p.read_bytes();width=8 if p.name.endswith('8.BIN') else 16 if p.name.endswith('16.BIN') else 32;mask=(1<<width)-1;n=0
 for lhs,rhs,result,flags in struct.iter_unpack('<IIIH',data):
  assert lhs<=mask and rhs<=mask and result<=mask and flags&~0x8d5==0
  if p.name.startswith(('AND','OR','TEST','XOR')):assert flags&0x10==0
  if p.name.startswith(('CMP','TEST')):assert result==lhs
  n+=1
 assert n==r['records'];total+=n
 if p.name in known:assert struct.unpack_from('<IIIH',data)==known[p.name]
assert total==1086848
for n in ['run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll']:
 assert sha(pathlib.Path('build/package-uuid/package-c/system32')/n)==sha(pathlib.Path('assets/release')/n)==sha(pathlib.Path('O:/winnt/system32')/n)
commands=(O/'commands.txt').read_text(encoding='utf-8-sig');assert 'CCACHE_DIR=O:/repos.hobby/ntvdm64/build/.ccache' in commands and 'CCACHE_TEMPDIR=O:/repos.hobby/ntvdm64/build/.ccache/tmp' in commands and 'CCACHE_MAXSIZE=5G' in commands
assert 'no work to do' in (O/'incremental.stdout.txt').read_text()
result=dict(result='pass',paths=4684,counts=counts,new_full_paths=changed,source_drift=drift,native_reference_records=total,actual_guest_checks=3260544,positive_and_negative_guest=True,image_correspondence=True,ten_images_unchanged=True,owned_stages_absent=True,cached_ninja_noop=True,repository_ccache_routing=True,scope='ALU leaf contracts; not whole decoder or full product regression')
(O/'checkpoint-result.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
