import csv,hashlib,json,re,struct,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));names=('romsys.def','system','system.lrf','system.rcv','vecsys.inc','windefs.inc');base='wow16/system/'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''
def inspect():
 f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};l={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))};rows=[]
 for n in names:
  rel=base+n;hs=[sha(root/rel) for root in (M,*O)];assert hs==[f[rel]['current_sha256'],f[rel]['opennt_sha256'],f[rel]['opennt45_sha256']] and l[rel]['shard']=='2' and l[rel]['status']=='pending';rows.append(dict(path=rel,hashes=hs))
 assert rows[2]['hashes'][1:]==['',''];root=M/base;mk=(root/'makefile').read_text(encoding='latin1').lower();alt=(root/'romsys').read_text(encoding='latin1').lower();hp=(root/'hpsystem').read_text(encoding='latin1').lower();assert all(s in mk for s in ('system.lrf: makefile','system.drv: system.obj timer.obj system.lrf system.res','rc16 -t system.res system.exe')) and 'romsys.def' in alt and 'envect.asm vecsys.inc' in hp
 src=root/'system.drv';dll=src.read_bytes();zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);entry=next(x for x in z.infolist() if x.filename.lower()=='winnt/system32/system.drv');assert z.read(entry)==dll==Path('O:/winnt/system32/SYSTEM.DRV').read_bytes();assert dll[:2]==b'MZ';ne=struct.unpack_from('<I',dll,60)[0];assert dll[ne:ne+2]==b'NE';pointers=(ne+struct.unpack_from('<H',dll,ne+0x26)[0],struct.unpack_from('<I',dll,ne+0x2c)[0]);exports={}
 for p in pointers:
  while dll[p]:
   n=dll[p];p+=1;name=dll[p:p+n].decode('latin1');p+=n;ordinal=struct.unpack_from('<H',dll,p)[0];p+=2
   if ordinal:exports[name.upper()]=ordinal
 expected={'INQUIRESYSTEM':1,'CREATESYSTEMTIMER':2,'KILLSYSTEMTIMER':3,'ENABLESYSTEMTIMERS':4,'DISABLESYSTEMTIMERS':5,'GETSYSTEMMSECCOUNT':6,'GET80X87SAVESIZE':7,'SAVE80X87STATE':8,'RESTORE80X87STATE':9,'WEP':10,'A20_PROC':20};assert all(exports.get(k)==v for k,v in expected.items())
 return dict(result='pass',files=rows,makefile_sha256=sha(root/'makefile'),archive_sha256=sha(zpath),system_drv_sha256=sha(src),ne_names=exports,scope='Export names and ordinal metadata only; no execution, implementation equivalence or source-to-binary provenance')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
