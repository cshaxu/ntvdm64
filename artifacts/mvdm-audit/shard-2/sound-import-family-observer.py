import csv,hashlib,json,re,struct,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));N=('wow16/lib/cdllcew.lib','wow16/lib/snocrtdw.lib','wow16/lib/sound.def')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
def inspect():
 f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};l={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))};rows=[]
 for n in N:
  hs=[sha(root/n) for root in (M,*O)];assert hs==[f[n]['current_sha256'],f[n]['opennt_sha256'],f[n]['opennt45_sha256']] and l[n]['shard']=='2' and l[n]['status']=='pending';rows.append(dict(path=n,hashes=hs))
 lib=M/'wow16/lib';mk=(lib/'makefile').read_text(encoding='latin1').lower();assert 'implib sound.lib sound.def' in mk and '+sound.lib' in mk and (lib/'sound.def').read_bytes()==(M/'wow16/drivers/sound/sound.def').read_bytes()
 source=M/'wow16/drivers/sound/SOUND.DRV';zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);entry=next(x for x in z.infolist() if x.filename.lower()=='winnt/system32/sound.drv');d=z.read(entry);assert d==source.read_bytes()==Path('O:/winnt/system32/SOUND.DRV').read_bytes() and d[:2]==b'MZ';ne=struct.unpack_from('<I',d,60)[0];assert d[ne:ne+2]==b'NE';ptrs=(ne+struct.unpack_from('<H',d,ne+0x26)[0],struct.unpack_from('<I',d,ne+0x2c)[0]);exports={}
 for p in ptrs:
  while d[p]:
   n=d[p];p+=1;name=d[p:p+n].decode('latin1');p+=n;ordinal=struct.unpack_from('<H',d,p)[0];p+=2
   if ordinal:exports[name.upper()]=ordinal
 definition=(lib/'sound.def').read_text(encoding='latin1'); expected={name.upper():int(num) for name,num in re.findall(r'(?im)^\s*(\w+)\s+@(\d+)',definition)};assert len(expected)==18 and all(exports.get(k)==v for k,v in expected.items());assert exports['WEP']==19
 return dict(result='pass',files=rows,makefile_sha256=sha(lib/'makefile'),archive_sha256=sha(zpath),sound_drv_sha256=sha(source),ne_names=exports,definition_named_ordinals=expected,scope='Captured guest import-library source edge and published NE name/ordinals; no sound execution or source build provenance')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
