import csv,hashlib,json,struct,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));N=('wow16/timer/libinit.asm','wow16/timer/local.asm','wow16/timer/math.asm','wow16/timer/sysinfo.inc','wow16/timer/TIMER.DRV','wow16/timer/timer.inc')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
def inspect():
 f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};l={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))};rows=[]
 for n in N:
  hs=[sha(root/n) for root in (M,*O)];assert hs==[f[n]['current_sha256'],f[n]['opennt_sha256'],f[n]['opennt45_sha256']] and l[n]['shard']=='2' and l[n]['status'] in ('pending','partial');rows.append(dict(path=n,hashes=hs))
 root=M/'wow16/timer';time=M/'wow16/mmsystem/time.c';init=M/'wow16/mmsystem/init.c';api=root/'api.asm';t=time.read_text(encoding='latin1');i=init.read_text(encoding='latin1');a=api.read_text(encoding='latin1');assert 'OpenDriver(szTimerDrv' in t and '"timer"' in i and all(x in t and x in a for x in ('TDD_BEGINMINPERIOD','TDD_ENDMINPERIOD','TDD_SETTIMEREVENT','TDD_KILLTIMEREVENT'))
 zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);entry=next(x for x in z.infolist() if x.filename.lower()=='winnt/system32/timer.drv');d=z.read(entry);assert d==(root/'TIMER.DRV').read_bytes() and d[:2]==b'MZ';ne=struct.unpack_from('<I',d,60)[0];assert d[ne:ne+2]==b'NE';pointers=(ne+struct.unpack_from('<H',d,ne+0x26)[0],struct.unpack_from('<I',d,ne+0x2c)[0]);exports={}
 for p in pointers:
  while d[p]:
   n=d[p];p+=1;name=d[p:p+n].decode('latin1');p+=n;ordinal=struct.unpack_from('<H',d,p)[0];p+=2
   if ordinal:exports[name.upper()]=ordinal
 assert exports.get('WEP')==1 and exports.get('DRIVERPROC')==2
 return dict(result='pass',files=rows,time_source_sha256=sha(time),init_source_sha256=sha(init),timer_api_sha256=sha(api),archive_sha256=sha(zpath),timer_dll_sha256=hashlib.sha256(d).hexdigest().upper(),ne_names=exports,installed_exists=Path('O:/winnt/system32/TIMER.DRV').exists(),scope='Published NE exports and original source-level dispatch only; no runtime driver lookup or timer execution')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
