"""Bind the changed Winsock ownership bodies to the selected WOW32 image.
Relocation-masked bytes are static selection evidence, not execution coverage.
"""
import hashlib,json,runpy,sys
from pathlib import Path
sys.dont_write_bytecode=True
root=Path('build/t478-s29'); package=root/'package-final'
objpath=Path('build/.ninja/msvc-x86-release/wow32/obj/wsdata.obj')
imagepath=package/'system32/WOW32.DLL'
h=runpy.run_path('artifacts/mvdm-audit/host-tail-image-observer.py')
obj=h['Coff'](objpath);image=imagepath.read_bytes();bindings=[]
for name in ('_WWS32CancelAsyncRequestsForTask@4','_WWS32FindAndRemoveAsyncContext@4'):
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
 bindings.append(dict(function=name,bytes=len(code),file_offset=matches[0],relocations=len(relocs)))
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest().upper()
rows=json.loads((package/'product-system32-manifest.json').read_text(encoding='utf-8-sig'))
for row in rows:assert sha(package/row['RelativePath'].replace('\\','/'))==row['Sha256']
result=dict(result='pass',package_id=rows[0]['PackageId'],bindings=bindings,inputs=[dict(path=str(p),sha256=sha(p)) for p in (objpath,imagepath,Path('src/mvdm-host/wow32/wsdata.c'),Path('src/mvdm-host/wow32/winsockp.h'))])
print(json.dumps(result,indent=2))
