"""Read-only cross-language WOW constants and normalized build dependencies."""
import csv,hashlib,json,re,runpy,subprocess
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
N=['inc/WOWCMPAT.INC','inc/WOWUSR.INC','inc/wowcmpat.h','inc/wowusr.h']
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
 def pair(prefix,asm,c):
  pairs=re.findall(r'^('+prefix+r'\w*)\s+EQU\s+([0-9a-fA-F]+[hH]|[0-9]+)',(ROOT/asm).read_text(encoding='latin1'),re.M)
  a={n:int(v[:-1],16) if v.lower().endswith('h') else int(v) for n,v in pairs};b={n:int(v,0) for n,v in re.findall(r'^#define\s+('+prefix+r'\w*)\s+(0[xX][0-9a-fA-F]+|[0-9]+)',(ROOT/c).read_text(encoding='latin1'),re.M)};assert all(a[n]==(int(v[:-1],16) if v.lower().endswith('h') else int(v)) for n,v in pairs) and a==b;return a
 flags=pair('WOWCF','inc/WOWCMPAT.INC','inc/wowcmpat.h');ids=pair('FUN_','inc/WOWUSR.INC','inc/wowusr.h');assert len(flags)==52 and len(ids)==455
 wt=ROOT/'wow32/wutbl2.h';table=re.findall(r'W32FUN\(.*?,\s*"([^"]*)"',wt.read_text(encoding='latin1'));assert len(table)==539
 labels=[dict(name=k,ordinal=v,table_label=table[v] if v<len(table) else None) for k,v in ids.items() if v>=len(table) or table[v].upper()!=k[4:].upper()]
 assert matches['inc/wowcmpat.h'] and matches['inc/wowusr.h']
 h=runpy.run_path(str(A/'host-tail-image-observer.py'));b=graphs[1];image=h['Image'](Path('assets/release/WOW32.DLL'));cached=h['Image'](b/'wow32.dll');assert image.code==cached.code and all(image.data[x:y]==cached.data[x:y] for x,y,_ in image.code)
 paths=[wt,ROOT/'wow32/precomp.h',ROOT/'wow32/wow32.h',ROOT/'wow16/kernel31/3ginterf.asm',ROOT/'wow16/user/user.asm',Path('assets/release/WOW32.DLL'),b/'wow32.dll.map']
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,flags_shared=len(flags),user_ids_shared=len(ids),user_table_entries=len(table),user_label_mismatches=labels,context_inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],limitations='Numeric literal comparison, normalized compiler dependencies, source anchors and image section identity only; no packing, guest image lineage, all table slots semantic equivalence, or live API behavior proof.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
