"""Bind selected WOW parameter map lifecycle to release WOW32 image."""
import csv,hashlib,json,runpy,struct
from pathlib import Path
A=Path('artifacts/mvdm-audit');B=Path('build/.ninja/msvc-x86-release/wow32')
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py'));rel='wow32/wparam.c';src=Path('src/mvdm')/rel;originals=[Path('O:/repos.external/OpenNT/base/mvdm')/rel,Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/rel]
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:row=next(r for r in csv.DictReader(f) if r['relative_path']==rel)
 assert sha(src)==row['current_sha256'] and sha(originals[0])==row['opennt_sha256'] and sha(originals[1])==row['opennt45_sha256'] and sha(src)!=sha(originals[0])
 graph=(B/'build.ninja').read_text().replace('$:',':');edge=next(x for x in graph.splitlines() if x.startswith('build obj/wparam.obj:'));link=next(x for x in graph.splitlines() if x.startswith('build wow32.dll | wow32.lib:'));assert src.resolve().as_posix().lower() in edge.lower() and 'obj/wparam.obj' in link.split()
 image=h['Image'](Path('assets/release/WOW32.DLL'));cached=h['Image'](B/'wow32.dll');assert image.code==cached.code and all(image.data[a:b]==cached.data[a:b] for a,b,_ in image.code)
 symbols=h['symbols']((B/'wow32.dll.map').read_text());obj=h['Coff'](B/'obj/wparam.obj');functions=[h['bind'](image,obj,x) for x in symbols if x['owner']=='wparam.obj' and x['function']];assert len(functions)==9
 callers=[('_WowOriginalFreeTask@4','wkman',{'_FreeParamMap@4'}),('_W32Init@0','wow32',{'_InitParamMap@0'}),('_WU32SetWindowText@4','wuwind',{'_AddParamMap@8','_DeleteParamMap@12'})];edges=[]
 for name,stem,wanted in callers:
  sym=next(x for x in symbols if x['name']==name and x['owner']==stem+'.obj');co=h['Coff'](B/'obj'/(stem+'.obj'));h['bind'](image,co,sym);raw,rels=co.function(name)
  for offset,target,kind in rels:
   if target not in wanted:continue
   assert kind==20;dst=next(x for x in symbols if x['name']==target and x['owner']=='wparam.obj');word=struct.unpack('<I',h['data_at'](image,sym['va']+offset,4))[0];add=struct.unpack_from('<i',raw,offset)[0];assert word==(dst['va']+add-(sym['va']+offset+4))&0xffffffff;edges.append(dict(caller=name,target=target,offset=offset))
 assert len(edges)==4
 paths=[src,*originals,B/'build.ninja',B/'wow32.dll.map',Path('assets/release/WOW32.DLL'),*[B/'obj'/(n+'.obj') for n in ('wparam','wkman','wow32','wuwind')],*[src.parent/(n+'.c') for n in ('wkman','wow32','wuwind')]]
 return dict(result='pass',source=rel,source_sha256=sha(src),original_sha256=sha(originals[0]),catalog_ids=row['wow_source_ids'],build_edge=edge,functions=functions,edges=edges,inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],limitations='Static image binding and selected lifecycle calls, not live nested callback, cross-task alias, failure recovery, or task retirement proof.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
