import hashlib,json,struct
from pathlib import Path
root=Path(__file__).resolve().parents[3]
items=[root/'build/.ninja/msvc-x86-release/wow32/wow32.dll',Path('O:/winnt/system32/wow32.dll'),root/'assets/release/WOW32.DLL']
def h(b): return hashlib.sha256(b).hexdigest().upper()
def pe(path):
 b=path.read_bytes(); off=struct.unpack_from('<I',b,0x3c)[0]; assert b[off:off+4]==b'PE\0\0'; n=struct.unpack_from('<H',b,off+6)[0]; opt=struct.unpack_from('<H',b,off+20)[0]; base=off+24+opt; secs=[]
 for i in range(n):
  pos=base+40*i; name=b[pos:pos+8].split(b'\0')[0].decode('ascii'); vs,va,rs,ro=struct.unpack_from('<IIII',b,pos+8)
  secs.append({'name':name,'virtual_address':va,'virtual_size':vs,'raw_size':rs,'raw_sha256':h(b[ro:ro+rs])})
 return {'path':str(path),'sha256':h(b),'sections':secs}
print(json.dumps([pe(x) for x in items],indent=2))
