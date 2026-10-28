import hashlib,json,struct,zipfile,pathlib
r=pathlib.Path('.'); src=r/'src/mvdm/wow16/shell/SHELL.DLL'; installed=pathlib.Path('O:/winnt/system32/SHELL.DLL'); z=r/'assets/winnt.zip'; raw=src.read_bytes(); sh=lambda b:hashlib.sha256(b).hexdigest().upper()
with zipfile.ZipFile(z) as f: arc=f.read('WINNT/SYSTEM32/SHELL.DLL')
assert raw==arc==installed.read_bytes()
w=lambda o:struct.unpack_from('<H',raw,o)[0]; ne=struct.unpack_from('<I',raw,60)[0]; assert raw[ne:ne+2]==b'NE'
ent=ne+w(ne+4);seg=ne+w(ne+0x22);shift=w(ne+0x32);pos=ent;ordinal=1;found={}
while raw[pos]:
 count,typ=raw[pos],raw[pos+1];pos+=2
 if typ==0:ordinal+=count;continue
 for _ in range(count):
  if typ==255:flags=raw[pos];sn=raw[pos+3];off=w(pos+4);pos+=6
  else:flags=raw[pos];sn=typ;off=w(pos+1);pos+=3
  if ordinal in (1,2,20,21,22):
   se=seg+(sn-1)*8;base=w(se)<<shift;length=w(se+2) or 65536; fixbase=base+length;nfix=w(fixbase); fixes=[raw[fixbase+2+8*i:fixbase+10+8*i] for i in range(nfix)]
   code=raw[base+off:base+off+13]; assert code[:1]==b'\x6a' and code[2:5]==b'\x68\x00\x00' and code[5]==0x68 and code[6]==ordinal and code[7]==0 and code[8]==0x9a
   # Relocation on the call-id immediate exists; resolving its imported absolute value is loader-owned and not modeled.
   reloc=[x.hex() for x in fixes if int.from_bytes(x[2:4],'little')==off+6];assert len(reloc)==1
   found[ordinal]={'segment':sn,'segment_offset':off,'file_offset':base+off,'entry_bytes':code.hex(),'args_bytes':code[1],'call_id_fixup':reloc[0]}
  ordinal+=1
assert set(found)=={1,2,20,21,22}
out={'result':'pass','image_sha256':sh(raw),'source':str(src),'installed':str(installed),'archive':str(z),'archive_entry':'WINNT/SYSTEM32/SHELL.DLL','entries':found,'scope':'NE entry bytes and fixup-site identity only; imports, loader relocation, source build provenance and live dispatch unproved'}
print(json.dumps(out,indent=2))
