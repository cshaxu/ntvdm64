"""Verify original delta and selected optimized DLC initializer bytes."""
import hashlib
import json
from pathlib import Path
import re
import runpy
import sys

sys.dont_write_bytecode=True
root=Path('build/t478-s27')
source=Path('src/mvdm-host/vdmredir/vrdlc5c.c')
objpath=Path('build/.ninja/msvc-x86-release/worker/obj/redir/vrdlc5c.obj')
package=root/'package-final2'
imagepath=package/'system32/VDMREDIR.dll'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
def body(text):
    m=list(re.finditer(r'BOOLEAN\s+InitializeDlcWorkerThread\s*\(\s*VOID\s*\)',text))[-1]
    start=text.index('{',m.end());i=start+1;depth=1
    while depth:
        depth+=(text[i]=='{')-(text[i]=='}');i+=1
    return text[start:i]
current=body(source.read_text(encoding='utf-8'));originals=[]
for base in ('O:/repos.external/OpenNT/base/mvdm','O:/repos.external/OpenNT-4.5/nt/private/mvdm'):
    p=Path(base)/'vdmredir/vrdlc5c.c';before=body(p.read_text(encoding='cp1252'))
    i=before.index('DlcWorkerCompletionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);')
    assert current==before[:i]+before[i:].replace('if (DlcWorkerEvent == NULL)','if (DlcWorkerCompletionEvent == NULL)',1)
    originals.append(dict(path=str(p),sha256=sha(p)))
h=runpy.run_path('artifacts/mvdm-audit/host-tail-image-observer.py')
obj=h['Coff'](objpath);image=imagepath.read_bytes()
code,relocs=obj.function('_InitializeAdapterSupport')
mask={i for off,_,_ in relocs for i in range(off,off+4)}
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
assert len(matches)==1,matches
rows=json.loads((package/'product-system32-manifest.json').read_text(encoding='utf-8-sig'))
assert len(rows)==10
for row in rows:assert sha(package/row['RelativePath'].replace('\\','/'))==row['Sha256']
result=dict(result='pass',originals=originals,package_id=rows[0]['PackageId'],
    binding=dict(function='_InitializeAdapterSupport',bytes=len(code),file_offset=matches[0],relocations=len(relocs)),
    inputs=[dict(path=str(p),sha256=sha(p)) for p in (source,objpath,imagepath)],
    fixtures=[dict(case=name,output=(root/name/'run.txt').read_text(encoding='utf-8'),
                   hashes=json.loads((root/name/'hashes.json').read_text(encoding='utf-8-sig'))) for name in ('baseline','current')],
    limitations='Exact initializer with real events/controlled thread; masked image match is static, not DLC provider or adapter execution. Original partial-initialization limits retained.')
for name,key in [('verify-final-Full/timings.json','full'),('guest-dlc-replay/result.json','guest')]:
    if (root/name).exists():result[key]=json.loads((root/name).read_text(encoding='utf-8-sig'))
if (root/'publication.txt').exists():result['publication']=(root/'publication.txt').read_text(encoding='utf-8-sig')
print(json.dumps(result,indent=2))
