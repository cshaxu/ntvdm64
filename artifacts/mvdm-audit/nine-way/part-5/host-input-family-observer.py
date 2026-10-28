import csv,hashlib,json,re,struct,subprocess
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));allrows=list(csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')));own=[x for x in allrows if x['bucket']=='5' and x['kind']=='mvdm' and not (x['path'].startswith(('softpc.new/bios/','softpc.new/data/','softpc.new/roms/','vdd/','vdmdbg/','vdmexts/')) or x['path']=='softpc.new/debugger/vdmdebug.c')];assert len(own)==39
f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};lookup={str((M/x['path']).resolve()).lower():x['path'] for x in own};basenames={Path(x['path']).name.lower() for x in own};hits={x['path']:[] for x in own};graphhits={x['path']:[] for x in own}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
for name in ('worker','wow32'):
 B=Path('build/.ninja/msvc-x86-release')/name;B=B.resolve();g=(B/'build.ninja').read_text().replace('$:',':').replace('\\','/').lower()
 for x in own:
  path=str((M/x['path']).resolve()).replace('\\','/').lower()
  if path in g:graphhits[x['path']].append(name)
 raw=subprocess.run(['ninja','-C',str(B),'-t','deps'],check=True,capture_output=True,text=True).stdout;obj='';valid=False
 for line in raw.splitlines():
  if line and not line[0].isspace() and ': #deps ' in line:obj=line.split(': #deps ',1)[0];valid='(VALID)' in line
  elif line.startswith('    ') and valid:
   val=line.strip()
   if Path(val).name.lower() not in basenames:continue
   rel=lookup.get(str((B/val).resolve()).lower())
   if rel and name+'/'+obj not in hits[rel]:hits[rel].append(name+'/'+obj)
rows=[]
for x in own:
 n=x['path'];v=f[n];hs=[sha(root/n) for root in (M,*O)];assert hs==[v['current_sha256'],v['opennt_sha256'],v['opennt45_sha256']];rows.append(dict(key=x['key'],path=n,hashes=hs,exact_graphs=graphhits[n],valid_dependency_objects=hits[n]))
objs=[]
for path in ('obj/support/get_env.obj','obj/video/ega_trcr.obj'):
 p=Path('build/.ninja/msvc-x86-release/worker')/path;d=p.read_bytes();count=struct.unpack_from('<H',d,2)[0];flags=[struct.unpack_from('<I',d,20+i*40+36)[0] for i in range(count)];objs.append(dict(path=path,sha256=sha(p),has_code=any(x&0x20 for x in flags)))
assert objs[0]['has_code'] and not objs[1]['has_code']
print(json.dumps(dict(result='pass',files=rows,objects=objs,scope='Exact normalized current VALID dependency paths and graph mentions; stale records, semantic activity and other configurations remain unproven'),indent=2))
