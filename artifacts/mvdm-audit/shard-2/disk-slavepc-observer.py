import csv,hashlib,json,re,runpy,struct
from pathlib import Path
A=Path('artifacts/mvdm-audit'); M=Path('src/mvdm'); B=Path('build/.ninja/msvc-x86-release/worker'); O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')); rel='softpc.new/base/disks/gfi_sflp.c'
def sha(p): return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py')); f=next(r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig')) if r['relative_path']==rel); l=next(r for r in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig')) if r['relative_path']==rel); hashes=[sha(root/rel) for root in (M,*O)]; assert hashes==[f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]; assert l['shard']=='2' and l['status']=='pending'
 source=(M/rel).read_text(encoding='latin1'); assert '#ifdef SLAVEPC' in source and re.search(r'#endif\s*/\* SLAVEPC \*/\s*$',source)
 graph=(B/'build.ninja').read_text().replace('$:',':'); edge=next(x for x in graph.splitlines() if x.startswith('build obj/disks/gfi_sflp.obj:')); lib=next(x for x in graph.splitlines() if x.startswith('build original-softpc-disks.lib:')); assert 'obj/disks/gfi_sflp.obj' in lib.split()
 obj=B/'obj/disks/gfi_sflp.obj'; d=obj.read_bytes(); machine,n,_,_,_,opt,_=struct.unpack_from('<HHIIIHH',d); assert machine==0x14c and opt==0; sec=[]
 for i in range(n):
  off=20+i*40; flags=struct.unpack_from('<I',d,off+36)[0]; assert not flags&0x20; sec.append(dict(name=d[off:off+8].rstrip(b'\0').decode('ascii'),flags=flags))
 mp=(B/'ntvdm.exe.map').read_text(); assert not re.search(r'(?im)^.*\bsoftpc-disks:gfi_sflp\.obj\s*$',mp)
 gfi=(M/'softpc.new/base/disks/gfi.c').read_text(encoding='latin1'); assert '#ifdef SLAVEPC' in gfi and 'gfi_slave_active(hostID, active, err)' in gfi
 image=h['Image'](Path('assets/release/ntvdm.exe')); cached=h['Image'](B/'ntvdm.exe'); assert image.base==cached.base and image.code==cached.code and all(image.data[s:e]==cached.data[s:e] for s,e,_ in image.code)
 sym=next(s for s in h['symbols'](mp) if s['name']=='_fla_inb' and s['owner'].lower().endswith(':fla.obj') and s['function']); binding=h['bind'](image,h['Coff'](B/'obj/disks/fla.obj'),sym)
 return dict(result='pass',source=rel,hashes=hashes,edge=edge,object_sha256=sha(obj),sections=sec,active_fdc_binding=binding,gfi_sha256=sha(M/'softpc.new/base/disks/gfi.c'),fla_sha256=sha(M/'softpc.new/base/disks/fla.c'),release_sha256=sha('assets/release/ntvdm.exe'),cached_sha256=sha(B/'ntvdm.exe'),map_sha256=sha(B/'ntvdm.exe.map'),graph_sha256=sha(B/'build.ninja'),scope='Inactive SLAVEPC module in captured build; neighboring FDC binding does not prove floppy runtime behavior')
if __name__=='__main__': print(json.dumps(inspect(),indent=2))
