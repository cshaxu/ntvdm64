"""Read-only WINOLDAP guest identity and WOW module-dispatch binding."""
import csv,hashlib,json,runpy,struct,subprocess,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm');N=['wow16/winoldap/WINOLDAP.MOD','wow16/winoldap/WINOLDAP.RES','wow16/winoldap/winoldap.c']
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
 d=ROOT/'wow16/winoldap';refs=['makefile','winoldap.rc','winoldap.rcv'];
 for n in refs:assert sha(d/n)==sha(ORIG/'wow16/winoldap'/n)==sha(ALT/'wow16/winoldap'/n)
 make=(d/'makefile').read_text(encoding='latin1').lower();assert 'winoldap.mod: winoldap.obj winoldap.lrf winoldap.def winoldap.res' in make and 'rc16 -t winoldap.res winoldap.mod' in make
 h=runpy.run_path(str(A/'host-tail-image-observer.py'));b=graphs[1];image=h['Image'](Path('assets/release/WOW32.DLL'));cached=h['Image'](b/'wow32.dll');assert image.code==cached.code and all(image.data[x:y]==cached.data[x:y] for x,y,_ in image.code);syms=h['symbols']((b/'wow32.dll.map').read_text())
 sym=next(x for x in syms if x['name']=='_WK32LoadModule32@4' and x['owner']=='wkman.obj');binding=h['bind'](image,h['Coff'](b/'obj/wkman.obj'),sym);table=next(x for x in syms if x['name']=='_aw32WOW' and x['owner']=='wowtbl.obj');obj=h['Coff'](b/'obj/wowtbl.obj');_,start,section=next(x for x in obj.symbols.values() if x[0]=='_aw32WOW' and x[2]>0);raw,relocs=obj.sections[section-1];off=start+225*4;assert [r for r in relocs if r[0]==off]==[(off,sym['name'],6)];actual=struct.unpack('<I',h['data_at'](image,table['va']+225*4,4))[0];assert actual==(sym['va']+struct.unpack_from('<I',raw,off)[0])&0xffffffff
 with zipfile.ZipFile('assets/winnt.zip') as z:names=[n for n in z.namelist() if n.lower().endswith('/winoldap.mod')]
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,recipe_inputs=[dict(path=str(d/n),sha256=sha(d/n)) for n in refs],host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],wow32_image_sha256=sha('assets/release/WOW32.DLL'),wow_module_binding=binding,kernel_ordinal=225,zip_members=names,installed_exact_present=Path('O:/winnt/system32/WINOLDAP.MOD').exists(),limitations='Original source/recipe and one WOW image table binding only; no installed guest execution, package search outside exact inspected locations or guest concurrency/teardown proof.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
