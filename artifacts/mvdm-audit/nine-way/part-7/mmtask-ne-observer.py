from pathlib import Path
import hashlib,json,struct
p=Path('src/mvdm/wow16/mmsystem/MMSYSTEM.DLL'); b=p.read_bytes(); u16=lambda o:struct.unpack_from('<H',b,o)[0]; u32=lambda o:struct.unpack_from('<I',b,o)[0]
base=u32(0x3c); assert b[base:base+2]==b'NE'; resources=base+u16(base+0x24); align=u16(resources); t=resources+2; found=[]
while True:
 typ=u16(t); t+=2
 if not typ: break
 count=u16(t); t+=6
 for i in range(count):
  off=u16(t)<<align; size=u16(t+2)<<align; name=u16(t+6); t+=12
  if typ==0x8006 and name==0x807e:
   pos=off; strings=[]
   for j in range(16):
    n=b[pos];pos+=1;strings.append(b[pos:pos+n].decode('latin1'));pos+=n
   found.append(dict(type=hex(typ),name=hex(name),offset=off,size=size,id2000=strings[0],all_strings=strings))
def names(pos):
 out=[]
 while b[pos]:
  n=b[pos]; pos+=1; s=b[pos:pos+n].decode('latin1');pos+=n; ordinal=u16(pos);pos+=2
  if 'TASK' in s.upper():out.append([s,ordinal])
 return out
resident=base+u16(base+0x26); nonresident=u32(base+0x2c)
print(json.dumps(dict(sha256=hashlib.sha256(b).hexdigest(),ne_offset=base,resource_alignment=align,strings=found,task_exports=names(resident)+names(nonresident),limitations='NE parsed static strings and name/ordinal only; no execution, source binding or search-path observation.'),indent=2))
