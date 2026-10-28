"""Read-only audit of pending guest DEBUG binary and SYSVER variants."""
import csv,hashlib,json,subprocess,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
N=['dos/v86/cmd/debug/DEBUG.EXE','dos/v86/cmd/debug/sysver.fal','dos/v86/cmd/debug/sysver.tru']
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 with (A/'shard-1/file-audit.csv').open(encoding='utf-8-sig',newline='') as f:states={x['relative_path']:x for x in csv.DictReader(f)}
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:rows={x['relative_path']:x for x in csv.DictReader(f)}
 graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')];evidence=[]
 for b in graphs:
  g=(b/'build.ninja').read_text().replace('$:',':').lower();d=subprocess.check_output(['ninja','-C',str(b),'-t','deps'],text=True,encoding='utf-8',errors='replace').lower().replace('\\','/');evidence.append((b,g,d))
 entries=[]
 for n in N:
  assert states[n]['audit_status']=='待核验';r=rows[n];p=ROOT/n;assert sha(p)==r['current_sha256']==r['opennt_sha256']==r['opennt45_sha256']==sha(ORIG/n)==sha(ALT/n)
  full=p.resolve().as_posix().lower();hits=[str(b) for b,g,d in evidence if full in g or full in d];assert not hits
  entries.append(dict(path=n,sha256=sha(p),catalog_ids=r['wow_source_ids'],status=states[n]['audit_status'],host_graph_or_dep_hits=hits))
 d=ROOT/'dos/v86/cmd/debug';refs=['makefile','debug.lnk','debug.inc','debmes.asm','sysver.inc']
 for n in refs:assert sha(d/n)==sha(ORIG/'dos/v86/cmd/debug'/n)==sha(ALT/'dos/v86/cmd/debug'/n)
 m,lnk,inc,deb,selected=[(d/n).read_text(encoding='latin1').lower() for n in refs]
 assert 'dest\t=debug.exe' in m and 'link16 $(exelink) /map @debug.lnk' in m and 'debug.exe;' in lnk
 assert 'include sysver.inc' in inc and 'include sysver.inc' in deb
 assert 'sysver\t\t   equ\tfalse' in (d/'sysver.fal').read_text(encoding='latin1').lower() and 'sysver\t\t   equ\ttrue' in (d/'sysver.tru').read_text(encoding='latin1').lower() and 'equ\tfalse' in selected
 with zipfile.ZipFile('assets/winnt.zip') as z: matches=[n for n in z.namelist() if n.lower().endswith('/debug.exe')]
 installed=Path('O:/winnt/system32/DEBUG.EXE')
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,recipe_inputs=[dict(path=str(d/n),sha256=sha(d/n)) for n in refs],host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],zip_debug_matches=matches,installed_exact_path_present=installed.exists(),limitations='Graph/path and original recipe evidence only; no guest execution, DEBUG image build provenance, variant selection mechanism, or other guest media search.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
