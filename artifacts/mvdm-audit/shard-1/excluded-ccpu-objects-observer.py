"""Read-only audit of four excluded CCPU profiles against release build."""
import csv,hashlib,json,re,runpy,subprocess
from pathlib import Path
A=Path('artifacts/mvdm-audit');B=Path('build/.ninja/msvc-x86-release/worker');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
N=['softpc.new/base/ccpu386/'+x for x in ('c_getset.c','ccpupig.c','ccpupig.h','nop.c')]
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def norm(p):return str(Path(p).resolve()).replace('\\','/').casefold()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py'))
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:rows={x['relative_path']:x for x in csv.DictReader(f)}
 with (A/'combined-initial-status.csv').open(encoding='utf-8-sig',newline='') as f:initial={x['relative_path']:x for x in csv.DictReader(f)}
 entries=[]
 for n in N:
  p=ROOT/n;r=rows[n];assert initial[n]['status']=='excluded' and sha(p)==r['current_sha256']==r['opennt_sha256']==r['opennt45_sha256']==sha(ORIG/n)==sha(ALT/n)
  entries.append(dict(path=n,sha256=sha(p),initial_class=initial[n]['status'],shard=initial[n]['shard']))
 graph=(B/'build.ninja').read_text().replace('$:',':');flags=next(x for x in graph.splitlines() if x.startswith('ccpu_flags ='));assert '/DPROD' in flags and '/DCPU_40_STYLE' in flags and not re.search(r'(?:/D|-D)PIG(?:=\S*)?(?=\s|$)',flags)
 arcmembers=subprocess.check_output(['ar','t',str(B/'original-ccpu386.lib')],text=True,encoding='utf-8',errors='replace').splitlines();symbols=h['symbols']((B/'ntvdm.exe.map').read_text());image=h['Image'](Path('assets/release/ntvdm.exe'));cached=h['Image'](B/'ntvdm.exe');assert image.code==cached.code and all(image.data[a:b]==cached.data[a:b] for a,b,_ in image.code)
 objects=[]
 for stem in ('c_getset','ccpupig','nop'):
  path=B/('obj/ccpu/'+stem+'.obj');edge=next(x for x in graph.splitlines() if x.startswith('build obj/ccpu/'+stem+'.obj:'));assert 'cc_ccpu ' in edge and ('obj/ccpu/'+stem+'.obj') in arcmembers
  obj=h['Coff'](path);defs=[dict(name=x[0],section=x[2]) for x in obj.symbols.values() if x[2]>0 and not x[0].startswith('.')];owners=[x for x in symbols if x['owner'].lower().endswith(':'+stem+'.obj')];assert not owners
  if stem in ('c_getset','ccpupig'):assert not defs
  else:assert any(x['name']=='_NOP' for x in defs) and obj.function('_NOP')[0]==b'\xc2\x00\x00' and not any(x['name']=='_NOP' for x in symbols)
  objects.append(dict(stem=stem,edge=edge,object_sha256=sha(path),defined_nonsection_symbols=defs,archive_member=True,map_owner_symbols=owners))
 dec=next(x for x in symbols if x['name']=='_ccpu' and x['owner']=='original-ccpu386:c_main.obj');co=h['Coff'](B/'obj/ccpu/c_main.obj');binding=h['bind'](image,co,dec);assert not any(x[1]=='_NOP' for x in co.function('_ccpu')[1]);source=(ROOT/'softpc.new/base/ccpu386/c_main.c').read_text(encoding='latin1');assert re.search(r'case\s+0x90:\s*/\*\s*T0 NOP\s*\*/\s*break\s*;',source)
 deps=subprocess.check_output(['ninja','-C',str(B),'-t','deps'],text=True,encoding='utf-8',errors='replace');needle=norm(ROOT/N[2]);hits=[];owner=None;valid=False
 for line in deps.splitlines():
  if not line.startswith(' '):owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
  elif valid:
   raw=line.strip();p=Path(raw)
   if norm(p if p.is_absolute() else B/p)==needle:hits.append(dict(object=owner,raw=raw))
 assert hits
 prior=json.loads((A/'ccpu-header-selection-observations.json').read_text(encoding='utf-8'));hdr=next(x for x in prior['headers'] if x['relative_path']==N[2]);assert not hdr['emitted_declaration_units']
 paths=[A/'combined-initial-status.csv',A/'frozen-file-reconciliation.csv',A/'ccpu-header-selection-observations.json',B/'build.ninja',B/'ntvdm.exe.map',B/'original-ccpu386.lib',B/'obj/ccpu/c_main.obj',Path('assets/release/ntvdm.exe')]
 return dict(result='pass',entries=entries,ccpu_flag_prod=True,ccpu_flag_pig=False,objects=objects,header_valid_dependencies=hits,header_prior_preprocessed=hdr,decoder_binding=binding,inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],limitations='Profile-scoped source, object/archive membership, released executable code equality and map labels; no assertion that absent map owner proves archive member was never extracted, nor alternate PIG/debug builds or guest behavior completeness.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
