from pathlib import Path
from collections import defaultdict,Counter
from functools import lru_cache
import csv,re,struct,subprocess,posixpath as pp,json,os
D=Path(__file__).resolve().parent
R=Path.cwd()
layers={k:v for v,s in enumerate(['lib-nxvm lib-win32','compat','non-mvdm','mvdm-host','common','frontend-base worker-base','ntcon-exe nthook-dll ntmon-exe ntsrv-exe ntvdm-exe ntvwm-exe run16-exe vdmredir-dll wow32-dll']) for k in s.split()}
def owner(p):
 a=p.split('/')
 return a[1] if len(a)>1 and a[0]=='src' and a[1] in layers else ''
def bad(a,b):return a and b and a!=b and layers[a]<=layers[b]
def cmd(*a):return subprocess.check_output(a).decode('utf-8','replace')
files=cmd('git','ls-files','src').splitlines()+cmd('git','ls-files','--others','--exclude-standard','src').splitlines()
files=[p for p in files if Path(p).is_file()]
canon={p.lower():p for p in files}
byname=defaultdict(set)
for p in files:byname[pp.basename(p).lower()].add(p)
records=defaultdict(set)
def add(kind,a,b,p,n,t,detail,ctx=''):
 if bad(a,b):records[kind].add((a,b,p,str(n),t,detail,ctx))
def output():
 for kind,rows in records.items():
  with (D/(kind+'.tsv')).open('w',encoding='utf-8',newline='') as f:
   w=csv.writer(f,delimiter='\t');w.writerow(['from','to','source','line_or_section','target','evidence','product']);w.writerows(sorted(rows))
def path(s,base):
 s=s.replace('\\','/').replace('$:',':')
 if not re.match(r'^\w:/',s):s=pp.join(base,s)
 s=pp.normpath(s).lower()
 prefix=R.as_posix().lower()+'/'
 return canon.get(s[len(prefix):],'') if s.startswith(prefix) else ''
@lru_cache(None)
def directives(p):
 data=Path(p).read_bytes().decode('latin1')
 data=re.sub(r'/\*.*?\*/',lambda m:'\n'*m[0].count('\n'),data,flags=re.S)
 return [(data.count('\n',0,m.start())+1,m[1].replace('\\','/')) for m in re.finditer(r'^[ \t]*#[ \t]*include\s*[<"]([^>"\n]+)[>"]',data,re.M)]
for p in files:
 if not owner(p) or Path(p).suffix.lower() not in ('.c','.cpp','.cxx','.h','.hpp','.inl','.inc','.rc','.idl','.asm'):continue
 for n,h in directives(p):
  for ref in [pp.normpath(pp.join(pp.dirname(p),h)),pp.normpath('src/'+h)]:
   t=canon.get(ref.lower())
   if t:
    add('literal-includes',owner(p),owner(t),p,n,t,h);break
base=Path('build/.ninja')
jobs={'msvc-x86-release/worker':['ntvdm.exe','VCDEX.DLL','VDMREDIR.dll','nthook32.dll'],'msvc-x86-release/wow32':['wow32.dll'],**{'msvc-x64-release/'+s:[t] for s,t in [('frontend','ntcon.exe'),('service','ntsrv.exe'),('monitor','ntmon.exe'),('launcher','run16.exe'),('worker','ntvwm.exe'),('hook','nthook64.dll')]}}
jobs['msvc-x64-release/worker-reference-x86']=['ntvdm.exe','VCDEX.DLL','nthook32.dll']

dlls={'ntvdm.exe':'ntvdm-exe','vcdex.dll':'mvdm-host','vdmredir.dll':'vdmredir-dll','nthook32.dll':'nthook-dll','wow32.dll':'wow32-dll','ntcon.exe':'ntcon-exe','ntsrv.exe':'ntsrv-exe','ntmon.exe':'ntmon-exe','run16.exe':'run16-exe','ntvwm.exe':'ntvwm-exe','nthook64.dll':'nthook-dll'}
expanded_stats={}; unresolved_symbols=defaultdict(set); stats={}; common_coverage=defaultdict(set); gaps=defaultdict(set)
@lru_cache(None)
def coff(fn):
 d=Path(fn).read_bytes()
 if len(d)<20:return []
 mach,ns,_,st,nsy,opt,_=struct.unpack_from('<HHIIIHH',d)
 if mach not in (332,34404) or not nsy:return []
 strings=st+18*nsy
 def name(off):
  if d[off:off+4]==bytes(4):
   k=strings+struct.unpack_from('<I',d,off+4)[0];return d[k:d.find(b'\0',k)].decode('latin1')
  return d[off:off+8].split(b'\0')[0].decode('latin1')
 sy={};defs=defaultdict(list);k=0
 while k<nsy:
  off=st+18*k;v,sec,typ,cl,aux=struct.unpack_from('<IhHBB',d,off+8)
  if cl in (2,105,3):
   nm=name(off);sy[k]=(nm,sec,cl)
   if sec>0 and (cl==2 or cl==3 and not nm.startswith('.')):defs[sec].append(nm)
  k+=1+aux
 links=[]
 for i in range(ns):
  off=20+opt+40*i;rr=struct.unpack_from('<I',d,off+24)[0];nr=struct.unpack_from('<H',d,off+32)[0]
  if nr==65535:continue
  for j in range(nr):
   ix=struct.unpack_from('<I',d,rr+10*j+4)[0];v=sy.get(ix)
   if v and v[1]==0 and v[2] in (2,105):links.append((i+1,tuple(defs[i+1]),v[0]))
 return set(links)

