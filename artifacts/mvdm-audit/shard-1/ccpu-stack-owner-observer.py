"""Read-only binding of original CCPU stack support and interrupt consumers."""
import csv,hashlib,json,runpy,struct
from pathlib import Path
A=Path('artifacts/mvdm-audit');B=Path('build/.ninja/msvc-x86-release/worker')
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py'));rel='softpc.new/base/ccpu386/c_stack.c';src=Path('src/mvdm')/rel
 originals=[Path('O:/repos.external/OpenNT/base/mvdm')/rel,Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/rel]
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:row=next(r for r in csv.DictReader(f) if r['relative_path']==rel)
 assert all(sha(p)==sha(src) for p in originals) and sha(src)==row['current_sha256']==row['opennt_sha256']==row['opennt45_sha256']
 graph=(B/'build.ninja').read_text().replace('$:',':');edge=next(x for x in graph.splitlines() if x.startswith('build obj/ccpu/c_stack.obj:'));archive=next(x for x in graph.splitlines() if x.startswith('build original-ccpu386.lib:'));assert src.resolve().as_posix().lower() in edge.lower() and 'obj/ccpu/c_stack.obj' in archive.split()
 image=h['Image'](Path('assets/release/ntvdm.exe'));cached=h['Image'](B/'ntvdm.exe');assert image.code==cached.code and all(image.data[a:b]==cached.data[a:b] for a,b,_ in image.code)
 symbols=h['symbols']((B/'ntvdm.exe.map').read_text());obj=h['Coff'](B/'obj/ccpu/c_stack.obj');functions=[h['bind'](image,obj,x) for x in symbols if x['owner']=='original-ccpu386:c_stack.obj' and x['function']];assert len(functions)==13
 caller=next(x for x in symbols if x['name']=='_do_intrupt' and x['owner']=='original-ccpu386:c_intr.obj');co=h['Coff'](B/'obj/ccpu/c_intr.obj');binding=h['bind'](image,co,caller);raw,rels=co.function(caller['name']);edges=[];wanted={'_spush','_spush16','_validate_stack_space','_validate_new_stack_space'}
 for off,name,kind in rels:
  if name not in wanted:continue
  assert kind==20;dst=next(x for x in symbols if x['name']==name and x['owner']=='original-ccpu386:c_stack.obj');word=struct.unpack('<I',h['data_at'](image,caller['va']+off,4))[0];add=struct.unpack_from('<i',raw,off)[0];assert word==(dst['va']+add-(caller['va']+off+4))&0xffffffff;edges.append(dict(caller=caller['name'],target=name,offset=off))
 assert {e['target'] for e in edges}==wanted
 paths=[src,*originals,B/'build.ninja',B/'ntvdm.exe.map',B/'obj/ccpu/c_stack.obj',B/'obj/ccpu/c_intr.obj',Path('assets/release/ntvdm.exe'),src.parent/'c_intr.c']
 return dict(result='pass',source=rel,source_sha256=sha(src),catalog_ids=row['wow_source_ids'],build_edge=edge,functions=functions,interrupt_caller=binding,interrupt_edges=edges,inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],limitations='Static binding only; no actual stack write, exception, size/wrap, privilege switch or IRET behavior test.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
