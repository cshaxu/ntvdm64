"""Bind original Win.ini embedding bridge and registry providers to WOW32 image."""
import csv,hashlib,json,runpy,struct
from pathlib import Path
A=Path('artifacts/mvdm-audit');B=Path('build/.ninja/msvc-x86-release/wow32')
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py'));rel='wow32/mapembed.c';src=Path('src/mvdm')/rel
 originals=[Path('O:/repos.external/OpenNT/base/mvdm')/rel,Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/rel]
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:row=next(r for r in csv.DictReader(f) if r['relative_path']==rel)
 assert all(sha(p)==sha(src) for p in originals) and sha(src)==row['current_sha256']==row['opennt_sha256']==row['opennt45_sha256']
 graph=(B/'build.ninja').read_text().replace('$:',':');edge=next(x for x in graph.splitlines() if x.startswith('build obj/mapembed.obj:'));link=next(x for x in graph.splitlines() if x.startswith('build wow32.dll | wow32.lib:'));assert src.resolve().as_posix().lower() in edge.lower() and 'obj/mapembed.obj' in link.split()
 image=h['Image'](Path('assets/release/WOW32.DLL'));cached=h['Image'](B/'wow32.dll');assert image.code==cached.code and all(image.data[a:b]==cached.data[a:b] for a,b,_ in image.code)
 symbols=h['symbols']((B/'wow32.dll.map').read_text());obj=h['Coff'](B/'obj/mapembed.obj');functions=[];edges=[]
 for sym in [x for x in symbols if x['owner']=='mapembed.obj' and x['function']]:
  functions.append(h['bind'](image,obj,sym));body,relocs=obj.function(sym['name'])
  for offset,target,kind in relocs:
   if not target.startswith('_wow_registry_'):continue
   assert kind==20;dst=next(x for x in symbols if x['name']==target and x['function']);word=struct.unpack('<I',h['data_at'](image,sym['va']+offset,4))[0];add=struct.unpack_from('<i',body,offset)[0];assert word==(dst['va']+add-(sym['va']+offset+4))&0xffffffff;edges.append(dict(caller=sym['name'],target=target,owner=dst['owner']))
 assert len(functions)==6 and {x['target'].split('@')[0] for x in edges}=={'_wow_registry_create_key','_wow_registry_set_value','_wow_registry_close_key','_wow_registry_enum_key','_wow_registry_open_key','_wow_registry_query_value'}
 wk=Path('src/mvdm/wow32/wkernel.c');table=Path('src/mvdm/wow32/wktbl2.h');paths=[src,*originals,B/'build.ninja',B/'obj/mapembed.obj',B/'wow32.dll.map',Path('assets/release/WOW32.DLL'),wk,table]
 return dict(result='pass',source=rel,source_sha256=sha(src),catalog_ids=row['wow_source_ids'],build_edge=edge,functions=functions,registry_edges=edges,inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],limitations='Static image and registry relocation only; no live guest OLE, registry persistence, native profile mapping, failure injection or cross-task timing proof.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
