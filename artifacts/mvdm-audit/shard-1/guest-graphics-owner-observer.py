"""Read-only current pending GRAPHICS guest image and build dependencies."""
import csv,hashlib,json,re,subprocess,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
N=['dos/v86/cmd/graphics/GRAPHICS.COM','dos/v86/cmd/graphics/grcommon.ext','dos/v86/cmd/graphics/grpattrn.str']
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
 d=ROOT/'dos/v86/cmd/graphics';refs=['makefile','graphics.lnk','grbwprt.asm','grcolprt.asm','grctrl.asm','grload2.asm','grload3.asm','grpattrn.asm']
 for n in refs:assert sha(d/n)==sha(ORIG/'dos/v86/cmd/graphics'/n)==sha(ALT/'dos/v86/cmd/graphics'/n)
 make=(d/'makefile').read_text(encoding='latin1').lower();assert 'dest1   =graphics.com' in make and 'grbwprt.asm grcommon.ext' in make and 'grpattrn.obj: grpattrn.asm grpattrn.str' in make and 'link16 @graphics.lnk' in make
 for n in refs[2:]:assert re.search(r'^\s*include\s+grpattrn\.str', (d/n).read_text(encoding='latin1'), re.I|re.M)
 asm=[p for p in d.glob('*.asm') if re.search(r'^\s*include\s+grcommon\.ext\b',p.read_text(encoding='latin1'),re.I|re.M)]
 with zipfile.ZipFile('assets/winnt.zip') as z:
  matches=[n for n in z.namelist() if n.lower()=='winnt/system32/graphics.com'];assert len(matches)==1;zsha=hashlib.sha256(z.read(matches[0])).hexdigest().upper()
 installed=Path('O:/winnt/system32/GRAPHICS.COM');assert installed.exists() and sha(installed)==zsha==sha(ROOT/N[0])
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,recipe_inputs=[dict(path=str(d/n),sha256=sha(d/n)) for n in refs],host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],grcommon_ext_direct_asm_include=[str(p) for p in asm],guest_image=dict(installed_path=str(installed),sha256=sha(installed),zip_member=matches[0],zip_sha256=zsha),limitations='Recipe and exact image identity only; no source-to-binary lineage, guest execution, printer or media behavior.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
