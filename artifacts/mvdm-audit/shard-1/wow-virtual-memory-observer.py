"""Read-only image and provenance observation for original WOW virtual-memory thunks."""
import csv,hashlib,json,runpy,struct
from pathlib import Path
A=Path('artifacts/mvdm-audit'); B=Path('build/.ninja/msvc-x86-release/wow32'); W=Path('build/.ninja/msvc-x86-release/worker')
def sha(p): return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py')); rel='wow32/wkmem.c'; src=Path('src/mvdm')/rel
 originals=[Path('O:/repos.external/OpenNT/base/mvdm')/rel,Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/rel]
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f: row=next(r for r in csv.DictReader(f) if r['relative_path']==rel)
 assert all(sha(p)==sha(src) for p in originals) and sha(src)==row['current_sha256']==row['opennt_sha256']==row['opennt45_sha256']
 graph=(B/'build.ninja').read_text().replace('$:',':'); edge=next(x for x in graph.splitlines() if x.startswith('build obj/wkmem.obj:')); link=next(x for x in graph.splitlines() if x.startswith('build wow32.dll | wow32.lib:')); assert src.resolve().as_posix().lower() in edge.lower() and 'obj/wkmem.obj' in link.split()
 image=h['Image'](Path('assets/release/WOW32.DLL')); cached=h['Image'](B/'wow32.dll'); assert image.code==cached.code and all(image.data[a:b]==cached.data[a:b] for a,b,_ in image.code)
 symbols=h['symbols']((B/'wow32.dll.map').read_text()); obj=h['Coff'](B/'obj/wkmem.obj'); result=[]
 for name in ('WK32VirtualAlloc','WK32VirtualFree','WK32GlobalMemoryStatus'):
  sym=next(x for x in symbols if x['name']=='_'+name+'@4' and x['owner']=='wkmem.obj' and x['function']); bound=h['bind'](image,obj,sym); body,rels=obj.function(sym['name']); edges=[]
  for offset,target,kind in rels:
   dst=next(x for x in symbols if x['name']==target)
   word=struct.unpack('<I',h['data_at'](image,sym['va']+offset,4))[0]; add=struct.unpack_from('<i',body,offset)[0]
   expected=(dst['va']+add-(sym['va']+offset+4))&0xffffffff if kind==20 else (dst['va']+add)&0xffffffff
   assert word==expected,(name,target,hex(word),hex(expected));edges.append(dict(symbol=target,owner=dst['owner'],relocation_type=kind))
  result.append(dict(binding=bound,edges=edges))
 edges={e['symbol']:e['owner'] for x in result for e in x['edges']}; assert edges['_VdmAllocateVirtualMemory']=='ntvdm:ntvdm.exe' and edges['_VdmFreeVirtualMemory']=='ntvdm:ntvdm.exe'
 worker_symbols=h['symbols']((W/'ntvdm.exe.map').read_text()); providers=[]
 for name in ('_VdmAllocateVirtualMemory','_VdmFreeVirtualMemory'):
  x=next(x for x in worker_symbols if x['name']==name and x['function']); assert x['owner']=='original-softpc-host-roots:nt_mem.obj'; providers.append(x)
 paths=[src,*originals,B/'build.ninja',B/'obj/wkmem.obj',B/'wow32.dll.map',Path('assets/release/WOW32.DLL'),W/'ntvdm.exe.map',Path('src/mvdm/softpc.new/host/src/nt_mem.c'),Path('src/mvdm/wow32/wktbl2.h')]
 return dict(result='pass',source=rel,sha256=sha(src),catalog_ids=row['wow_source_ids'],build_edge=edge,thunks=result,worker_map_providers=providers,inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],limitations='Static linked-code proof, not allocation execution, cross-image identity, stress, host failure injection, or guest lifetime proof.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