@lru_cache(None)
def coff_sections(fn):
 d=Path(fn).read_bytes()
 if len(d)<20:return {},{},{}
 mach,ns,_,st,nsy,opt,_=struct.unpack_from('<HHIIIHH',d)
 if mach not in (332,34404) or not nsy:return {},{},{}
 strings=st+18*nsy
 def name(off):
  if d[off:off+4]==bytes(4):
   k=strings+struct.unpack_from('<I',d,off+4)[0]
   return d[k:d.find(b'\0',k)].decode('latin1')
  return d[off:off+8].split(b'\0')[0].decode('latin1')
 sy={};defs=defaultdict(set);k=0
 while k<nsy:
  off=st+18*k;value,sec,typ,storage,aux=struct.unpack_from('<IhHBB',d,off+8)
  nm=name(off);sy[k]=(nm,sec,storage)
  if sec>0 and storage in (2,3,105) and not nm.startswith('.'):
   defs[sec].add(nm)
  k+=1+aux
 refs=defaultdict(list);sections={}
 for i in range(ns):
  off=20+opt+40*i
  rr=struct.unpack_from('<I',d,off+24)[0]
  nr=struct.unpack_from('<H',d,off+32)[0]
  flags=struct.unpack_from('<I',d,off+36)[0]
  label=d[off:off+8].split(b'\0')[0].decode('latin1')
  sections[i+1]=(label,flags)
  skip=0
  if nr==65535 and flags&0x01000000:
   nr=struct.unpack_from('<I',d,rr)[0];skip=1
  for j in range(skip,nr):
   ix=struct.unpack_from('<I',d,rr+10*j+4)[0]
   if ix in sy:refs[i+1].append(sy[ix])
 return defs,refs,sections

