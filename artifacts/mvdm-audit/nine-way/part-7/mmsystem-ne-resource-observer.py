from pathlib import Path
import json,struct,hashlib
b=Path('src/mvdm/wow16/mmsystem/MMSYSTEM.DLL').read_bytes(); u=lambda p:struct.unpack_from('<H',b,p)[0]; n=struct.unpack_from('<I',b,60)[0];r=n+u(n+36);align=u(r);p=r+2; rows=[]
def name(x):return str(x&0x7fff) if x&0x8000 else b[r+x+1:r+x+1+b[r+x]].decode('latin1')
while u(p):
 typ=u(p); count=u(p+2);p+=8
 for k in range(count):
  offset=u(p)<<align;size=u(p+2)<<align; nm=u(p+6);p+=12
  rows.append(dict(type=name(typ),name=name(nm),offset=offset,size=size,sha256=hashlib.sha256(b[offset:offset+size]).hexdigest()))
print(json.dumps(dict(sha256=hashlib.sha256(b).hexdigest(),resources=rows,limitations='NE resource membership only, not source identity or runtime use.'),indent=2))
