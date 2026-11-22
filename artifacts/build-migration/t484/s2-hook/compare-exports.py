from pathlib import Path
import struct,json

def exports(path):
 b=Path(path).read_bytes();pe=struct.unpack_from('<I',b,60)[0];opt=pe+24
 assert b[pe:pe+4]==b'PE\0\0'
 magic=struct.unpack_from('<H',b,opt)[0];base=opt+(112 if magic==0x20b else 96)
 sections=opt+struct.unpack_from('<H',b,pe+20)[0]
 def off(rva):
  for n in range(struct.unpack_from('<H',b,pe+6)[0]):
   size,va,raw,ptr=struct.unpack_from('<IIII',b,sections+n*40+8)
   if va<=rva<va+max(size,raw):return ptr+rva-va
  raise ValueError('unmapped RVA')
 def string(rva):
  p=off(rva);return b[p:b.index(0,p)].decode('ascii')
 rva,size=struct.unpack_from('<II',b,base);p=off(rva)
 ordinal,count,names,functions,namelist,ordinals=struct.unpack_from('<IIIIII',b,p+16)
 rows=[]
 for n in range(names):
  name=string(struct.unpack_from('<I',b,off(namelist)+4*n)[0]);idx=struct.unpack_from('<H',b,off(ordinals)+2*n)[0]
  rows.append((name,ordinal+idx))
 return {'machine':struct.unpack_from('<H',b,pe+4)[0],'exports':rows,'function_count':count}
old=exports('build/t484-s2-tib/stage-input/nthook64.dll');new=exports('build/.ninja/msvc-x64-release/hook/nthook64.dll')
assert old==new,(old,new)
Path('build/t484-s2-hook/export-comparison.json').write_text(json.dumps({'equal':True,'contract':new},indent=2)+'\n',encoding='utf-8')
print('PASS export/ordinal/machine contract identical',new)
