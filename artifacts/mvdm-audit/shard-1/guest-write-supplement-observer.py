"""Read-only current pending Win16 Write historical inputs and build roles."""
import csv,hashlib,json,re,subprocess,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
N=['wow16/write/aaa.c','wow16/write/center.bms','wow16/write/editftn.c','wow16/write/edm.edm','wow16/write/globdefs.itl','wow16/write/kmakend.bat','wow16/write/ltabbtn.bms','wow16/write/write.sed']
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
 d=ROOT/'wow16/write';refs=['makefile','write.lnk','write.rc','readme.txt','kmake.bat'];
 for n in refs:assert sha(d/n)==sha(ORIG/'wow16/write'/n)==sha(ALT/'wow16/write'/n)
 make,lnk,rc,readme,kmake=[(d/n).read_text(encoding='latin1').lower() for n in refs]
 assert 'aaa.obj:       aaa.c' in make and 'editftn.obj:   editftn.c' in make and 'aaa.obj' not in lnk and 'editftn.obj' not in lnk
 assert 'kmakend %1' in kmake and re.search(r'^rem\s+\*+\s+command /c kmakend',kmake,re.M)
 assert 'edm.edm' in readme and 'have n-o-t been updated yet' in readme
 assert 'cgabtns.bmp' in rc and 'vgabtns.bmp' in rc and 'center.bms' not in rc and 'ltabbtn.bms' not in rc
 assert 'sysver' not in (d/'write.sed').read_text(encoding='latin1').lower()
 with zipfile.ZipFile('assets/winnt.zip') as z:
  matches=[n for n in z.namelist() if n.lower()=='winnt/write.exe'];assert len(matches)==1;zsha=hashlib.sha256(z.read(matches[0])).hexdigest().upper()
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,original_context=[dict(path=str(d/n),sha256=sha(d/n)) for n in refs],host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],guest_image=dict(source_path=str(d/'WRITE.EXE'),source_sha256=sha(d/'WRITE.EXE'),archive_member=matches[0],archive_sha256=zsha,source_equals_archive=sha(d/'WRITE.EXE')==zsha),limitations='Original recipe and four host graph observations only; no guest execution, resource lineage, legacy selection exhaustiveness or arbitrary media inventory.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
