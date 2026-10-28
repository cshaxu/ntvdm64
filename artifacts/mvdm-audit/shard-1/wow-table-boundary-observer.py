"""Verify all original WOW table boundaries in the linked release image."""
import csv,hashlib,json,re,runpy,struct
from pathlib import Path
A=Path('artifacts/mvdm-audit');B=Path('build/.ninja/msvc-x86-release/wow32')
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 h=runpy.run_path(str(A/'host-tail-image-observer.py'));rel='wow32/wowtbl.c';src=Path('src/mvdm')/rel
 originals=[Path('O:/repos.external/OpenNT/base/mvdm')/rel,Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/rel]
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:row=next(r for r in csv.DictReader(f) if r['relative_path']==rel)
 assert all(sha(p)==sha(src) for p in originals) and sha(src)==row['current_sha256']==row['opennt_sha256']==row['opennt45_sha256']
 graph=(B/'build.ninja').read_text().replace('$:',':');edge=next(x for x in graph.splitlines() if x.startswith('build obj/wowtbl.obj:'));link=next(x for x in graph.splitlines() if x.startswith('build wow32.dll | wow32.lib:'));assert src.resolve().as_posix().lower() in edge.lower() and 'obj/wowtbl.obj' in link.split()
 image=h['Image'](Path('assets/release/WOW32.DLL'));cached=h['Image'](B/'wow32.dll');assert image.code==cached.code and all(image.data[a:b]==cached.data[a:b] for a,b,_ in image.code)
 symbols=h['symbols']((B/'wow32.dll.map').read_text());obj=h['Coff'](B/'obj/wowtbl.obj');base=next(x for x in symbols if x['name']=='_aw32WOW' and x['owner']=='wowtbl.obj');entry=next(x for x in symbols if x['name']=='_InitThunkTableOffsets@0' and x['owner']=='wowtbl.obj');binding=h['bind'](image,obj,entry)
 _,start,section=next(s for s in obj.symbols.values() if s[0]=='_aw32WOW' and s[2]>0);body,relocs=obj.sections[section-1]
 headers=['wktbl2.h','wutbl2.h','wgtbl2.h','wkbdtbl2.h','wstbl2.h','wshtbl2.h','wwstbl2.h','wthtbl2.h','wmmtbl2.h','wcmdgtbl.h'];idx=0;parts=[]
 for name in headers:
  t=(src.parent/name).read_text(encoding='latin1');assert not re.search(r'^\s*#\s*(?:if|ifdef|ifndef|else|elif|endif)\b',t,re.M);n=len(re.findall(r'W32FUN\(',t));sep=idx+n
  assert struct.unpack_from('<I',body,start+sep*4)[0]==0xffffffff and struct.unpack('<I',h['data_at'](image,base['va']+sep*4,4))[0]==0xffffffff
  assert not [r for r in relocs if r[0]==start+sep*4]
  parts.append(dict(header=name,base=idx,entries=n,separator=sep));idx=sep+1
 dllsym=next(x for x in symbols if x['name']=='_W32DllInitialize@12' and x['owner']=='wow32.obj');dllobj=h['Coff'](B/'obj/wow32.obj');dllbinding=h['bind'](image,dllobj,dllsym);raw,rels=dllobj.function(dllsym['name']);offset=next(o for o,t,k in rels if t==entry['name'] and k==20);word=struct.unpack('<I',h['data_at'](image,dllsym['va']+offset,4))[0];add=struct.unpack_from('<i',raw,offset)[0];assert word==(entry['va']+add-(dllsym['va']+offset+4))&0xffffffff
 tablesym=next(x for x in symbols if x['name']=='_W32GetTableOffsets@4' and x['owner']=='wow32.obj');tablebinding=h['bind'](image,dllobj,tablesym)
 paths=[src,*originals,B/'build.ninja',B/'obj/wowtbl.obj',B/'obj/wow32.obj',B/'wow32.dll.map',Path('assets/release/WOW32.DLL'),Path('src/mvdm/inc/wow.h'),Path('src/mvdm/wow32/wow32.c'),*[src.parent/n for n in headers]]
 return dict(result='pass',source=rel,source_sha256=sha(src),catalog_ids=row['wow_source_ids'],build_edge=edge,table_base=base['va'],parts=parts,init_binding=binding,attach_binding=dllbinding,guest_copy_binding=tablebinding,inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],limitations='Image boundaries and selected bodies only; no runtime initialization, guest publication or each entry behavioral validation.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
