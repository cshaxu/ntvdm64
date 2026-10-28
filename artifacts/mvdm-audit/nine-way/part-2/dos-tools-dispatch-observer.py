import csv,hashlib,json,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));selected=[]
for x in csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')):
 n=x['path']
 if x['bucket']=='2' and x['kind']=='mvdm' and (n.startswith(('dos/v86/doskrnl/dos/','dos/v86/redir/','dos/v86/tools/','dos/v86/lib/')) or n in ('dpmi32/i386/dpmimem.c','ieuvddex/alpha/helpp.c')):selected.append(x)
assert len(selected)==20
f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};rows=[]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''
for x in selected:
 n=x['path'];v=f[n];hs=[sha(root/n) for root in (M,*O)];assert hs==[v['current_sha256'],v['opennt_sha256'],v['opennt45_sha256']];rows.append(dict(key=x['key'],path=n,hashes=hs))
mk=(M/'dos/v86/redir/makefile').read_text(encoding='latin1').lower();rs=(M/'dos/v86/redir/redir.asm').read_text(encoding='latin1').lower();assert 'localmac.inc' in mk and 'include rdrmisc.inc' in rs and 'h2inc -c' in mk
m=(M/'dos/v86/messages/makefile').read_text(encoding='latin1').lower();get=(M/'dos/v86/doskrnl/dos/makedos.cmd').read_text(encoding='latin1').lower();him=(M/'dos/v86/dev/himem/makefile').read_text(encoding='latin1').lower();kernel=(M/'dos/v86/doskrnl/dos/makefile').read_text(encoding='latin1').lower();assert all((s in v) for s,v in (('buildidx',m),('getsize',get),('stripdd',him),('stripz',kernel)))
help=(M/'ieuvddex/help.c').read_text(encoding='latin1').lower();alpha=(M/'ieuvddex/alpha/helpp.c').read_text(encoding='latin1').lower();assert 'helpp();' in help and 'currently no processor specific extensions' in alpha
mp=Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map');maptext=mp.read_text(encoding='latin1').lower();assert 'dpmimemr.obj' in maptext
zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);e=next(x for x in z.infolist() if x.filename.lower()=='winnt/system32/redir.exe');archive=hashlib.sha256(z.read(e)).hexdigest().upper();installed=sha(Path('O:/winnt/system32/REDIR.EXE'));supp=sha(M/'dos/v86/redir/obj/redir.exe');assert archive==installed and archive!=supp
print(json.dumps(dict(result='pass',files=rows,worker_map_sha256=sha(mp),archive_sha256=sha(zpath),packaged_redir_sha256=archive,installed_redir_sha256=installed,supplemental_redir_sha256=supp,scope='File identities, observed build/source edges and snapshot image; no guest execution or full behavioral equivalence'),indent=2))
