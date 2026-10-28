"""Read-only cross-language DOS, DPMI, XMS and SoftPC service definitions."""
import csv,hashlib,json,re,subprocess
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
S=['DOSSVC','DPMI','XMSSVC','SOFTPC'];N=['inc/'+x+'.INC' for x in S];C=['inc/'+x.lower()+'.h' for x in S]
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def norm(p):return str(Path(p).resolve()).replace('\\','/').casefold()
def inspect():
 with (A/'shard-1/file-audit.csv').open(encoding='utf-8-sig',newline='') as f:states={x['relative_path']:x for x in csv.DictReader(f)}
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:rows={x['relative_path']:x for x in csv.DictReader(f)}
 graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')];matches={n:[] for n in N+C};wanted={norm(ROOT/n):n for n in N+C}
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
 pairs=[]
 for s,asm,c in zip(S,N,C):
  a={k:int(v[:-1],16) if v.lower().endswith('h') else int(v) for k,v in re.findall(r'^(\w+)\s+EQU\s+([0-9a-fA-F]+[hH]|[0-9]+)\s*(?:;.*)?$',(ROOT/asm).read_text(encoding='latin1'),re.I|re.M)}
  b={k:int(v,0) for k,v in re.findall(r'^#define\s+(\w+)\s+(0[xX][0-9a-fA-F]+|[0-9]+)(?=\s|$)',(ROOT/c).read_text(encoding='latin1'),re.M)}
  common=sorted(a.keys()&b.keys());dif=[dict(name=k,asm=a[k],c=b[k]) for k in common if a[k]!=b[k]];assert not dif
  pairs.append(dict(name=s,asm_literals=len(a),c_literals=len(b),shared_literals=len(common),only_asm=sorted(a.keys()-b.keys()),only_c=sorted(b.keys()-a.keys()),differences=dif,c_sha256=sha(ROOT/c),c_valid_dependencies=matches[c]))
 assert all(re.search(r'^'+n+r'\s+EQU\s+'+v+r'H', (ROOT/'inc/BOP.INC').read_text(encoding='latin1'),re.I|re.M) for n,v in [('BOP_DOS','50'),('BOP_XMS','52'),('BOP_DPMI','53')])
 assert [(x['name'],x['shared_literals']) for x in pairs]==[('DOSSVC',81),('DPMI',38),('XMSSVC',13),('SOFTPC',66)]
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,pairs=pairs,host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],limitations='Literal constants and normalized exact host dependencies only; no guest built, selector dispatch table validation, service behavior or structure layout compatibility.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
