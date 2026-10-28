"""Bind original spool and shared 16-bit handle carrier to the release image."""
import csv,hashlib,json,runpy,struct
from pathlib import Path
A=Path('artifacts/mvdm-audit');B=Path('build/.ninja/msvc-x86-release/wow32')
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py'));rel='wow32/wspool.c';src=Path('src/mvdm')/rel
 originals=[Path('O:/repos.external/OpenNT/base/mvdm')/rel,Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/rel]
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:row=next(r for r in csv.DictReader(f) if r['relative_path']==rel)
 assert all(sha(p)==sha(src) for p in originals) and sha(src)==row['current_sha256']==row['opennt_sha256']==row['opennt45_sha256']
 graph=(B/'build.ninja').read_text().replace('$:',':');edge=next(x for x in graph.splitlines() if x.startswith('build obj/wspool.obj:'));link=next(x for x in graph.splitlines() if x.startswith('build wow32.dll | wow32.lib:'));assert src.resolve().as_posix().lower() in edge.lower() and 'obj/wspool.obj' in link.split()
 image=h['Image'](Path('assets/release/WOW32.DLL'));cached=h['Image'](B/'wow32.dll');assert image.code==cached.code and all(image.data[a:b]==cached.data[a:b] for a,b,_ in image.code)
 symbols=h['symbols']((B/'wow32.dll.map').read_text());obj=h['Coff'](B/'obj/wspool.obj');functions=[];edges=[]
 for sym in [x for x in symbols if x['owner']=='wspool.obj' and x['function']]:
  functions.append(h['bind'](image,obj,sym));body,relocs=obj.function(sym['name'])
  for offset,target,kind in relocs:
   if kind!=20 or target not in ('_Prn32@4','_FreePrn@4','_GetPrn16@4','_GetDriverName@8','_GetDefaultDevMode32@4'):continue
   dst=next(x for x in symbols if x['name']==target and x['owner']=='wspool.obj');word=struct.unpack('<I',h['data_at'](image,sym['va']+offset,4))[0];add=struct.unpack_from('<i',body,offset)[0]
   assert word==(dst['va']+add-(sym['va']+offset+4))&0xffffffff;edges.append(dict(caller=sym['name'],target=target))
 assert len(functions)==11 and '_LocalFree16@4' in [r[1] for r in obj.function('_WG32CloseJob@4')[1]]
 table=Path('src/mvdm/wow32/wgtbl2.h').read_text(encoding='latin1');alias=Path('src/mvdm/wow32/walias.h').read_text(encoding='latin1');window=Path('src/mvdm/wow32/wuwind.c').read_text(encoding='latin1')
 assert all(n in table for n in ('WG32OpenJob','WG32CloseJob','WG32DeleteJob','WG32WriteSpool','WG32StartSpoolPage','WG32EndSpoolPage')) and 'GetPrn16((HAND32)(hdwp32))' in alias and 'GETHDWP16(BeginDeferWindowPos' in window and 'FREEHDWP16(parg16->f1)' in window
 paths=[src,*originals,B/'build.ninja',B/'obj/wspool.obj',B/'wow32.dll.map',Path('assets/release/WOW32.DLL'),Path('src/mvdm/wow32/wgtbl2.h'),Path('src/mvdm/wow32/walias.h'),Path('src/mvdm/wow32/wuwind.c')]
 return dict(result='pass',source=rel,source_sha256=sha(src),catalog_ids=row['wow_source_ids'],build_edge=edge,functions=functions,internal_edges=edges,inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],limitations='Static image/source binding only; no actual spool provider, long profile name, allocation failure, task loss, or deferred positioning execution.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
