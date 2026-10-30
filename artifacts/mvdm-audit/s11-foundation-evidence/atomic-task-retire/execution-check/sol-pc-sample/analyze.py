import re,collections,bisect,json,pathlib,sys,hashlib
root=pathlib.Path(sys.argv[1]); lines=(root/'probe.txt').read_text().splitlines(); modules=[]; samples=[]
for l in lines:
 m=re.match(r'SOL sample: module=(\S+) base=([0-9A-Fa-f]+) size=(\d+)',l)
 if m:modules.append((int(m[2],16),int(m[3]),m[1]))
 m=re.match(r'SOL sample: phase=(\w+) pc=([0-9A-Fa-f]+)',l)
 if m:samples.append((m[1],int(m[2],16)))
paths={'ntvdm.exe':(pathlib.Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map'),0x400000),'wow32.dll':(pathlib.Path('build/.ninja/msvc-x86-release/wow32/wow32.dll.map'),0x10000000)}
def symbols(path,base):
 out=[]
 for line in path.read_text(errors='replace').splitlines():
  m=re.match(r'\s*0001:[0-9A-Fa-f]+\s+(\S+)\s+([0-9A-Fa-f]{8})\s+f\s+',line)
  if m:out.append((int(m[2],16)-base,m[1]))
 return sorted(out)
maps={k:symbols(p,b) for k,(p,b) in paths.items()};counts=collections.Counter();details=[]
for phase,pc in samples:
 match=next(((base,size,name) for base,size,name in modules if base<=pc<base+size),None)
 if not match:key=(phase,'<outside-loaded-module>','')
 else:
  base,size,name=match;rva=pc-base;ss=maps.get(name.lower());sym=''
  if ss:
   i=bisect.bisect_right(ss,(rva,chr(0x10ffff)))-1
   if i>=0 and rva-ss[i][0]<0x4000:sym=ss[i][1]+'+'+hex(rva-ss[i][0])
  key=(phase,name,sym.split('+')[0]);details.append({'phase':phase,'module':name,'rva':hex(rva),'nearest_symbol':sym})
 counts[key]+=1
out={'samples':len(samples),'counts':[{'phase':k[0],'module':k[1],'nearest_symbol':k[2],'samples':v} for k,v in counts.most_common()],'details':details,'map_sha256':{k:hashlib.sha256(p.read_bytes()).hexdigest().upper() for k,(p,b) in paths.items()},'limitation':'One game window thread, nominal 25ms sampling, observer overhead, nearest map symbol only; not process CPU percentages or stack attribution.'}
(root/'analysis.json').write_text(json.dumps(out,indent=2)+'\n')
print('samples',len(samples))
for k,v in counts.most_common(14):print(*k,v)
