import csv,hashlib,json,re,runpy,struct
from pathlib import Path
A=Path('artifacts/mvdm-audit'); M=Path('src/mvdm'); B=Path('build/.ninja/msvc-x86-release/worker'); O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
rels=['softpc.new/base/ccpu386/cmpxchg.c','softpc.new/base/ccpu386/xadd.c']
def sha(p): return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py')); graph=(B/'build.ninja').read_text().replace('$:',':'); mp=(B/'ntvdm.exe.map').read_text(); rows=[]
 frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}; ledger={r['relative_path']:r for r in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))}
 for rel in rels:
  name=Path(rel).stem; f=frozen[rel]; l=ledger[rel]; hashes=[sha(root/rel) for root in (M,*O)]; assert hashes==[f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]; assert l['shard']=='2' and l['status']=='pending'
  source=(M/rel).read_text(encoding='latin1'); assert '#ifdef SPC486' in source
  edge=next(x for x in graph.splitlines() if x.startswith('build obj/ccpu/'+name+'.obj:')); lib=next(x for x in graph.splitlines() if x.startswith('build original-ccpu386.lib:')); assert 'obj/ccpu/'+name+'.obj' in lib.split()
  obj=B/('obj/ccpu/'+name+'.obj'); d=obj.read_bytes(); machine,n,_,_,_,opt,_=struct.unpack_from('<HHIIIHH',d); assert machine==0x14c and opt==0
  sec=[]
  for i in range(n):
   off=20+i*40; flags=struct.unpack_from('<I',d,off+36)[0]; assert not flags&0x20; sec.append(dict(name=d[off:off+8].rstrip(b'\0').decode('ascii'),flags=flags))
  assert not re.search(r'(?im)^.*\bccpu386:'+name+r'\.obj\s*$',mp); rows.append(dict(path=rel,hashes=hashes,edge=edge,object_sha256=sha(obj),sections=sec))
 src=(M/'softpc.new/base/ccpu386/c_main.c').read_text(encoding='latin1')
 for case,name in [('0xb0','CMPXCHG8'),('0xb1','CMPXCHG16'),('0xc0','XADD'),('0xc1','XADD')]:
  chunk=src[src.index('case  '+case):]; chunk=chunk[:chunk.index('break;')]; assert '#ifdef SPC486' in chunk and name in chunk and '#else' in chunk and 'Int6();' in chunk
 image=h['Image'](Path('assets/release/ntvdm.exe')); cached=h['Image'](B/'ntvdm.exe'); assert image.base==cached.base and image.code==cached.code and all(image.data[s:e]==cached.data[s:e] for s,e,_ in image.code)
 sym=next(s for s in h['symbols'](mp) if s['name']=='_c_cpu_simulate' and s['owner'].lower().endswith(':c_main.obj') and s['function']); binding=h['bind'](image,h['Coff'](B/'obj/ccpu/c_main.obj'),sym)
 return dict(result='pass',files=rows,dispatch_source_sha256=sha(M/'softpc.new/base/ccpu386/c_main.c'),dispatch_entry_binding=binding,release_sha256=sha('assets/release/ntvdm.exe'),cached_sha256=sha(B/'ntvdm.exe'),map_sha256=sha(B/'ntvdm.exe.map'),graph_sha256=sha(B/'build.ninja'),scope='Objects have no code; dispatch entry binding does not prove instruction execution or all CPU backends')
if __name__=='__main__': print(json.dumps(inspect(),indent=2))
