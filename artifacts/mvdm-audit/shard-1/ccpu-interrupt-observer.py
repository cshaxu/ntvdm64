"""Read-only original CCPU interrupt entry and release call-site binding."""
import csv,hashlib,json,runpy,struct
from pathlib import Path
A=Path('artifacts/mvdm-audit');B=Path('build/.ninja/msvc-x86-release/worker')
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py'));rel='softpc.new/base/ccpu386/c_intr.c';src=Path('src/mvdm')/rel
 originals=[Path('O:/repos.external/OpenNT/base/mvdm')/rel,Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/rel]
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:row=next(r for r in csv.DictReader(f) if r['relative_path']==rel)
 assert all(sha(p)==sha(src) for p in originals) and sha(src)==row['current_sha256']==row['opennt_sha256']==row['opennt45_sha256']
 graph=(B/'build.ninja').read_text().replace('$:',':');edge=next(x for x in graph.splitlines() if x.startswith('build obj/ccpu/c_intr.obj:'));archive=next(x for x in graph.splitlines() if x.startswith('build original-ccpu386.lib:'));assert src.resolve().as_posix().lower() in edge.lower() and 'obj/ccpu/c_intr.obj' in archive.split()
 image=h['Image'](Path('assets/release/ntvdm.exe'));cached=h['Image'](B/'ntvdm.exe');assert image.code==cached.code and all(image.data[a:b]==cached.data[a:b] for a,b,_ in image.code)
 symbols=h['symbols']((B/'ntvdm.exe.map').read_text());obj=h['Coff'](B/'obj/ccpu/c_intr.obj');target=next(x for x in symbols if x['name']=='_do_intrupt' and x['owner']=='original-ccpu386:c_intr.obj');body=h['bind'](image,obj,target);callers=[]
 for name,stem in [('_INTx','intx'),('_INTO','into'),('_Int0','c_xcptn'),('_ccpu','c_main')]:
  sym=next(x for x in symbols if x['name']==name and x['owner']=='original-ccpu386:'+stem+'.obj');co=h['Coff'](B/('obj/ccpu/'+stem+'.obj'));bound=h['bind'](image,co,sym);raw,rels=co.function(name)
  for offset,_,kind in [r for r in rels if r[1]=='_do_intrupt']:
   assert kind==20;word=struct.unpack('<I',h['data_at'](image,sym['va']+offset,4))[0];add=struct.unpack_from('<i',raw,offset)[0];assert word==(target['va']+add-(sym['va']+offset+4))&0xffffffff;callers.append(dict(caller=name,offset=offset,binding=bound))
 assert {x['caller'] for x in callers}=={'_INTx','_INTO','_Int0','_ccpu'}
 paths=[src,*originals,B/'build.ninja',B/'ntvdm.exe.map',Path('assets/release/ntvdm.exe'),*[B/('obj/ccpu/'+n+'.obj') for n in ('c_intr','intx','into','c_xcptn','c_main')],*[src.parent/(n+'.c') for n in ('intx','into','c_xcptn','c_main')]]
 return dict(result='pass',source=rel,source_sha256=sha(src),catalog_ids=row['wow_source_ids'],build_edge=edge,interrupt_binding=body,selected_callers=callers,inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],limitations='Static binary/source bindings only; no hardware IRQ, fault delivery, nested gates, privilege-stack or IRET execution proof.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