for graph,targets in jobs.items():
 b=base/graph; bp=b.resolve().as_posix();edges={};deps={};key=None;flags={};latest=[]
 for l in (b/'build.ninja').read_text(errors='replace').splitlines():
  m=re.match(r'^build (.+?):\s+\S+\s+(.*)',l)
  if m:
   ins=m[2].split(' |')[0].split()
   latest=m[1].split()
   for out in latest:
    if out!='|' and not (out.lower().endswith('.lib') and latest[0].lower().endswith(('.exe','.dll'))):edges[out]=ins
  elif l.startswith('  cflags ='):
   for out in latest:flags[out]=l
 for l in cmd('ninja','-C',str(b),'-t','deps').splitlines():
  m=re.match(r'^(.+?): #deps .*\((VALID|STALE)\)',l)
  if m:
   key=m[1] if m[2]=='VALID' else None
   if key:deps[key]=set()
  elif key and l.startswith('    '):
   f=path(l.strip(),bp)
   if f:deps[key].add(f)
 for target in targets:
  seen=set();obj={};pending=[target];assert target in edges,target
  while pending:
   k=pending.pop()
   if k in seen:continue
   seen.add(k);vs=edges.get(k,[])
   if k.endswith('.obj'):
    f=next((path(v,bp) for v in vs if v.lower().endswith(('.c','.cpp','.cxx','.asm'))),'')
    if not f and vs:
     g=pp.basename(vs[0]).lower()
     if g in ('service_c.c','service_s.c'):f='src/common/protocol/service.idl'
     elif g=='rpc_management_c.c':f='src/common/protocol/rpc_management.idl'
     elif g=='base_vdm_config.c':f='src/non-mvdm/base/win32/client/vdm.c'
    if owner(f):obj[k]=f
    else:gaps[target].add('unattributed-object '+k)
   else:pending.extend(v for v in vs if v in edges)
  objs=defaultdict(list)
  for k,p in obj.items():
   if owner(p) in ('frontend-base','worker-base'): common_coverage[p].add(graph+'/'+target)
   objs[pp.basename(k).lower()].append(k)
   if k not in deps:gaps[target].add('no-valid-includes '+k)
   selected=deps.get(k,set())
   for t in selected:add('compiled-headers',owner(p),owner(t),p,'',t,'MSVC include closure',target)
   for f in selected|{p}:
    if not owner(f):continue
    for n,h in directives(f):
     opts=byname[pp.basename(h).lower()] & selected
     ts=[t for t in opts if ('/'+t.lower()).endswith('/'+h.lower())]
     if len(ts)==1:add('resolved-includes',owner(f),owner(ts[0]),f,n,ts[0],h,target)
  for k,p in obj.items():
   for typ,h in re.findall(r'/(FI|I)\s*"([^"]+)"',flags.get(k,'')):
    v=h.replace('$:',':').replace('\\','/')
    if '/src/' in v:
     rel='src/'+v.split('/src/',1)[1];dest=owner(rel+'/')
     add('forced-includes' if typ=='FI' else 'search-paths',owner(p),dest,p,'',rel,'/'+typ,target)
  mp=b/(target+'.map')
  if target=='VDMREDIR.dll' and (D/(target+'.map')).exists():mp=D/(target+'.map')
  if not mp.exists():gaps[target].add('no-link-map')
  symbols={};symbol_kinds={};live=defaultdict(set)
  if mp.exists():
   public=True
   for l in mp.read_text(errors='replace').splitlines():
    if l.strip()=='Static symbols':public=False
    m=re.match(r'^\s+[\da-fA-F]+:[\da-fA-F]+\s+(\S+)\s+[\da-fA-F]+\s+(?:[fi]\s+)*(\S+)\s*$',l)
    if not m:continue
    nm,tag=m.groups();ar,sep,on=tag.lower().rpartition(':');ks=objs[on]
    if sep:
     matches=[k for k in ks if k in edges.get(ar+'.lib',[])]
     ks=matches
    if len(ks)==1:
     live[ks[0]].add(nm)
     if public:
      symbols[nm]=obj[ks[0]]
      symbol_kinds[nm]='function' if 'f' in l.split()[3:-1] else 'data-or-untyped'
  for k,ns in live.items():
   add('foreign-source-members',('' if target.lower()=='vcdex.dll' else dlls[target.lower()]),owner(obj[k]),target,'',obj[k],'link map object',target)
   p=obj[k]
   for sec,fn,nm in coff(str(b/k)):
    t=symbols.get(nm)
    if t and bad(owner(p),owner(t)) and any(x in ns for x in fn):add('generated-rpc-relocations' if p.endswith('.idl') or t.endswith('.idl') else 'linked-relocations',owner(p),owner(t),p,sec,t,nm,target)

  # Include anonymous/local data sections reached from linked map roots.
  # Unrooted references are candidates, never counted as live violations.
  for k,live_names in live.items():
   p=obj[k]
   if owner(p)!='compat':continue
   definitions,refs,section_info=coff_sections(str(b/k))
   group=graph+'/'+target
   counts=expanded_stats.setdefault(group,dict(objects=0,rooted_sections=0,reachable_sections=0,external_relocations=0,resolved_relocations=0))
   counts['objects']+=1
   reachable={sec for sec,names in definitions.items() if names&live_names}
   counts['rooted_sections']+=len(reachable)
   todo=list(reachable)
   while todo:
    sec=todo.pop()
    for nm,target_sec,storage in refs.get(sec,[]):
     if target_sec>0 and target_sec not in reachable:
      reachable.add(target_sec);todo.append(target_sec)
   counts['reachable_sections']+=len(reachable)
   for sec,rows in refs.items():
    for nm,target_sec,storage in rows:
     if target_sec!=0 or storage not in (2,105):continue
     t=symbols.get(nm)
     if sec in reachable:
      counts['external_relocations']+=1
      if t:counts['resolved_relocations']+=1
      else:unresolved_symbols[group].add(nm)
     if t and owner(t) in ('non-mvdm','mvdm-host'):
      classification='expanded-live-links' if sec in reachable else 'unrooted-link-candidates'
      label,flags=section_info[sec]
      add(classification,owner(p),owner(t),p,sec,t,
          nm+'; target='+symbol_kinds.get(nm,'unknown')+'; source='+label,target)

  # Only classify foreign source ownership for F-layer binaries; VCDEX is a product, not a C component.
  pe=cmd('objdump','-p',str(b/target))
  for imp in re.findall(r'DLL Name:\s*(\S+)',pe,re.I):
   add('binary-imports',dlls[target.lower()],dlls.get(imp.lower(),''),target,'',imp,'PE import',target)
  for k,p in obj.items():
   for no,line in enumerate(Path(p).read_bytes().decode('latin1').splitlines(),1):
    for raw in re.findall(r'"([^"\n]*)"',line):
     for image,to in dlls.items():
      if image in raw.lower(): add('runtime-image-names',owner(p),to,p,no,image,raw,target)
  stats[graph+"/"+target]=(len(obj),len(live),len(symbols))
 output()
(D/'coverage.json').write_text(json.dumps({'head':cmd('git','rev-parse','HEAD').strip(),'objects_mapped_and_linkmap_objects_and_symbols':stats,'gaps':{k:sorted(v) for k,v in gaps.items()}},indent=2))
for kind,rows in records.items():print(kind,len(rows),sorted(Counter((r[0],r[1]) for r in rows).items(),key=lambda x:-x[1])[:4])

(D/'common-coverage.json').write_text(json.dumps({k:sorted(v) for k,v in sorted(common_coverage.items())},indent=2),encoding='utf-8')

(D/"expanded-coverage.json").write_text(json.dumps({"stats":expanded_stats,"unresolved_symbols":{k:sorted(v) for k,v in unresolved_symbols.items()},"limitation":"Map-resolved sources only. Missing maps, import targets and unassigned/common symbols require separate resolution."},indent=2))
