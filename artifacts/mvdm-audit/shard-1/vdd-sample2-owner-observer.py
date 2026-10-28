"""Read-only sample2 VDD and 16-bit companion identity/ownership audit."""
import csv,hashlib,json,re,subprocess,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
N=['vdd/samples/sample2/16bits/16bits.asm','vdd/samples/sample2/16bits/16bits.inc','vdd/samples/sample2/vdd/vdd.def']
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
 d=ROOT/'vdd/samples/sample2';refs=['16bits/makefile','vdd/sources','vdd/vdd.c','vdd/vdd.h']
 for n in refs:assert sha(d/n)==sha(ORIG/'vdd/samples/sample2'/n)==sha(ALT/'vdd/samples/sample2'/n)
 mk=(d/'16bits/makefile').read_text(encoding='latin1').lower();src=(d/'vdd/sources').read_text(encoding='latin1').lower();asm=(d/'16bits/16bits.asm').read_text(encoding='latin1').lower();df=(d/'vdd/vdd.def').read_text(encoding='latin1').lower()
 assert '16bits.exe: 16bits.obj' in mk and '16bits.obj: 16bits.inc' in mk and re.search(r'^\s*include\s+16bits.inc',asm,re.M) and 'targettype=dynlink' in src and 'dllentry=vddinitialize' in src and re.search(r'\bexports\s+vddinitialize',df)
 guest=(d/'16bits/16bits.inc').read_text(encoding='latin1');host=(d/'vdd/vdd.h').read_text(encoding='latin1');numeric={}
 for name in ('IO_PORT_FIRST','IO_PORT_LAST','MIO_SEGMENT','MIO_PORT_FIRST','MIO_PORT_LAST','DMA_CHANNEL'):
  a=re.search(r'^\s*'+name+r'\s+equ\s+([0-9a-f]+h|[0-9]+)',guest,re.I|re.M);b=re.search(r'^#define\s+'+name+r'\s+(0x[0-9a-f]+|[0-9]+)',host,re.I|re.M);assert a and b;v=a[1];x=int(v[:-1],16) if v.lower().endswith('h') else int(v);y=int(b[1],0);assert x==y;numeric[name]=x
 with zipfile.ZipFile('assets/winnt.zip') as z:names=[n for n in z.namelist() if n.lower().endswith('/16bits.exe')]
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,recipe_inputs=[dict(path=str(d/n),sha256=sha(d/n)) for n in refs],host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],numeric_constants=numeric,zip_16bits_exe_matches=names,limitations='Source/recipe and normalized dependencies only; no historical producer provenance, VDD DLL deployment, provider behavior, IRQ/DMA or guest test.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
