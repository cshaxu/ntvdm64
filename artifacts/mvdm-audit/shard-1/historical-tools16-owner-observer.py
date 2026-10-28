"""Read-only pending historical 16-bit build-tool census and current boundaries."""
import csv,hashlib,json,re,subprocess,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
N=['tools16/'+x for x in ('buildmsg.exe','c1.err','cl.err','cl.msg','cl16.exe','h2inc.exe','masm.exe','q23.exe','rc16.exe','rcqq.err','rcqq.exe','stripdd.exe')]
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def norm(p):return str(Path(p).resolve()).replace('\\','/').casefold()
def inspect():
 with (A/'shard-1/file-audit.csv').open(encoding='utf-8-sig',newline='') as f:states={x['relative_path']:x for x in csv.DictReader(f)}
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:rows={x['relative_path']:x for x in csv.DictReader(f)}
 graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')];matches={n:[] for n in N};wanted={norm(base/n):n for base in (ROOT,ORIG,ALT) for n in N};graph_hits={n:[] for n in N}
 for b in graphs:
  text=(b/'build.ninja').read_text().replace('$:',':').lower()
  for n in N:
   for base in (ROOT,ORIG,ALT):
    if (base/n).resolve().as_posix().lower() in text:graph_hits[n].append(str(b))
  deps=subprocess.check_output(['ninja','-C',str(b),'-t','deps'],text=True,encoding='utf-8',errors='replace');owner=None;valid=False
  for line in deps.splitlines():
   if not line.startswith(' '):owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
   elif valid:
    raw=line.strip();p=Path(raw);key=norm(p if p.is_absolute() else b/p)
    if key in wanted:matches[wanted[key]].append(dict(build=str(b),object=owner,raw=raw))
 with zipfile.ZipFile('assets/winnt.zip') as z:archive_names={Path(x.replace('\\','/')).name.lower() for x in z.namelist()}
 entries=[]
 for n in N:
  assert states[n]['audit_status']=='待核验';r=rows[n];assert not (ROOT/n).exists() and r['current_identity']=='not-currently-mirrored'
  refs=[]
  for base,col in ((ORIG,'opennt_sha256'),(ALT,'opennt45_sha256')):
   if r[col]:assert (base/n).exists() and sha(base/n)==r[col];refs.append(dict(path=str(base/n),sha256=sha(base/n)))
  assert refs and not matches[n] and not graph_hits[n]
  entries.append(dict(path=n,status=states[n]['audit_status'],catalog_ids=r['wow_source_ids'],originals=refs,valid_host_dependencies=matches[n],exact_graph_path_hits=graph_hits[n],installed_system32_basename_present=(Path('O:/winnt/system32')/Path(n).name).exists(),archive_basename_present=Path(n).name.lower() in archive_names))
 anchors=['dos/v86/cmd/command/makefile','inc/makefile','dpmi/makefile','wow16/commdlg/makefile','dos/v86/dev/himem/makefile'];text=[(ROOT/x).read_text(encoding='latin1').lower() for x in anchors]
 assert all(q in t for q,t in zip(['buildmsg.exe','h2inc','masm','rc16','stripdd'],text));assert 'cl16' in text[3]
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,recipe_anchors=[dict(path=x,sha256=sha(ROOT/x)) for x in anchors],host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],limitations='Original bytes and bounded name/recipe/graph evidence only; no execution, proven tool-file-to-recipe identity, full PATH search, guest rebuild or all-profile tool requirement claim.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
