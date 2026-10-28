"""Read-only COFF section to PE masked binding for WOW file cache."""
import pathlib,struct,hashlib,json,re
obj=pathlib.Path('build/.ninja/msvc-x86-release/wow32/obj/wkfileio.obj')
b=obj.read_bytes(); symoff,num=struct.unpack_from('<II',b,8);st=symoff+num*18
names=[]
for i in range(num):
 raw=b[symoff+i*18:symoff+i*18+8]
 names.append((b[st+struct.unpack_from('<I',raw,4)[0]:].split(b'\0',1)[0] if raw[:4]==b'\0'*4 else raw.split(b'\0')[0]).decode('latin1','replace'))
files=[pathlib.Path('build/.ninja/msvc-x86-release/wow32/wow32.dll'),pathlib.Path('O:/winnt/system32/wow32.dll')]
rows=[]
for sec,name in [(15,'FlushMapFileCaches'),(17,'FreeMapFileCache'),(37,'W32MapViewOfFile'),(39,'WK32FileClose'),(57,'WK32FileRead')]:
 o=20+(sec-1)*40;size,raw,rel=struct.unpack_from('<III',b,o+16);count=struct.unpack_from('<H',b,o+32)[0]
 rs=[struct.unpack_from('<IIH',b,rel+i*10) for i in range(count)]; data=b[raw:raw+size];mask=set(k for p,s,t in rs for k in range(p,p+4));results=[]
 for f in files:
  z=f.read_bytes(); found=None
  for chunk in range(max(0,size-24)):
   if any(k in mask for k in range(chunk,chunk+24)):continue
   needle=data[chunk:chunk+24];pos=z.find(needle)
   if pos<chunk or z.find(needle,pos+1)>=0:continue
   base=pos-chunk
   if base+size<=len(z) and all(k in mask or data[k]==z[base+k] for k in range(size)):
    found=base;break
  if found is None: raise RuntimeError(f'{name} not fully matched in {f}')
  targets=[dict(offset=p,symbol=names[s],type=t,encoded=z[found+p:found+p+4].hex()) for p,s,t in rs if any(w in names[s] for w in ('CloseHandle','MapView','CreateFileMapping','FindMapFileCache'))]
  results.append(dict(path=str(f),sha256=hashlib.sha256(z).hexdigest(),file_offset=found,relocations=targets))
 rows.append(dict(section=sec,name=name,size=size,relocation_count=count,images=results))
print(json.dumps(dict(object=str(obj),object_sha256=hashlib.sha256(b).hexdigest(),sections=rows),indent=2))
