from pathlib import Path
import struct,json,hashlib
b=Path('src/mvdm/wow16/mmsystem/MMSYSTEM.DLL').read_bytes(); u16=lambda x:struct.unpack_from('<H',b,x)[0];u32=lambda x:struct.unpack_from('<I',b,x)[0]; ne=u32(0x3c);p=ne+u16(ne+4);ordinal=1; found=None
while b[p]:
 count=b[p]; seg=b[p+1]; p+=2
 for _ in range(count):
  if seg==0: entry=None
  elif seg==255:
   entry=dict(flags=b[p],segment=b[p+3],offset=u16(p+4));p+=6
  else:
   entry=dict(flags=b[p],segment=seg,offset=u16(p+1));p+=3
  if ordinal==900: found=entry
  ordinal+=1
if found:
 s=ne+u16(ne+0x22)+(found['segment']-1)*8; alignment=u16(ne+0x32);file_offset=(u16(s)<<alignment)+found['offset'];found.update(file_offset=file_offset,bytes_hex=b[file_offset:file_offset+48].hex())
print(json.dumps(dict(sha256=hashlib.sha256(b).hexdigest(),ordinal=900,entry=found,limitation='NE entry-table and segment translation only; code disassembly and invocation not verified.'),indent=2))
