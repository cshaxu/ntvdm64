import csv,hashlib,json,runpy
from pathlib import Path
A=Path('artifacts/mvdm-audit'); M=Path('src/mvdm'); B=Path('build/.ninja/msvc-x86-release/wow32'); O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')); rel='wow32/wcall32.c'
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py'));f=next(x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig')) if x['relative_path']==rel); l=next(x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig')) if x['relative_path']==rel); hashes=[sha(root/rel) for root in (M,*O)];assert hashes==[f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']] and l['shard']=='2' and l['status'] in ('pending','partial')
 graph=(B/'build.ninja').read_text().replace('$:',':');edge=next(x for x in graph.splitlines() if x.startswith('build obj/wcall32.obj:'));link=next(x for x in graph.splitlines() if x.startswith('build wow32.dll |')); assert 'obj/wcall32.obj' in link.split()
 image=h['Image'](Path('assets/release/WOW32.DLL'));cached=h['Image'](B/'wow32.dll');assert image.base==cached.base and image.code==cached.code and all(image.data[s:e]==cached.data[s:e] for s,e,_ in image.code)
 syms=h['symbols']((B/'wow32.dll.map').read_text()); coff=h['Coff'](B/'obj/wcall32.obj'); bindings=[]
 for name in ('_W32LocalAlloc@12','_W32LocalReAlloc@20','_W32EditNextWord@20','_W32InitDlg@8'):
  sym=next(s for s in syms if s['name']==name and s['owner'].lower()=='wcall32.obj' and s['function']);bindings.append(h['bind'](image,coff,sym))
 reg=(M/'wow32/wow32.c').read_text(encoding='latin1'); assert all(n in reg for n in ('W32LocalAlloc','W32LocalReAlloc','W32EditNextWord','W32InitDlg'))
 return dict(result='pass',source=rel,hashes=hashes,edge=edge,object_sha256=sha(B/'obj/wcall32.obj'),registration_source_sha256=sha(M/'wow32/wow32.c'),bindings=bindings,release_sha256=sha('assets/release/WOW32.DLL'),cached_sha256=sha(B/'wow32.dll'),map_sha256=sha(B/'wow32.dll.map'),graph_sha256=sha(B/'build.ninja'),scope='Object body binding only; no runtime or callback registration-table image binding')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
