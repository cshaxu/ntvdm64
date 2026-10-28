"""Read-only role/identity reconciliation for shard 1 Win16 Toolhelp inputs."""
import csv,hashlib,json,re,subprocess,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');D=Path('src/mvdm/wow16/toolhelp');R=Path('O:/repos.external/OpenNT/base/mvdm/wow16/toolhelp');S=Path('O:/repos.external/opennt-src-2/nt/private/mvdm/wow16/toolhelp')
N=['global.c','int2.OBJ','local.c','local.obj','memman.asm','memory.asm','notify1.c','notify1.obj','stack2.asm','terminat.asm','timer.OBJ','toolhelp.obj','toolhelp.rcv','toolpriv.inc','usergdi2.OBJ']
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:rows={x['relative_path']:x for x in csv.DictReader(f)}
 make=(D/'makefile').read_text(encoding='latin1');assert (D/'makefile').read_bytes()==(R/'makefile').read_bytes()
 entries=[];graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')];evidence=[]
 for b in graphs:
  g=(b/'build.ninja').read_text().replace('$:',':').lower();deps=subprocess.check_output(['ninja','-C',str(b),'-t','deps'],text=True,encoding='utf-8',errors='replace').lower();evidence.append((b,g,deps))
 for n in N:
  p=D/n;rel='wow16/toolhelp/'+n;r=rows[rel];assert sha(p)==r['current_sha256'];historical=R/n if r['opennt_sha256'] else S/n;assert sha(p)==sha(historical)
  if r['opennt_sha256']:assert sha(p)==r['opennt_sha256']==r['opennt45_sha256']
  refs=[i for i,line in enumerate(make.splitlines(),1) if re.search(r'(?<![\w.])'+re.escape(n)+r'(?![\w.])',line,re.I)];assert refs,(n,'missing original recipe reference')
  full=p.resolve().as_posix().lower(); hits=[str(b) for b,g,deps in evidence if full in g or full in deps.replace('\\','/')];assert not hits,(n,hits)
  entries.append(dict(path=rel,sha256=sha(p),comparison=str(historical),comparison_sha256=sha(historical),source_identity=r['current_identity'],recipe_reference_lines=refs,host_graphs_or_deps_matching=hits))
 dll=D/'TOOLHELP.DLL';installed=Path('O:/winnt/system32/TOOLHELP.DLL');zpath=Path('assets/winnt.zip')
 with zipfile.ZipFile(zpath) as z:
  names=[x for x in z.namelist() if x.lower()=='winnt/system32/toolhelp.dll'];assert len(names)==1;zdata=z.read(names[0]);zsha=hashlib.sha256(zdata).hexdigest().upper()
 return dict(result='pass',entries=entries,makefile_sha256=sha(D/'makefile'),host_graphs=[dict(path=str(b),graph_sha256=sha(b/'build.ninja')) for b in graphs],guest_dll=dict(source_path=str(dll),source_sha256=sha(dll),zip_member=names[0],zip_member_sha256=zsha,zip_matches_source=sha(dll)==zsha,installed_present=installed.exists(),installed_sha256=sha(installed) if installed.exists() else None),limitations='Recipe and exact artifact identity only; no object-to-DLL provenance, guest rebuild, API execution, entire build closure, or archive deployment assertion.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
