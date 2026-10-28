import csv,hashlib,json,re,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');OUT=A/'nine-way/part-5';own=[x for x in csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='5' and x['kind']=='external'];ids={x['key'] for x in own};cat={x['id']:x for x in csv.DictReader(Path('O:/repos.hobby/ntvdm64-wow32-audit/management/source-file-catalog.csv').open(encoding='utf-8-sig')) if x['id'] in ids};assert len(cat)==len(own)==4773;own=[x for x in own if cat[x['key']]['role']!='host_provider_exact_dependency'];assert len(own)==4758
roots={'OpenNT':Path('O:/repos.external/OpenNT'),'OpenNT-4.5':Path('O:/repos.external/OpenNT-4.5')};z=zipfile.ZipFile('assets/winnt.zip');zn={x.replace('\\','/').split('/')[-1].lower() for x in z.namelist()};graphs=[Path('build/.ninja/msvc-x86-release')/b/'build.ninja' for b in ('worker','wow32')];gt=[p.read_text().replace('$:',':').replace('\\','/').lower() for p in graphs];contexts={};rows=[]
for x in own:
 c=cat[x['key']];p=roots[c['edition']]/x['path'];exists=p.is_file(); blob='';digest='';size='';verified='missing';refs=[];graph=[]
 if exists:
  d=p.read_bytes();size=len(d);digest=hashlib.sha256(d).hexdigest().upper();normal=d.replace(b'\r\n',b'\n');f=lambda q:hashlib.sha1(b'blob '+str(len(q)).encode()+b'\0'+q).hexdigest();raw=f(d);norm=f(normal);verified='raw' if raw==c['git_blob'] else ('crlf-to-lf' if norm==c['git_blob'] else 'mismatch');blob=c['git_blob'];directory=p.parent
  if directory not in contexts:
   targets=[q for q in directory.iterdir() if q.is_file() and (q.name.lower() in ('sources','makefile','dirs') or q.suffix.lower() in ('.rc','.inf','.def'))];contexts[directory]=[(q,q.read_text(encoding='latin1').replace('\\','/').splitlines()) for q in targets]
  for q,lines in contexts[directory]:
   if q==p:continue
   for i,line in enumerate(lines):
    if re.search(r'(?i)(?<![a-z0-9_.-])'+re.escape(p.name)+r'(?![a-z0-9_.-])',line):refs.append(str(q.relative_to(roots[c['edition']])).replace('\\','/')+':'+str(i+1)+':'+line.strip()[:200])
  canonical=str(p.resolve()).replace('\\','/').lower();graph=[str(q) for q,g in zip(graphs,gt) if canonical in g]
 suffix=p.suffix.lower();role={'.c':'source','.cpp':'source','.cxx':'source','.h':'declaration','.hxx':'declaration','.inc':'include','.rc':'resource script','.def':'module definition','.asm':'assembler source','.inf':'installation metadata','.txt':'documentation','.bmp':'bitmap data','.cur':'cursor data','.pfm':'font metrics','.ifi':'font data'}.get(suffix,'other original asset or build input')
 rows.append(dict(key=x['key'],path=x['path'],edition=c['edition'],catalog_role=c['role'],catalog_reason=c['relevance_reason'],catalog_blob=c['git_blob'],identity=verified,raw_sha256=digest,bytes=size,file_role=role,same_directory_build_refs=' | '.join(refs[:30]),exact_current_graph_path=' | '.join(graph),archive_basename_present=p.name.lower() in zn,decision='relevance pending',boundary='Identity and local filename references only; current semantic owner and actual host seam require proof'))
with (OUT/'external-envelope-owned.csv').open('w',newline='',encoding='utf-8') as f:
 w=csv.DictWriter(f,fieldnames=rows[0].keys());w.writeheader();w.writerows(rows)
from collections import Counter
print(json.dumps(dict(result='pass',files=len(rows),identity_counts=dict(Counter(x['identity'] for x in rows)),catalog_roles=dict(Counter(x['catalog_role'] for x in rows)),same_directory_refs=sum(bool(x['same_directory_build_refs']) for x in rows),exact_graph_hits=sum(bool(x['exact_current_graph_path']) for x in rows),archive_basename_hits=sum(x['archive_basename_present'] for x in rows),ownership_sha256=hashlib.sha256((A/'nine-way-ownership.csv').read_bytes()).hexdigest().upper(),scope='Per-key frozen edition identity and local build leads; no relevance/behavior completion claim'),indent=2))
