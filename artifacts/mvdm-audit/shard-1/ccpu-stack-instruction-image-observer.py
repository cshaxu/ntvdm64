"""Read-only PUSHA/LEAVE decoder, released object, and stack binding."""
import csv,hashlib,json,runpy,struct
from pathlib import Path
A=Path('artifacts/mvdm-audit');B=Path('build/.ninja/msvc-x86-release/worker')
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py'));rows=list(csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='')));sources=[]
 graph=(B/'build.ninja').read_text().replace('$:',':');archive=next(x for x in graph.splitlines() if x.startswith('build original-ccpu386.lib:'))
 for stem in ('pusha','leave'):
  rel='softpc.new/base/ccpu386/'+stem+'.c';src=Path('src/mvdm')/rel;roots=[Path('O:/repos.external/OpenNT/base/mvdm')/rel,Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/rel];row=next(r for r in rows if r['relative_path']==rel)
  assert all(sha(p)==sha(src) for p in roots) and sha(src)==row['current_sha256']==row['opennt_sha256']==row['opennt45_sha256']
  edge=next(x for x in graph.splitlines() if x.startswith('build obj/ccpu/'+stem+'.obj:'));assert src.resolve().as_posix().lower() in edge.lower() and ('obj/ccpu/'+stem+'.obj') in archive.split();sources.append(dict(path=rel,sha256=sha(src),catalog_ids=row['wow_source_ids'],edge=edge))
 image=h['Image'](Path('assets/release/ntvdm.exe'));cached=h['Image'](B/'ntvdm.exe');assert image.code==cached.code and all(image.data[a:b]==cached.data[a:b] for a,b,_ in image.code)
 symbols=h['symbols']((B/'ntvdm.exe.map').read_text());functions=[];edges=[]
 callers=[('_PUSHA','pusha'),('_LEAVE16','leave'),('_LEAVE32','leave'),('_ccpu','c_main')];wanted={'_PUSHA','_LEAVE16','_LEAVE32','_validate_stack_exists','_validate_stack_space','_set_current_SP','_spop','_spush'}
 for name,stem in callers:
  sym=next(x for x in symbols if x['name']==name and x['owner']=='original-ccpu386:'+stem+'.obj');obj=h['Coff'](B/('obj/ccpu/'+stem+'.obj'));functions.append(h['bind'](image,obj,sym));raw,rels=obj.function(name)
  for offset,target,kind in rels:
   if target not in wanted:continue
   assert kind==20;dst=next(x for x in symbols if x['name']==target and x['function']);word=struct.unpack('<I',h['data_at'](image,sym['va']+offset,4))[0];add=struct.unpack_from('<i',raw,offset)[0];assert word==(dst['va']+add-(sym['va']+offset+4))&0xffffffff;edges.append(dict(caller=name,target=target,owner=dst['owner'],offset=offset))
 assert len(edges)==18 and sum(x['caller']=='_PUSHA' and x['target']=='_spush' for x in edges)==8
 paths=[B/'build.ninja',B/'ntvdm.exe.map',Path('assets/release/ntvdm.exe'),*[B/('obj/ccpu/'+n+'.obj') for n in ('pusha','leave','c_main')],*[Path('src/mvdm/softpc.new/base/ccpu386')/(n+'.c') for n in ('pusha','leave','c_main','c_stack')]]
 return dict(result='pass',sources=sources,functions=functions,resolved_edges=edges,inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],limitations='Static image bindings only; no guest decode, real memory, flag, exception or segment-wrapping execution proof.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
