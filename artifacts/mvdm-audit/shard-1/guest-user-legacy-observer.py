"""Read-only current-pending WOW16 USER guest-source reconciliation."""
import csv,hashlib,json,re,subprocess,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
N=['wow16/user/comdev.c','wow16/user/comdevi.h','wow16/user/edmlonce.c','wow16/user/readme.txt','wow16/user/usermenu.inc','wow16/user/wmsyserr.c']
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 with (A/'shard-1/file-audit.csv').open(encoding='utf-8-sig',newline='') as f:states={x['relative_path']:x for x in csv.DictReader(f)}
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:rows={x['relative_path']:x for x in csv.DictReader(f)}
 graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')];evidence=[]
 for b in graphs:
  g=(b/'build.ninja').read_text().replace('$:',':').lower();dep=subprocess.check_output(['ninja','-C',str(b),'-t','deps'],text=True,encoding='utf-8',errors='replace').lower().replace('\\','/');evidence.append((b,g,dep))
 entries=[]
 for n in N:
  assert states[n]['audit_status']=='待核验';r=rows[n];p=ROOT/n;assert sha(p)==r['current_sha256']==r['opennt_sha256']==r['opennt45_sha256']==sha(ORIG/n)==sha(ALT/n)
  full=p.resolve().as_posix().lower();hits=[str(b) for b,g,dep in evidence if full in g or full in dep];assert not hits
  entries.append(dict(path=n,sha256=sha(p),catalog_ids=r['wow_source_ids'],status=states[n]['audit_status'],host_graph_or_dep_hits=hits))
 d=ROOT/'wow16/user';refs=['makefile','k/daytona/userk.lrf','layer.asm','wowcomm.c']
 for n in ('makefile','layer.asm','wowcomm.c'):assert sha(d/n)==sha(ORIG/'wow16/user'/n)
 make=(d/'makefile').read_text(encoding='latin1').lower();lnk=(d/'k/daytona/userk.lrf').read_text(encoding='latin1').lower();layer=(d/'layer.asm').read_text(encoding='latin1').lower()
 for n in ('comdev','edmlonce','wmsyserr'):assert not re.search(r'\b'+n+r'\.obj\b',lnk)
 assert '#include "comdevi.h"' in (d/'comdev.c').read_text(encoding='latin1').lower()
 assert re.search(r'^\s*;\s*include\s+usermenu\.inc',layer,re.M) and 'wowcomm.obj' in make and 'wowcomm.obj' in lnk
 with zipfile.ZipFile('assets/winnt.zip') as z:
  names=[n for n in z.namelist() if n.lower()=='winnt/system32/user.exe'];assert len(names)==1;zsha=hashlib.sha256(z.read(names[0])).hexdigest().upper()
 installed=Path('O:/winnt/system32/USER.EXE');image=d/'k/daytona/USER.EXE';assert installed.exists() and sha(installed)==sha(image)==zsha
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,recipe_inputs=[dict(path=str(d/n),sha256=sha(d/n)) for n in refs],host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],guest_image=dict(source=str(image),source_sha256=sha(image),installed_sha256=sha(installed),zip_member=names[0],zip_sha256=zsha),limitations='Original source/list, selected graphs and package identity only; no guest source-image lineage, alternate profile resolution, USER execution or feature equivalence proof.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
