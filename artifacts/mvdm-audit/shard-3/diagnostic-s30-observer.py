"""Bind the changed CPU diagnostic bodies to the selected WOW32 image.
Relocation-masked bytes are static selection evidence, not execution coverage.
"""
import hashlib,json,runpy,sys
from pathlib import Path
sys.dont_write_bytecode=True
root=Path('build/t478-s30'); package=Path('build/t478-s29/package-final')
imagepath=package/'system32/ntvdm.exe'
h=runpy.run_path('artifacts/mvdm-audit/host-tail-image-observer.py')
image=imagepath.read_bytes();bindings=[]
for relative,name in (
 ('ccpu/c_xfer.obj','_validate_far_dest'),
 ('adapter-softpc/mvdm_softpc_termination.obj','_mvdm_softpc_report_wow_far_call_attempt'),
 ('adapter-softpc/mvdm_softpc_termination.obj','_mvdm_softpc_report_dpmi_fault_dispatch'),
 ('adapter-softpc/mvdm_softpc_termination.obj','_mvdm_softpc_report_wow_source_descriptor_store'),
 ('keymouse/keybd_io.obj','_keyboard_io'),
 ('system/illegalp.obj','_illegal_op_int')):
 objpath=Path('build/.ninja/msvc-x86-release/worker/obj')/relative
 obj=h['Coff'](objpath)
 code,relocs=obj.function(name);mask={i for off,_,_ in relocs for i in range(off,off+4)}
 run=[];best=[]
 for i in range(len(code)):
  run=run+[i] if i not in mask else []
  if len(run)>len(best):best=run
 anchor=bytes(code[best[0]:best[-1]+1]);pos=0;matches=[]
 while True:
  hit=image.find(anchor,pos)
  if hit<0:break
  start=hit-best[0]
  if start>=0 and start+len(code)<=len(image) and all(i in mask or image[start+i]==v for i,v in enumerate(code)):matches.append(start)
  pos=hit+1
 assert len(matches)==1,(name,matches)
 bindings.append(dict(object_path=str(objpath),object_sha256=hashlib.sha256(objpath.read_bytes()).hexdigest().upper(),function=name,bytes=len(code),file_offset=matches[0],relocations=len(relocs)))
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest().upper()
rows=json.loads((package/'product-system32-manifest.json').read_text(encoding='utf-8-sig'))
for row in rows:assert sha(package/row['RelativePath'].replace('\\','/'))==row['Sha256']
result=dict(result='pass',package_id=rows[0]['PackageId'],bindings=bindings,image_sha256=sha(imagepath),limitations='Relocation-masked full bodies prove static selection, not arbitrary nested guest execution.')
print(json.dumps(result,indent=2))
