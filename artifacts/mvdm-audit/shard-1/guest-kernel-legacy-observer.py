"""Read-only pending Win16 kernel historical build roles and image identity."""
import csv,hashlib,json,re,subprocess,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm');N=['wow16/kernel31/'+x for x in ('krnl286.rc','log.slm','retail3/kernstub.exe','swappro/swap.doc','w32sys.asm')]
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
  assert states[n]['audit_status']=='待核验';r=rows[n];p=ROOT/n;assert sha(p)==r['current_sha256'];references=[]
  for base,col in ((ORIG,'opennt_sha256'),(ALT,'opennt45_sha256')):
   if r[col]:assert sha(p)==r[col]==sha(base/n);references.append(str(base/n))
  entries.append(dict(path=n,sha256=sha(p),reference_paths=references,catalog_ids=r['wow_source_ids'],status=states[n]['audit_status'],valid_dependency_matches=matches[n]))
 d=ROOT/'wow16/kernel31';refs=['makefile','kernel.def','krnl386.lnk','krnl286.lnk','kernstub.asm','swappro/makefile']
 for n in refs:assert sha(d/n)==sha(ORIG/'wow16/kernel31'/n)==sha(ALT/'wow16/kernel31'/n)
 make=(d/'makefile').read_text(encoding='latin1').lower();df=(d/'kernel.def').read_text(encoding='latin1').lower();l386=(d/'krnl386.lnk').read_text(encoding='latin1').lower();l286=(d/'krnl286.lnk').read_text(encoding='latin1').lower();swap=(d/'swappro/makefile').read_text(encoding='latin1').lower()
 assert "stub 'kernstub.exe'" in df and '$(dest)\\kernstub.exe: kernstub.asm' in make and 'krnl386.res: krnl386.rc' in make and 'retail: r386' in make and 'debug: d386' in make
 assert 'w32sys.obj' not in l386 and 'w32sys.obj' not in l286 and re.search(r'\$\(dest\)\\w32sys\.obj\s+w32sys\.lst:\s+w32sys\.asm',make)
 assert 'swap.exe: swap.obj' in swap
 src=d/'retail3/krnl386.exe';installed=Path('O:/winnt/system32/KRNL386.EXE')
 with zipfile.ZipFile('assets/winnt.zip') as z:
  names=[n for n in z.namelist() if n.lower()=='winnt/system32/krnl386.exe'];assert len(names)==1;zsha=hashlib.sha256(z.read(names[0])).hexdigest().upper()
 assert installed.exists() and sha(installed)==zsha and sha(src)!=zsha
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,recipe_inputs=[dict(path=str(d/n),sha256=sha(d/n)) for n in refs],host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],guest_image=dict(retained_source_sha256=sha(src),installed_sha256=sha(installed),zip_member=names[0],zip_sha256=zsha),limitations='Current ledger/original recipes, normalized host deps and binary identity only; no guest compilation, runtime loader/behavior, binary source lineage or all variant claims.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
