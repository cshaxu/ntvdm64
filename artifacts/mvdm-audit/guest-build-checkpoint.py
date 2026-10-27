import csv,pathlib,hashlib,json,collections,subprocess,io,struct,zipfile
A=pathlib.Path('artifacts/mvdm-audit');O=pathlib.Path('build/t473-s10/guest-build-checkpoint')
def rows(p):
 with p.open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
F=rows(A/'frozen-file-reconciliation.csv');M={r['relative_path']:r for r in rows(A/'combined-initial-status.csv')};assert len(F)==len(M)==4684
assert sha(A/'frozen-file-reconciliation.csv')=='F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196'
counts=dict(collections.Counter(r['status'] for r in M.values()));assert counts==dict(pending=2971,excluded=1466,partial=224,full=23)
old={r['relative_path']:r['status'] for r in csv.DictReader(io.StringIO(subprocess.check_output(['git','show','e973ae47e:artifacts/mvdm-audit/combined-initial-status.csv']).decode('utf-8-sig')))}
changed=[p for p in M if M[p]['status']!=old[p]];assert len(changed)==840 and all(old[p]=='pending' and M[p]['status']=='excluded' for p in changed)
expected={0:221,1:191,2:197,3:231};assert collections.Counter(int(M[p]['shard']) for p in changed)==expected
omf=[];other=[];drift=[];files={r['relative_path']:r for r in F}
for r in F:
 p=r['relative_path'];assert int.from_bytes(hashlib.sha256(p.encode()).digest()[:8],'big')%4==int(M[p]['shard'])
 if 'mirror-different' in r['current_identity']:assert M[p]['status']=='partial'
 src=pathlib.Path('src/mvdm')/p
 if not src.exists():assert not r['current_sha256'];continue
 h=sha(src)
 if h!=r['current_sha256']:drift.append(dict(relative_path=p,current_sha256=h))
 if p not in changed:continue
 assert h==r['current_sha256'] and r['audit_disposition']=='immutable-guest-or-guest-build-input'
 ext=src.suffix.lower();name=src.name.lower();data=src.read_bytes()
 assert ext in ('.obj','.map','.sym','.pdb','.lst','.lnk','.lrf') or name in ('makefile','sources','dirs') or p in ('wow16/ddeml/tests/ddestrs/makefile.dos','wow16/ddeml/tests/ddestrs/mk.cmd','wow16/ddeml/tests/src/makedos.bat'),p
 if ext=='.obj':
  pos=0;types=[];zero=0;bad=[]
  while pos<len(data):
   size=struct.unpack_from('<H',data,pos+1)[0];assert size>0 and pos+3+size<=len(data);record=data[pos:pos+3+size];types.append(data[pos])
   if record[-1]==0:zero+=1
   elif sum(record)&255:bad.append(pos)
   pos+=3+size
  assert types[0] in (0x80,0x82) and types[-1] in (0x8a,0x8b)
  omf.append(dict(relative_path=p,records=len(types),stored_zero_checksums=zero,nonzero_checksum_mismatches=bad))
 else:other.append(dict(relative_path=p,sha256=h,bytes=len(data)))
assert len(omf)==628
with zipfile.ZipFile('assets/winnt.zip') as z:archive_hashes={hashlib.sha256(z.read(i)).hexdigest().upper() for i in z.infolist() if not i.is_dir()}
package_hashes={sha(p) for p in pathlib.Path('build/package-uuid/package-c/system32').iterdir() if p.is_file()}
for p in changed:assert files[p]['current_sha256'] not in archive_hashes|package_hashes,p
full=rows(A/'full-selected-leaf.csv');assert len(full)==23 and {r['relative_path'] for r in full}=={p for p in M if M[p]['status']=='full'}
result=dict(result='pass',paths=4684,counts=counts,new_excluded_file_roles=840,changed_shards=expected,omf_objects=628,omf_records=sum(r['records'] for r in omf),omf_nonzero_stored_checksum_mismatches=sum(len(r['nonzero_checksum_mismatches']) for r in omf),archive_sha256=sha(pathlib.Path('assets/winnt.zip')),excluded_whole_files_absent_from_archive_and_package=True,source_drift=drift,scope='Historical file-role/provenance audit; source and guest functionality remain independently in scope')
(O/'result.json').write_text(json.dumps(result,indent=2)+'\n');(O/'omf-format-result.json').write_text(json.dumps(omf,indent=2)+'\n');print(json.dumps(result))
