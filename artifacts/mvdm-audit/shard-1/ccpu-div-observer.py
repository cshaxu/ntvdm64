"""Read-only binding of CCPU unsigned DIV decode, leaf and fault path."""
import csv,hashlib,json,runpy,struct
from pathlib import Path
A=Path('artifacts/mvdm-audit');B=Path('build/.ninja/msvc-x86-release/worker')
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py'));rel='softpc.new/base/ccpu386/div.c';src=Path('src/mvdm')/rel
 originals=[Path('O:/repos.external/OpenNT/base/mvdm')/rel,Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/rel]
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:row=next(r for r in csv.DictReader(f) if r['relative_path']==rel)
 assert all(sha(p)==sha(src) for p in originals) and sha(src)==row['current_sha256']==row['opennt_sha256']==row['opennt45_sha256']
 graph=(B/'build.ninja').read_text().replace('$:',':');edge=next(x for x in graph.splitlines() if x.startswith('build obj/ccpu/div.obj:'));archive=next(x for x in graph.splitlines() if x.startswith('build original-ccpu386.lib:'));assert src.resolve().as_posix().lower() in edge.lower() and 'obj/ccpu/div.obj' in archive.split()
 image=h['Image'](Path('assets/release/ntvdm.exe'));cached=h['Image'](B/'ntvdm.exe');assert image.code==cached.code and all(image.data[a:b]==cached.data[a:b] for a,b,_ in image.code)
 symbols=h['symbols']((B/'ntvdm.exe.map').read_text());obj=h['Coff'](B/'obj/ccpu/div.obj');functions=[];fault=[]
 def resolve(sym,obj,targets):
  raw,rels=obj.function(sym['name']);found=[]
  for offset,target,kind in rels:
   if target not in targets:continue
   dst=next(x for x in symbols if x['name']==target and x['function']);word=struct.unpack('<I',h['data_at'](image,sym['va']+offset,4))[0];add=struct.unpack_from('<i',raw,offset)[0]
   assert word==((dst['va']+add-(sym['va']+offset+4)) if kind==20 else (dst['va']+add))&0xffffffff;found.append(dict(caller=sym['name'],target=target,owner=dst['owner'],kind=kind))
  return found
 for name in ('_DIV8','_DIV16','_DIV32'):
  sym=next(x for x in symbols if x['name']==name and x['owner']=='original-ccpu386:div.obj');functions.append(h['bind'](image,obj,sym));e=resolve(sym,obj,{'_Int0','_divu64'});assert sum(x['target']=='_Int0' for x in e)==2;fault+=e
 cobj=h['Coff'](B/'obj/ccpu/c_main.obj');decoder=next(x for x in symbols if x['name']=='_ccpu' and x['owner']=='original-ccpu386:c_main.obj');decoder_binding=h['bind'](image,cobj,decoder);decoder_edges=resolve(decoder,cobj,{'_DIV8','_DIV16','_DIV32'});assert {x['target'] for x in decoder_edges}=={'_DIV8','_DIV16','_DIV32'}
 xobj=h['Coff'](B/'obj/ccpu/c_xcptn.obj');int0=next(x for x in symbols if x['name']=='_Int0' and x['owner']=='original-ccpu386:c_xcptn.obj');exception=h['bind'](image,xobj,int0);retire=resolve(int0,xobj,{'_c_cpu_continue'});assert retire
 paths=[src,*originals,B/'build.ninja',B/'obj/ccpu/div.obj',B/'obj/ccpu/c_main.obj',B/'obj/ccpu/c_xcptn.obj',B/'ntvdm.exe.map',Path('assets/release/ntvdm.exe'),src.parent/'c_main.c',src.parent/'c_xcptn.c']
 return dict(result='pass',source=rel,source_sha256=sha(src),catalog_ids=row['wow_source_ids'],build_edge=edge,functions=functions,decoder_binding=decoder_binding,decoder_edges=decoder_edges,fault_edges=fault,exception_binding=exception,continuation_edges=retire,inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],limitations='Static decode/exception binding only; no arithmetic oracle, fault guest execution, hook return or exception stack assertions.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
