# Test-only, length-preserving relocation of copied Win95 registry hives.
import pathlib,struct,sys
src,dst,old,new=sys.argv[1:]
a,b=old.encode('ascii'),new.encode('ascii')
assert len(a)==len(b)
data=bytearray(pathlib.Path(src).read_bytes())
u16=lambda o:struct.unpack_from('<H',data,o)[0]
u32=lambda o:struct.unpack_from('<I',data,o)[0]
assert data[:4]==b'CREG'
at=u32(8); changes=0
for block in range(u16(16)):
 assert data[at:at+4]==b'RGDB'
 size=u32(at+4);entry=at+32
 while entry<at+size:
  n=u32(entry)
  if not n:break
  assert n>=8 and entry+n<=at+size
  if u16(entry+4)!=65535:
   assert n>=20
   value=entry+20+u16(entry+12)
   for k in range(u16(entry+14)):
    length=u16(value+10);start=value+12+u16(value+8);end=start+length
    assert end<=entry+n
    if u32(value)==1:
     text=bytes(data[start:end]);pos=0
     while True:
      pos=text.lower().find(a.lower(),pos)
      if pos<0:break
      tail=pos+len(a)
      if tail==len(text) or text[tail] in b'\x00\\/ \t"\',;)':
       data[start+pos:start+tail]=b;changes+=1
      pos=tail
    value=end
  entry+=n
 if u32(at+12)&4:
  struct.pack_into('<I',data,at+28,0)
  checksum=sum(struct.unpack_from('<I',data,i)[0] for i in range(at,at+size,4))
  struct.pack_into('<I',data,at+28,(-checksum)&0xffffffff)
 at+=size
assert at==len(data)
assert not pathlib.Path(dst).exists()
pathlib.Path(dst).write_bytes(data)
print('length-preserved string substitutions',changes)
