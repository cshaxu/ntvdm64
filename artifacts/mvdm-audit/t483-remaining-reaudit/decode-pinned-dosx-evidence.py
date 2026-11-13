from pathlib import Path
import struct,hashlib,json
p=Path('src/mvdm-guest/dpmi/486/dosx.exe');b=p.read_bytes();h=struct.unpack_from('<14H',b);start=h[4]*16;packed=h[11]*16;stub=b[start+packed:];size=struct.unpack_from('<H',stub,12)[0]*16;src=bytearray(b[start:start+packed]);out=bytearray(size);s=len(src)-1;d=size-1
while src[s]==255:s-=1
runs=[]
while True:
 cmd=src[s];s-=1;n=src[s-1]|(src[s]<<8);s-=2
 assert (cmd&254) in (176,178),(hex(s),hex(cmd))
 assert 0<=d-n+1 and n>0
 if (cmd&254)==176:
  value=src[s];s-=1;out[d-n+1:d+1]=bytes([value])*n
 else:
  assert s-n+1>=0;out[d-n+1:d+1]=src[s-n+1:s+1];s-=n
 runs.append([cmd,n]);d-=n
 if cmd&1:break
assert d==-1,(d,s)
root=Path('build/t483-s5-dpmiint-review');(root/'dosx-decompressed.bin').write_bytes(out)
for name,address,length in [('iret-bops',0xa8d0+0x1770,24),('nsetseg',0xa8d0+0x35e5,110)]: (root/(name+'.bin')).write_bytes(out[address:address+length])
print(json.dumps(dict(input_sha256=hashlib.sha256(b).hexdigest(),decoded_size=size,runs=len(runs),remaining_input=s+1,iret_bops=out[0xa8d0+0x1770:0xa8d0+0x1784].hex(),ntdebug_trace_public='DEBUGTRACE' in Path('src/mvdm-guest/dpmi/486/dosx.map').read_text().upper())))
