import csv,hashlib,json,runpy,struct
from pathlib import Path
A=Path('artifacts/mvdm-audit'); M=Path('src/mvdm'); B=Path('build/.ninja/msvc-x86-release/worker'); O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')); names=('dasm','ega_dump','profile','trace')
def sha(p): return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py')); frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}; ledger={r['relative_path']:r for r in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))}; graph=(B/'build.ninja').read_text().replace('$:',':'); mp=(B/'ntvdm.exe.map').read_text(); rows=[]
 for name in names:
  rel='softpc.new/base/debug/'+name+'.c'; f=frozen[rel]; l=ledger[rel]; hashes=[sha(root/rel) for root in (M,*O)]; assert hashes==[f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]; assert l['shard']=='2' and l['status']=='pending'; edge=next(x for x in graph.splitlines() if x.startswith('build obj/base-debug/'+name+'.obj:')); libs=[x.split(':')[0][6:] for x in graph.splitlines() if x.startswith('build ') and '.lib:' in x and 'obj/base-debug/'+name+'.obj' in x.split()]; assert libs
  obj=B/('obj/base-debug/'+name+'.obj'); d=obj.read_bytes(); machine,n,_,_,_,opt,_=struct.unpack_from('<HHIIIHH',d); assert machine==0x14c and opt==0; sec=[]
  for i in range(n):
   off=20+i*40; flags=struct.unpack_from('<I',d,off+36)[0]; sec.append(dict(name=d[off:off+8].rstrip(b'\0').decode('ascii'),flags=flags,code=bool(flags&0x20)))
  owners=[x.strip() for x in mp.splitlines() if (':'+name+'.obj') in x.lower()]; rows.append(dict(path=rel,hashes=hashes,edge=edge,libraries=libs,object_sha256=sha(obj),sections=sec,map_owners=owners))
 assert not any(s['code'] for r in rows[:2] for s in r['sections']); assert all(any(s['code'] for s in r['sections']) for r in rows[2:]); assert all(not r['map_owners'] for r in rows[:3]); assert len(rows[3]['map_owners'])==2 and all(' f ' not in x for x in rows[3]['map_owners']); assert all(any('_'+x in line for line in rows[3]['map_owners']) for x in ('io_verbose','sub_io_verbose'))
 image=h['Image'](Path('assets/release/ntvdm.exe')); cached=h['Image'](B/'ntvdm.exe'); assert image.base==cached.base and image.code==cached.code and all(image.data[s:e]==cached.data[s:e] for s,e,_ in image.code)
 return dict(result='pass',files=rows,release_sha256=sha('assets/release/ntvdm.exe'),cached_sha256=sha(B/'ntvdm.exe'),map_sha256=sha(B/'ntvdm.exe.map'),graph_sha256=sha(B/'build.ninja'),scope='Release code-section and map evidence only; function behavior and other profiles not tested')
if __name__=='__main__': print(json.dumps(inspect(),indent=2))
