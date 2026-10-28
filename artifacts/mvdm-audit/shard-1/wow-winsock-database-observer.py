"""Read-only release binding of original WOW Winsock database and async lifecycle."""
import csv,hashlib,json,runpy,struct
from pathlib import Path
A=Path('artifacts/mvdm-audit');B=Path('build/.ninja/msvc-x86-release/wow32')
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py'));rel='wow32/wsdata.c';src=Path('src/mvdm')/rel;orig=[Path('O:/repos.external/OpenNT/base/mvdm')/rel,Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/rel]
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:r=next(x for x in csv.DictReader(f) if x['relative_path']==rel)
 assert sha(src)==r['current_sha256'] and sha(orig[0])==r['opennt_sha256'] and sha(orig[1])==r['opennt45_sha256'] and sha(src)!=sha(orig[0])
 graph=(B/'build.ninja').read_text().replace('$:',':');edge=next(x for x in graph.splitlines() if x.startswith('build obj/wsdata.obj:'));link=next(x for x in graph.splitlines() if x.startswith('build wow32.dll | wow32.lib:'));assert src.resolve().as_posix().lower() in edge.lower() and 'obj/wsdata.obj' in link.split()
 image=h['Image'](Path('assets/release/WOW32.DLL'));cached=h['Image'](B/'wow32.dll');assert image.code==cached.code and all(image.data[a:b]==cached.data[a:b] for a,b,_ in image.code)
 syms=h['symbols']((B/'wow32.dll.map').read_text());obj=h['Coff'](B/'obj/wsdata.obj');functions=[h['bind'](image,obj,x) for x in syms if x['owner']=='wsdata.obj' and x['function']];assert len(functions)==23
 edges=[]
 for caller,stem,wanted in [('_WWS32TaskCleanup@0','wsext',{'_WWS32CancelAsyncRequestsForTask@4'}),('_WWS32PostAsyncGetHost@16','wsdata',{'_WWS32FindAndRemoveAsyncContext@4'})]:
  sym=next(x for x in syms if x['name']==caller and x['owner']==stem+'.obj');co=h['Coff'](B/'obj'/(stem+'.obj'));h['bind'](image,co,sym);body,relocs=co.function(caller)
  for off,target,kind in relocs:
   if target not in wanted:continue
   assert kind==20;dst=next(x for x in syms if x['name']==target and x['function']);word=struct.unpack('<I',h['data_at'](image,sym['va']+off,4))[0];add=struct.unpack_from('<i',body,off)[0];assert word==(dst['va']+add-(sym['va']+off+4))&0xffffffff;edges.append(dict(caller=caller,target=target,owner=dst['owner'],offset=off))
 assert len(edges)==2
 paths=[src,*orig,B/'build.ninja',B/'wow32.dll.map',B/'obj/wsdata.obj',B/'obj/wsext.obj',Path('assets/release/WOW32.DLL'),src.parent/'wsext.c',src.parent/'winsockp.h']
 return dict(result='pass',source=rel,source_sha256=sha(src),original_sha256=sha(orig[0]),catalog_ids=r['wow_source_ids'],build_edge=edge,functions=functions,edges=edges,inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],limitations='Static image/source evidence, not live provider cancellation, completion race, handle reuse or guest execution proof.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
