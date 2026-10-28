import csv,hashlib,json,re,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));N=('wow16/mciole/libinit.asm','wow16/mciole/MCIOLE16.DLL','wow16/mciole/mciole16.rcv','wow16/mmsystem/messages/usa/mci.rc','wow16/mmsystem/messages/usa/mmtask.rc','wow16/mmsystem/mmsystem.rc','wow16/mmsystem/mmtask/mmtask.asm','wow16/mmsystem/timea.asm')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
def inspect():
 f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};l={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))};rows=[]
 for n in N:
  hs=[sha(root/n) for root in (M,*O)];assert hs==[f[n]['current_sha256'],f[n]['opennt_sha256'],f[n]['opennt45_sha256']] and l[n]['shard']=='2' and l[n]['status']=='pending';rows.append(dict(path=n,hashes=hs))
 root=M/'wow16/mmsystem';make=(root/'makefile').read_text(encoding='latin1').lower();time=(root/'time.c').read_text(encoding='latin1').lower();timea=(root/'timea.asm').read_text(encoding='latin1').lower();mp=(root/'mmsystem.map').read_text(encoding='latin1').lower();assert '$ztime.obj: time.c' in make and 'timea.obj' not in make and 'timebeginperiod' in time and 'cproc\ttimebeginperiod' in timea and 'time.obj(time.c)' in mp and 'timebeginperiod' in mp
 task=root/'mmtask.tsk';assert task.read_bytes()==(root/'mmtask/MMTASK.TSK').read_bytes();zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);fs=[x for x in z.infolist() if not x.is_dir()];d=task.read_bytes();same=[x.filename for x in fs if x.file_size==len(d) and hashlib.sha256(z.read(x)).hexdigest().upper()==sha(task)];named=[x.filename for x in fs if x.filename.lower().endswith('/mmtask.tsk')];assert not same and not named
 binaries=[]
 for relative in ('wow16/mciole/MCIOLE16.DLL','wow16/mmsystem/MMSYSTEM.DLL'):
  p=M/relative;entry=next(x for x in fs if x.filename.lower()=='winnt/system32/'+p.name.lower());assert z.read(entry)==p.read_bytes();binaries.append(dict(source=relative,sha256=sha(p),member=entry.filename,installed_exists=(Path('O:/winnt/system32')/p.name).exists()))
 return dict(result='pass',files=rows,makefile_sha256=sha(root/'makefile'),time_c_sha256=sha(root/'time.c'),map_sha256=sha(root/'mmsystem.map'),mmtask_sha256=sha(task),mmtask_archive_equal=same,mmtask_archive_named=named,archive_sha256=sha(zpath),binaries=binaries,scope='Historical build and package identity; no guest execution, installed search path or image provenance')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
