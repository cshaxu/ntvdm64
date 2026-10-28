"""Read-only current-pending DOS keyboard source/profile reconciliation."""
import csv,hashlib,json,re,subprocess,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm');SUP=Path('O:/repos.external/opennt-src-2/nt/private/mvdm')
N=['dos/v86/dev/keyboard/KEYBOARD.EXS','dos/v86/dev/keyboard/kdffr120.asm','dos/v86/dev/keyboard/kdfjp.asm','dos/v86/dev/keyboard/kdfru093.asm','dos/v86/dev/keyboard/kdfuk.asm','dos/v86/dev/keyboard/kdfur095.asm','dos/v86/dev/keyboard/kdfyc118.asm']
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 with (A/'shard-1/file-audit.csv').open(encoding='utf-8-sig',newline='') as f:states={x['relative_path']:x for x in csv.DictReader(f)}
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:rows={x['relative_path']:x for x in csv.DictReader(f)}
 graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')];evidence=[]
 for b in graphs:
  g=(b/'build.ninja').read_text().replace('$:',':').lower();d=subprocess.check_output(['ninja','-C',str(b),'-t','deps'],text=True,encoding='utf-8',errors='replace').lower().replace('\\','/');evidence.append((b,g,d))
 entries=[]
 for n in N:
  assert states[n]['audit_status']=='待核验';r=rows[n];p=ROOT/n;assert sha(p)==r['current_sha256'];reference=SUP/n if not r['opennt_sha256'] else ORIG/n;assert sha(p)==sha(reference)
  if r['opennt_sha256']:assert sha(p)==r['opennt_sha256']==r['opennt45_sha256']==sha(ALT/n)
  full=p.resolve().as_posix().lower();hits=[str(b) for b,g,d in evidence if full in g or full in d];assert not hits
  entries.append(dict(path=n,sha256=sha(p),reference=str(reference),catalog_ids=r['wow_source_ids'],status=states[n]['audit_status'],host_graph_or_dep_hits=hits))
 d=ROOT/'dos/v86/dev/keyboard';refs=['makefile','keyboard.lnk']
 for n in refs:assert sha(d/n)==sha(ORIG/'dos/v86/dev/keyboard'/n)==sha(ALT/'dos/v86/dev/keyboard'/n)
 make=(d/'makefile').read_text(encoding='latin1').lower();lnk=(d/'keyboard.lnk').read_text(encoding='latin1').lower();assert 'keyboard.exs:' in make and 'link16 @keyboard.lnk' in make and 'keyboard.exs;' in lnk and '# kdfjp.obj:' in make
 selected=[]
 for n in N[1:]:
  stem=Path(n).stem.lower();assert not re.search(r'\b'+re.escape(stem)+r'\.obj\b',lnk);assert re.search(r'^\s*include\s+keybshar\.inc', (ROOT/n).read_text(encoding='latin1'),re.I|re.M);assert re.search(r'^\s*include\s+keybmac\.inc',(ROOT/n).read_text(encoding='latin1'),re.I|re.M);selected.append(dict(source=n,exact_object_in_link=False))
 with zipfile.ZipFile('assets/winnt.zip') as z:
  names=[n for n in z.namelist() if n.lower()=='winnt/system32/keyboard.sys'];assert len(names)==1;zsha=hashlib.sha256(z.read(names[0])).hexdigest().upper()
 installed=Path('O:/winnt/system32/KEYBOARD.SYS');sys=d/'KEYBOARD.SYS';assert installed.exists() and sha(installed)==sha(sys)==zsha
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,recipe_inputs=[dict(path=str(d/n),sha256=sha(d/n)) for n in refs],variant_selection=selected,host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],guest_image=dict(source_sys_sha256=sha(sys),installed_sha256=sha(installed),zip_member=names[0],zip_sha256=zsha,exs_sha256=sha(d/'KEYBOARD.EXS')),limitations='Historical list and exact file identity only; no build provenance, keyboard layout equivalence, system configuration, real input or alternate image verification.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
