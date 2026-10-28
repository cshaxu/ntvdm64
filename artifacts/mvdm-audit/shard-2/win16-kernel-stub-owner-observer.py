import csv,hashlib,json,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));N=('wow16/bin/winstub.exe','wow16/kernel31/KRNL386.RES','wow16/kernel31/ksource.txt','wow16/kernel31/newexe.doc','wow16/kernel31/retail3/krnl386.exe')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''
def inspect():
 f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};l={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))};rows=[]
 for n in N:
  hs=[sha(root/n) for root in (M,*O)];assert hs==[f[n]['current_sha256'],f[n]['opennt_sha256'],f[n]['opennt45_sha256']] and l[n]['shard']=='2' and l[n]['status'] in ('pending','partial');rows.append(dict(path=n,hashes=hs))
 root=M/'wow16/kernel31';make=(root/'makefile').read_text(encoding='latin1').lower();assert '386p=1 dest=retail3' in make and 'krnl386.res' in make and 'rc16 -t ..\\krnl386.res kernel.exe' in make
 zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);fs=[x for x in z.infolist() if not x.is_dir()];stub=(M/N[0]).read_bytes();assert stub[:2]==b'MZ' and len(stub)>64;images=[]
 for member,path in (('WINNT/WRITE.EXE','O:/winnt/WRITE.EXE'),('WINNT/SYSTEM32/COMM.DRV','O:/winnt/system32/COMM.DRV'),('WINNT/SYSTEM32/KRNL386.EXE','O:/winnt/system32/KRNL386.EXE')):
  e=next(x for x in fs if x.filename.lower()==member.lower());d=z.read(e);assert d==Path(path).read_bytes() and d[:2]==b'MZ'; images.append(dict(member=e.filename,sha256=hashlib.sha256(d).hexdigest().upper(),mz_payload_matches_stub=d[64:len(stub)]==stub[64:]))
 assert images[0]['mz_payload_matches_stub'] and images[1]['mz_payload_matches_stub'] and not images[2]['mz_payload_matches_stub'] and images[2]['sha256']!=rows[4]['hashes'][0]
 return dict(result='pass',files=rows,makefile_sha256=sha(root/'makefile'),archive_sha256=sha(zpath),stub_length=len(stub),stub_payload_compared_bytes=len(stub)-64,release_images=images,scope='MZ payload sample, archival and package identity; no kernel source-to-image proof, run-time load or behavior')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
