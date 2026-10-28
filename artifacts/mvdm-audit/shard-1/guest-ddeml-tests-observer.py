"""Read-only role reconciliation for pending shard-1 DDEML test inputs."""
import csv,hashlib,json,subprocess,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
N=['wow16/ddeml/STRIPPED.DEF','wow16/ddeml/tests/ddestrs/ddestrs.c','wow16/ddeml/tests/ddestrs/ddestrs.rc','wow16/ddeml/tests/ddestrs/globals.h','wow16/ddeml/tests/ddestrs/mk4dos.cmd','wow16/ddeml/tests/src/client/ddemlcl.h']
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 with (A/'combined-initial-status.csv').open(encoding='utf-8-sig',newline='') as f:states={x['relative_path']:x for x in csv.DictReader(f)}
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:rows={x['relative_path']:x for x in csv.DictReader(f)}
 graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')];evidence=[]
 for b in graphs:
  g=(b/'build.ninja').read_text().replace('$:',':').lower();dep=subprocess.check_output(['ninja','-C',str(b),'-t','deps'],text=True,encoding='utf-8',errors='replace').lower().replace('\\','/');evidence.append((b,g,dep))
 entries=[]
 for n in N:
  assert states[n]['shard']=='1' and states[n]['status']=='pending';r=rows[n];p=ROOT/n;assert sha(p)==r['current_sha256']==r['opennt_sha256']==r['opennt45_sha256']==sha(ORIG/n)==sha(ALT/n)
  full=p.resolve().as_posix().lower();hits=[str(b) for b,g,d in evidence if full in g or full in d];assert not hits
  entries.append(dict(path=n,sha256=sha(p),catalog_ids=r['wow_source_ids'],initial_status=states[n]['status'],host_graph_or_dep_hits=hits))
 paths=['wow16/ddeml/makefile','wow16/ddeml/tests/ddestrs/makefile.dos','wow16/ddeml/tests/src/client/makefile.dos','wow16/ddeml/tests/ddestrs/ddestrs.h','wow16/ddeml/tests/src/client/ddemlcl.rc']
 for n in paths:assert sha(ROOT/n)==sha(ORIG/n)==sha(ALT/n)
 prod,stress,client,header,rc=[(ROOT/n).read_text(encoding='latin1').lower() for n in paths]
 assert '$(mkpublic) ddeml.def stripped.def' in prod and '$(implib) ddeml.lib stripped.def' in prod
 assert 'test   =ddestrs' in stress and '$(s3).obj: $(s3).c' in stress and '$(rcfile).res: $(rcfile).rc' in stress
 assert '#include "globals.h"' in header and 'ddemlcl.obj: ddemlcl.c infoctrl.h ddemlcl.h' in client and '#include "ddemlcl.h"' in rc
 assert 'nmake -f makefile.dos' in (ROOT/N[4]).read_text(encoding='latin1').lower()
 zpath=Path('assets/winnt.zip')
 with zipfile.ZipFile(zpath) as z:matches=[n for n in z.namelist() if n.lower().endswith('/ddeml.dll')]
 return dict(result='pass',initial_status_sha256=sha(A/'combined-initial-status.csv'),entries=entries,original_recipe_inputs=[dict(path=n,sha256=sha(ROOT/n)) for n in paths],host_graphs=[dict(path=str(b),graph_sha256=sha(b/'build.ninja')) for b in graphs],zip_ddeml_matches=matches,installed_exact_path_present=Path('O:/winnt/system32/ddeml.dll').exists(),limitations='These exact checked paths and graphs only; no guest execution, all media search, or generated DEF lineage proof.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
