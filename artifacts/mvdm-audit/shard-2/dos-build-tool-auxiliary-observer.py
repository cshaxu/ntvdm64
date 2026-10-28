import csv,hashlib,json,re,subprocess
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));names=('dos/v86/h/process.h','dos/v86/h/stddef.h','dos/v86/lib/oldnames.lib','dos/v86/tools/src/getsize/getsize.def');B=Path('build/.ninja/msvc-x86-release/worker').resolve();W=Path('build/.ninja/msvc-x86-release/wow32').resolve()
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
def inspect():
 f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};l={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))};lookup={str((M/n).resolve()).lower():n for n in names};hits={n:[] for n in names};graphs={}
 for base in (B,W):
  raw=subprocess.run(['ninja','-C',str(base),'-t','deps'],check=True,capture_output=True,text=True).stdout;obj='';valid=False
  for line in raw.splitlines():
   if line and not line[0].isspace() and ': #deps ' in line:obj=line.split(': #deps ',1)[0];valid='(VALID)' in line
   elif line.startswith('    ') and valid:
    n=lookup.get(str((base/line.strip()).resolve()).lower())
    if n and (base.name+'/'+obj) not in hits[n]:hits[n].append(base.name+'/'+obj)
  graph=(base/'build.ninja').read_text(encoding='utf-8').replace('$:',':').replace('\\','/').lower();graphs[base.name]=sha(base/'build.ninja')
  for name in names: assert str((M/name).resolve()).replace('\\','/').lower() not in graph
 rows=[]
 for n in names:
  hashes=[sha(root/n) for root in (M,*O)];assert hashes==[f[n]['current_sha256'],f[n]['opennt_sha256'],f[n]['opennt45_sha256']] and l[n]['shard']=='2' and l[n]['status']=='pending';rows.append(dict(path=n,hashes=hashes,valid_dependencies=hits[n]))
 assert not any(hits.values());get=(M/'dos/v86/tools/src/getsize/getsize.c').read_text(encoding='latin1').lower();assert all(s in get for s in ('ntdos.map','ntio.map','bdsize.inc')); assert re.search(r'(?im)^\s*getsize\s*$',(M/'dos/v86/doskrnl/dos/makedos.cmd').read_text(encoding='latin1'));sources=(M/'dos/v86/tools/src/getsize/sources').read_text(encoding='latin1').lower();assert 'sources=getsize.c' in sources and 'buildtool=1' in sources
 return dict(result='pass',files=rows,graphs=graphs,getsize_source_sha256=sha(M/'dos/v86/tools/src/getsize/getsize.c'),getsize_sources_sha256=sha(M/'dos/v86/tools/src/getsize/sources'),makedos_sha256=sha(M/'dos/v86/doskrnl/dos/makedos.cmd'),scope='Captured normalized VALID deps and literal graph paths; external tools, unrecorded/stale dependencies and historical include resolution unproven')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
