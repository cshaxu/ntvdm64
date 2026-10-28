import csv,hashlib,json,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));N=('bde/bde.def','bde/data.c','bde/disasm.c','softpc.new/debugger/vdmdebug.c','vdmdbg/vdmdbg.def','vdmexts/i386/vdmtib.c','vdmexts/vdmexts.def','wow16/wowdeb/wowdeb.def')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
def inspect():
 f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};l={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))};rows=[];graphs=[]
 for n in N:
  hs=[sha(root/n) for root in (M,*O)];assert hs==[f[n]['current_sha256'],f[n]['opennt_sha256'],f[n]['opennt45_sha256']] and l[n]['shard']=='2' and l[n]['status']=='pending';rows.append(dict(path=n,hashes=hs))
 for b in ('worker','wow32'):
  p=Path('build/.ninja/msvc-x86-release')/b/'build.ninja';g=p.read_text().replace('$:',':').replace('\\','/').lower();assert all(str((M/n).resolve()).replace('\\','/').lower() not in g for n in N);graphs.append(dict(path=str(p),sha256=sha(p)))
 bs=(M/'bde/sources').read_text(encoding='latin1').lower();es=(M/'vdmexts/sources').read_text(encoding='latin1').lower();ds=(M/'vdmdbg/sources').read_text(encoding='latin1').lower();debug=(M/'softpc.new/debugger/sources').read_text(encoding='latin1').lower();platform=(M/'vdmexts/platform.c').read_text(encoding='latin1').lower();vdm=(M/'vdmexts/i386/vdmtib.c').read_text(encoding='latin1').lower();wow=(M/'wow16/wowdeb/wowdeb.def').read_text(encoding='latin1').lower();assert all(x in bs for x in ('targetname=bde','targettype=dynlink','data.c','disasm.c')) and all(x in es for x in ('targetname=vdmexts','i386\\vdmtib.c')) and 'targetname=vdmdbg' in ds and 'targetname=vdmdebug' in debug and 'vdmtibp();' in platform and 'ntvdm!vdmtib' in vdm and 'wowkillremotetask' in wow
 zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);nn=('bde.dll','vdmdbg.dll','vdmexts.dll','vdmdebug.exe','wowdeb.exe');names=[x.filename for x in z.infolist() if any(x.filename.replace('\\','/').lower().endswith('/'+n) for n in nn)];assert not names
 return dict(result='pass',files=rows,graphs=graphs,archive_sha256=sha(zpath),named_archive_outputs=names,scope='Historical separate debugger/tool source and filename snapshot; no debug APIs runtime, alternate installation or replacement equivalence')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
