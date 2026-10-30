import re,pathlib,collections,bisect,json
root=pathlib.Path('build/t473-s11/sol-stack-sample-r1');lines=(root/'probe.txt').read_text().splitlines();mods=[]
for line in lines:
 m=re.match(r'SOL sample: module=(\S+) base=([0-9A-Fa-f]+) size=(\d+)',line)
 if m:mods.append((int(m[2],16),int(m[3]),m[1].lower()))
def symfile(p,b):
 a=[]
 for l in pathlib.Path(p).read_text(errors='replace').splitlines():
  m=re.match(r'\s*0001:[0-9A-Fa-f]+\s+(\S+)\s+([0-9A-Fa-f]{8})\s+f\s+',l)
  if m:a.append((int(m[2],16)-b,m[1]))
 return sorted(a)
maps={'ntvdm.exe':symfile('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map',0x400000),'wow32.dll':symfile('build/.ninja/msvc-x86-release/wow32/wow32.dll.map',0x10000000)}
def desc(pc):
 for base,size,name in mods:
  if base<=pc<base+size:
   if name not in maps:return name
   syms=maps[name];i=bisect.bisect_right(syms,(pc-base,chr(0x10ffff)))-1
   return name+':'+(syms[i][1] if i>=0 and pc-base-syms[i][0]<0x4000 else '?')
 return '?'
counts=collections.Counter(); details=[]
for l in lines:
 m=re.match(r'SOL sample: phase=(\w+) pc=([0-9a-f]+) esp=([0-9a-f]+) ebp=([0-9a-f]+) stack=(.*)',l,re.I)
 if not m:continue
 pc,esp,ebp=(int(m[i],16) for i in [2,3,4]); words=[int(x,16) for x in m[5].split(',') if x]; off=(ebp-esp)//4
 if ebp>=esp and (ebp-esp)%4==0 and 0<=off+1<len(words):
  a,b=desc(pc),desc(words[off+1]); counts[(m[1],a,b)]+=1;details.append((m[1],a,b))
(root/'stack-analysis.json').write_text(json.dumps({'counts':[[*k,v] for k,v in counts.most_common()],'possible_frames':len(details),'limitations':'EBP can be omitted or point to non-frame data; stack words are candidate return addresses only, not proven call chains.'},indent=2))
print('possible frames',len(details))
for k,v in counts.most_common(15):print(*k,v)
