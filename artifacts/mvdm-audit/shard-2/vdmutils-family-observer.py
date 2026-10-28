import csv,hashlib,json,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));N=('vdmutils/graftabl/graftabl.h','vdmutils/graftabl/obj/i386/graftabl.com','vdmutils/pifedit/pif.def','vdmutils/pifedit/pifedit.c','vdmutils/win/obj/i386/win.com')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''
def inspect():
 f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};l={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))};rows=[];graphs=[]
 for b in ('worker','wow32'):
  p=Path('build/.ninja/msvc-x86-release')/b/'build.ninja'; g=p.read_text().replace('$:',':').replace('\\','/').lower();graphs.append(dict(path=str(p),sha256=sha(p)));assert all(str((M/n).resolve()).replace('\\','/').lower() not in g for n in N)
 for n in N:
  hs=[sha(root/n) for root in (M,*O)];assert hs==[f[n]['current_sha256'],f[n]['opennt_sha256'],f[n]['opennt45_sha256']] and l[n]['shard']=='2' and l[n]['status']=='pending';rows.append(dict(path=n,hashes=hs))
 assert rows[1]['hashes'][1:]==['',''] and rows[4]['hashes'][1:]==['',''];graft=(M/'vdmutils/graftabl/graftabl.c').read_text(encoding='latin1').lower();gs=(M/'vdmutils/graftabl/sources').read_text(encoding='latin1').lower();pifs=(M/'vdmutils/pifedit/sources').read_text(encoding='latin1').lower();pif=(M/'vdmutils/pifedit/pifedit.c').read_text(encoding='latin1').lower();wins=(M/'vdmutils/win/sources').read_text(encoding='latin1').lower();win=(M/'vdmutils/win/win.c').read_text(encoding='latin1').lower();assert '#include "graftabl.h"' in graft and 'sources=graftabl.c graftabl.rc' in gs and 'pifedit.c' in pifs and 'winmain(' in pif and 'targetname=win' in wins and 'createprocess' in win
 zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);fs=[x for x in z.infolist() if not x.is_dir()];comparison=[]
 for idx in (1,4):
  p=M/N[idx];d=p.read_bytes();equal=[x.filename for x in fs if x.file_size==len(d) and hashlib.sha256(z.read(x)).hexdigest().upper()==rows[idx]['hashes'][0]];named=[x.filename for x in fs if x.filename.replace('\\','/').lower().endswith('/'+p.name.lower())];installed=Path('O:/winnt/system32')/p.name;assert not equal and not named and not installed.exists();comparison.append(dict(path=N[idx],zip_identical=equal,zip_named=named,installed_exists=installed.exists()))
 return dict(result='pass',files=rows,graphs=graphs,archive_sha256=sha(zpath),package_comparison=comparison,scope='Historical standalone utility source and current-only binary identity; no execution, provenance, or universal capability conclusion')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
