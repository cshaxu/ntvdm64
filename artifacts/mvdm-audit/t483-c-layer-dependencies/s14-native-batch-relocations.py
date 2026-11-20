from pathlib import Path
import struct,re,json,collections,hashlib,argparse
arguments=argparse.ArgumentParser()
arguments.add_argument('--output-root',default='build/t483-s14-native-closure')
args=arguments.parse_args()
root=Path('build/.ninja/msvc-x86-release/wow32');out=Path(args.output_root);out.mkdir(exist_ok=True)
graph=(root/'build.ninja').read_text();link=next(l for l in graph.splitlines() if l.startswith('build wow32.dll |'))
objects=[root/p for p in link.split(': wow_dll_link ',1)[1].split(' | ')[0].split() if p.endswith('.obj')]
def parse(p):
 b=p.read_bytes();machine,nsec,_,ptr,num,opts,_=struct.unpack_from('<HHIIIHH',b);assert machine==0x14c and opts==0,p
 strings=b[ptr+18*num:];syms={};defs={};i=0
 while i<num:
  at=ptr+18*i;raw=b[at:at+8];zero,offset=struct.unpack('<II',raw);name=(strings[offset:].split(b'\0',1)[0] if zero==0 else raw.split(b'\0',1)[0]).decode('ascii',errors='replace');value,sec,typ,store,aux=struct.unpack_from('<IhHBB',b,at+8);syms[i]=(name,sec,store)
  if store==2 and sec>0:defs[name]=sec
  i+=1+aux
 refs={}
 for sec in range(1,nsec+1):
  at=20+(sec-1)*40;rp=struct.unpack_from('<I',b,at+24)[0];count=struct.unpack_from('<H',b,at+32)[0];refs[sec]=[]
  for j in range(count):
   _,idx,_=struct.unpack_from('<IIH',b,rp+j*10)
   if idx not in syms:raise RuntimeError((p,idx))
   refs[sec].append(syms[idx])
 return defs,refs
parsed={};owners=collections.defaultdict(list)
for p in objects:
 defs,refs=parse(p);key=p.relative_to(root).as_posix();parsed[key]=(defs,refs)
 for symbol,sec in defs.items():owners[symbol].append((key,sec))
rows=json.loads(Path('artifacts/mvdm-audit/t483-c-layer-dependencies/s14-batch-priority.json').read_text());ids=set(rows['priority_batches'][0]['ids']);ledger=[json.loads(l) for l in Path('artifacts/mvdm-audit/t483-c-layer-dependencies/points.jsonl').read_text().splitlines()];names={r['symbol'] for r in ledger if r['id'] in ids}
plain=lambda n:re.sub(r'@\d+$','',n.lstrip('_'))
queue=[];starts={}
for symbol,providers in owners.items():
 if plain(symbol) in names:
  assert len(providers)==1,(symbol,providers);starts[symbol]=providers[0];queue+=providers
assert {plain(symbol) for symbol in starts}==names, (names-{plain(symbol) for symbol in starts})
seen=set();edges=[];local_edges=[];external=collections.Counter();ambiguous={}
while queue:
 node=queue.pop()
 if node in seen:continue
 seen.add(node);obj,sec=node
 for symbol,target_sec,storage in parsed[obj][1][sec]:
  if target_sec>0:local_edges.append((obj,sec,symbol,obj,target_sec));queue.append((obj,target_sec));continue
  if storage!=2:continue
  providers=owners.get(symbol,[])
  if len(providers)==1:
   target=providers[0];edges.append((obj,sec,symbol,target[0],target[1]));queue.append(target)
  elif providers:ambiguous[symbol]=providers
  else:external[symbol]+=1
files=collections.Counter(obj for obj,sec in seen)
result={'method':'Relocation reachability from the current priority-batch public symbols through actual WOW32 direct link objects; section-level COMDAT traversal includes data references. Not a runtime trace or full library/indirect-call closure.','roots':starts,'objects_read':len(objects),'reachable_objects':len(files),'reachable_sections':len(seen),'reachable_object_sections':dict(sorted(files.items())),'edges':edges,'external_leaves':dict(sorted(external.items())),'ambiguous':ambiguous,'input_hashes':{p.relative_to(root).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in objects if p.relative_to(root).as_posix() in files}}
result['local_edges']=local_edges
result['section_symbols']={obj:{str(sec):[name for name,s in defs.items() if s==sec] for o,sec in seen if o==obj} for obj,(defs,refs) in parsed.items() if obj in files}
(out/'closure.json').write_text(json.dumps(result,indent=2)+'\n',newline='\n');print('roots',len(starts),'objects',len(files),'sections',len(seen),'ambiguities',len(ambiguous));print('\n'.join(f'{k}: {v}' for k,v in sorted(files.items()) if 'wow32-dll/' in k))
