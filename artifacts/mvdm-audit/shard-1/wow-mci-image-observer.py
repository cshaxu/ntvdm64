"""Read-only evidence for the disabled original MCI reverse thunk and live path."""
import csv, hashlib, json, re, runpy, struct
from pathlib import Path
A=Path('artifacts/mvdm-audit'); B=Path('build/.ninja/msvc-x86-release/wow32')
def sha(p): return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py'))
 rel='wow32/wmmstru2.c'; src=Path('src/mvdm')/rel
 roots=[Path('O:/repos.external/OpenNT/base/mvdm')/rel,Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/rel]
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f: row=next(r for r in csv.DictReader(f) if r['relative_path']==rel)
 assert all(sha(p)==sha(src) for p in roots) and sha(src)==row['current_sha256']==row['opennt_sha256']==row['opennt45_sha256']
 assert row['dos_catalog_rows']=='1' and row['wow_catalog_rows']=='2'
 s=src.read_text(encoding='latin1'); assert re.search(r'^#if 0\s*$',s,re.M) and 'UnThunkMciCommand16' in s
 graph=(B/'build.ninja').read_text().replace('$:',':'); edge=next(x for x in graph.splitlines() if x.startswith('build obj/wmmstru2.obj:'))
 link=next(x for x in graph.splitlines() if x.startswith('build wow32.dll | wow32.lib:'))
 assert 'obj/wmmstru2.obj' in link.split() and src.resolve().as_posix().lower() in edge.lower()
 obj=h['Coff'](B/'obj/wmmstru2.obj'); assert not any('UnThunkMciCommand16' in x[0] for x in obj.symbols.values())
 syms=h['symbols']((B/'wow32.dll.map').read_text()); assert not any('UnThunkMciCommand16' in x['name'] for x in syms)
 image=h['Image'](Path('assets/release/WOW32.DLL')); cached=h['Image'](B/'wow32.dll'); assert image.code==cached.code and all(image.data[a:b]==cached.data[a:b] for a,b,_ in image.code)
 functions=[]
 for name, owner in [('_wow_original_WMM32CallProc32@4','wmmedia.obj'),('_WMM32CallProc32@4','wow_user_native_extension.obj')]:
  symbol=next(x for x in syms if x['name']==name and x['owner']==owner and x['function'])
  functions.append(h['bind'](image,h['Coff'](B/'obj'/('wow32-dll/'+owner if owner.startswith('wow_user_') else owner)),symbol))
 guest=[Path('src/mvdm/wow16/mmsystem')/n for n in ('init.c','mci.c','thunks.asm')]
 init,mc,asm=[x.read_text(encoding='latin1') for x in guest]
 assert '"winmm.dll"' in init and 'GetProcAddress32W( mmwow32Lib, "mci32Message" )' in init
 assert 'mciMessage( THUNK_MCI_SENDCOMMAND' in mc and 'call    FAR PTR MMCALLPROC32' in asm and 'push    mci32Message.sel' in asm
 host=Path('C:/Windows/SysWOW64/winmm.dll'); data=host.read_bytes(); pe=struct.unpack_from('<I',data,0x3c)[0]; machine,nsec=struct.unpack_from('<HH',data,pe+4); opts=struct.unpack_from('<H',data,pe+20)[0]; opt=pe+24; assert machine==0x14c and struct.unpack_from('<H',data,opt)[0]==0x10b
 erva=struct.unpack_from('<I',data,opt+96)[0]; sections=[]
 for i in range(nsec):
  p=opt+opts+i*40; vs,va,rs,ro=struct.unpack_from('<IIII',data,p+8); sections.append((va,max(vs,rs),ro))
 def off(r): return next(ro+r-va for va,size,ro in sections if va<=r<va+size)
 ed=off(erva); nn=struct.unpack_from('<I',data,ed+24)[0]; nrva=struct.unpack_from('<I',data,ed+32)[0]; n=off(nrva)
 names=[]
 for i in range(nn):
  p=off(struct.unpack_from('<I',data,n+4*i)[0]); names.append(data[p:data.index(b'\0',p)].decode('ascii'))
 assert 'mci32Message' in names
 files=[src,*roots,B/'build.ninja',B/'obj/wmmstru2.obj',B/'wow32.dll.map',B/'obj/wmmedia.obj',B/'obj/wow32-dll/wow_user_native_extension.obj',Path('assets/release/WOW32.DLL'),*guest,host]
 return dict(result='pass',source=rel,catalog_ids=row['wow_source_ids'],source_sha256=sha(src),build_edge=edge,disabled_symbol_absent=True,linked_functions=functions,host_winmm_machine=hex(machine),host_winmm_mci32Message_export=True,inputs=[{'path':str(p),'sha256':sha(p)} for p in files],limitations='Static evidence only: no proof of guest image lineage, installed host export compatibility, invocation, device functionality, or callback and lifetime semantics.')
if __name__=='__main__': print(json.dumps(inspect(),indent=2))
