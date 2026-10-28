import csv,hashlib,json,re,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));base='wow16/write/';names=('dtabbtn.bms','dtabmark.bms','editsect.c','form1.c','help.c','just.bms','looks.h','sp15.bms','WRITE.EXE','write16.exe')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''
def inspect():
 f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};l={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))};rows=[]
 for n in names:
  rel=base+n;hs=[sha(root/rel) for root in (M,*O)]; assert hs==[f[rel]['current_sha256'],f[rel]['opennt_sha256'],f[rel]['opennt45_sha256']] and l[rel]['shard']=='2' and l[rel]['status']=='pending';rows.append(dict(path=rel,hashes=hs))
 root=M/base;mk=(root/'makefile').read_text(encoding='latin1').lower();link=(root/'write.lnk').read_text(encoding='latin1').lower();assert '!ifdef  dbcs' in mk and 'obj_dbcs=help.obj' in mk and r's/format\.obj/format\.obj help\.obj/' in mk and 'd_form1.c' in mk and 'editsect.obj:' in mk and 'form1.obj:' in mk and 'help.obj' not in link and 'editsect.obj' not in link and 'form1.obj' not in link
 published=Path('O:/winnt/WRITE.EXE');zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);entry=next(x for x in z.infolist() if x.filename.lower()=='winnt/write.exe');assert z.read(entry)==published.read_bytes() and sha(root/'WRITE.EXE')==sha(root/'write16.exe') and sha(published)!=sha(root/'WRITE.EXE')
 return dict(result='pass',files=rows,makefile_sha256=sha(root/'makefile'),link_sha256=sha(root/'write.lnk'),archive_sha256=sha(zpath),published_write_sha256=sha(published),archived_write_sha256=hashlib.sha256(z.read(entry)).hexdigest().upper(),scope='Historical conditional response-generation source and captured binary identities, no executed guest or build lineage')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
