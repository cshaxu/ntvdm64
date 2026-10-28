"""Read-only Win16 system historical Vectra/ROM declaration roles."""
import csv,hashlib,json,re,subprocess,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm');N=['wow16/system/equate.inc','wow16/system/romstuff.inc']
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def norm(p):return str(Path(p).resolve()).replace('\\','/').casefold()
def inspect():
 with (A/'shard-1/file-audit.csv').open(encoding='utf-8-sig',newline='') as f:states={x['relative_path']:x for x in csv.DictReader(f)}
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:rows={x['relative_path']:x for x in csv.DictReader(f)}
 graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')];matches={n:[] for n in N};wanted={norm(ROOT/n):n for n in N}
 for b in graphs:
  deps=subprocess.check_output(['ninja','-C',str(b),'-t','deps'],text=True,encoding='utf-8',errors='replace');owner=None;valid=False
  for line in deps.splitlines():
   if not line.startswith(' '):owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
   elif valid:
    raw=line.strip();p=Path(raw);key=norm(p if p.is_absolute() else b/p)
    if key in wanted:matches[wanted[key]].append(dict(build=str(b),object=owner,raw=raw))
 entries=[]
 for n in N:
  assert states[n]['audit_status']=='待核验';r=rows[n];p=ROOT/n;assert sha(p)==r['current_sha256']==r['opennt_sha256']==r['opennt45_sha256']==sha(ORIG/n)==sha(ALT/n)
  entries.append(dict(path=n,sha256=sha(p),catalog_ids=r['wow_source_ids'],status=states[n]['audit_status'],valid_dependency_matches=matches[n]))
 d=ROOT/'wow16/system';refs=['makefile','system.lrf','envect.asm','system.asm','system.inc','timer.asm']
 for n in ('makefile','envect.asm','system.asm','system.inc','timer.asm'):assert sha(d/n)==sha(ORIG/'wow16/system'/n)==sha(ALT/'wow16/system'/n)
 make=(d/'makefile').read_text(encoding='latin1').lower();link=(d/'system.lrf').read_text(encoding='latin1').lower();env=(d/'envect.asm').read_text(encoding='latin1').lower();sysinc=(d/'system.inc').read_text(encoding='latin1').lower()
 assert re.search(r'^\s*include\s+equate\.inc',env,re.M) and 'system.obj timer.obj' in link and 'envect.obj' not in link and 'system.drv: system.obj timer.obj system.lrf system.res' in make
 assert '; picked off romstuff.inc' in sysinc and 'dseg equ data' in sysinc and 'doffset equ <dataoffset>' in sysinc
 src=d/'SYSTEM.DRV';installed=Path('O:/winnt/system32/SYSTEM.DRV')
 with zipfile.ZipFile('assets/winnt.zip') as z:
  names=[n for n in z.namelist() if n.lower()=='winnt/system32/system.drv'];assert len(names)==1;zsha=hashlib.sha256(z.read(names[0])).hexdigest().upper()
 assert sha(src)==sha(installed)==zsha
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,recipe_inputs=[dict(path=str(d/n),sha256=sha(d/n)) for n in refs],host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],guest_image=dict(source_sha256=sha(src),installed_sha256=sha(installed),zip_member=names[0],zip_sha256=zsha),limitations='Source, recipe and host dependency evidence only; no guest build provenance, Vectra/ROM feature activation or SYSTEM runtime behavior.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
