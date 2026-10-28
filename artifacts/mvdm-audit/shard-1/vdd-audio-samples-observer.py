"""Read-only identity/build census of pending original sample sound VDDs."""
import csv,hashlib,json,subprocess,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm');N=['vdd/samples/adlibvdd/vdd.c','vdd/samples/vsbd/vdd.c']
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def norm(p):return str(Path(p).resolve()).replace('\\','/').casefold()
def inspect():
 with (A/'shard-1/file-audit.csv').open(encoding='utf-8-sig',newline='') as f:states={x['relative_path']:x for x in csv.DictReader(f)}
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:rows={x['relative_path']:x for x in csv.DictReader(f)}
 graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')];matches={n:[] for n in N};graphhits={n:[] for n in N};wanted={norm(ROOT/n):n for n in N}
 for b in graphs:
  graph=(b/'build.ninja').read_text().replace('$:',':').lower()
  for n in N:
   if (ROOT/n).resolve().as_posix().lower() in graph:graphhits[n].append(str(b))
  deps=subprocess.check_output(['ninja','-C',str(b),'-t','deps'],text=True,encoding='utf-8',errors='replace');owner=None;valid=False
  for line in deps.splitlines():
   if not line.startswith(' '):owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
   elif valid:
    raw=line.strip();p=Path(raw);key=norm(p if p.is_absolute() else b/p)
    if key in wanted:matches[wanted[key]].append(dict(build=str(b),object=owner,raw=raw))
 entries=[]
 for n in N:
  assert states[n]['audit_status']=='待核验';r=rows[n];p=ROOT/n;assert sha(p)==r['current_sha256']==r['opennt_sha256']==r['opennt45_sha256']==sha(ORIG/n)==sha(ALT/n)
  rel=str(Path(n).parent/'sources').replace('\\','/');source=ROOT/rel;assert sha(source)==sha(ORIG/rel)==sha(ALT/rel);s=source.read_text(encoding='latin1').lower();assert 'targettype=dynlink' in s and 'sources=vdd.c' in s and 'dllentry=dllentrypoint' in s
  entries.append(dict(path=n,sha256=sha(p),catalog_ids=r['wow_source_ids'],status=states[n]['audit_status'],sources_path=rel,sources_sha256=sha(source),target_name=next(l.split('=',1)[1].strip() for l in s.splitlines() if l.startswith('targetname=')),exact_graph_hits=graphhits[n],valid_dependency_matches=matches[n]))
 with zipfile.ZipFile('assets/winnt.zip') as z: names=[x for x in z.namelist() if x.lower().endswith(('/adlibvdd.dll','/vsbd.dll'))]
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],zip_members=names,exact_paths=[dict(path=str(Path(base)/name),present=(Path(base)/name).exists()) for base in ('assets/release','O:/winnt/system32') for name in ('adlibvdd.dll','vsbd.dll')],limitations='Historical sample source/recipes and checked graph/path census only; no DLL provenance, audio hardware, VDD loading, lifecycle or functional execution proof.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
