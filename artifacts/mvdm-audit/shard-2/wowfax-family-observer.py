import csv,hashlib,json,re,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));N=('fax/dummy.ppd','fax/faxcomm.h','fax/wowfax/wowfax.def','fax/wowfaxui/wfsheets.c','fax/wowfaxui/wfsheets.h','fax/wowfaxui/wowfaxui.c','fax/wowfaxui/wowfaxui.h')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''
def inspect():
 f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};l={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))};rows=[];graphs=[]
 for b in ('worker','wow32'):
  p=Path('build/.ninja/msvc-x86-release')/b/'build.ninja';g=p.read_text().replace('$:',':').replace('\\','/').lower();graphs.append(dict(path=str(p),sha256=sha(p)));assert all(str((M/n).resolve()).replace('\\','/').lower() not in g for n in N)
 for n in N:
  hs=[sha(root/n) for root in (M,*O)];assert hs==[f[n]['current_sha256'],f[n]['opennt_sha256'],f[n]['opennt45_sha256']] and l[n]['shard']=='2' and l[n]['status']=='pending';rows.append(dict(path=n,hashes=hs))
 root=M/'fax';ui=(root/'wowfaxui/sources').read_text(encoding='latin1').lower();drv=(root/'wowfax/sources').read_text(encoding='latin1').lower();uih=(root/'wowfaxui/wowfaxui.h').read_text(encoding='latin1').lower();ddh=(root/'wowfax/wowfaxdd.h').read_text(encoding='latin1').lower();sheet=(root/'wowfaxui/wfsheets.c').read_text(encoding='latin1').lower();uis=(root/'wowfaxui/wowfaxui.c').read_text(encoding='latin1').lower();inf=(root/'printer.inf').read_text(encoding='latin1').lower();assert all(s in ui for s in ('targetname=wowfaxui','wfsheets.c','wowfaxui.c')) and 'targetname=wowfax' in drv and '#include "faxcomm.h"' in uih and '#include "faxcomm.h"' in ddh and '#include "wfsheets.h"' in sheet and '#include "wowfaxui.h"' in uis and 'wowfaxui.dll' in inf and 'wowfax.dll' in inf
 zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);zipmatches=[x for x in z.namelist() if any(x.lower().endswith('/'+name) for name in ('wowfax.dll','wowfaxui.dll','dummy.ppd'))];assert not zipmatches
 return dict(result='pass',sources=rows,graphs=graphs,archive_sha256=sha(zpath),archive_named_fax_resources=zipmatches,dummy_contents=(root/'dummy.ppd').read_bytes().decode('latin1'),scope='Original source edges, graph and archive name snapshot only; separate driver runtime and alternate installations not tested')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
