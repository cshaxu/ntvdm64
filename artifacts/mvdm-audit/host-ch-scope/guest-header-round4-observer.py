"""Bounded direct guest-header evidence; never builds or edits guest."""
import collections,csv,hashlib,json,os,re,subprocess
from pathlib import Path,PurePosixPath
base=Path('src/mvdm'); original=Path('O:/repos.external/OpenNT/base/mvdm'); out=Path('artifacts/mvdm-audit/host-ch-scope'); source=out/'guest-target-round3-inventory.csv'
hashfile=lambda p:hashlib.sha256(p.read_bytes()).hexdigest().upper()
rows=list(csv.DictReader(source.open(encoding='utf-8',newline=''))); pending={r['path'].casefold():r for r in rows if r['group']=='unresolved'}
witness=collections.defaultdict(list)
for row in rows:
 if row['group']!='guest-source-observed-profile': continue
 p=base/row['path']; assert hashfile(p)==row['source_sha256']
 for no,line in enumerate(p.read_text(encoding='latin1').splitlines(),1):
  m=re.match(r'^\s*#\s*include\s*"([^"/\\]+)"',line)
  if m:
   key=(PurePosixPath(row['path']).parent/m.group(1)).as_posix().casefold()
   if key in pending: witness[key].append({'source':row['path'],'line':no,'target':row['guest_target'],'recipe':row['makefile'],'source_sha256':row['source_sha256']})
names={PurePosixPath(k).name for k in witness}; hostinc=collections.defaultdict(list)
# Scan all C/H outside original guest trees, including tools, not only inventory host candidates.
files=subprocess.check_output(['rg','--files','src','tools','-g','*.c','-g','*.h']).decode('utf-8').splitlines()
for name in sorted(files, key=str.casefold):
 p=Path(name); norm=p.as_posix().casefold()
 if norm.startswith(('src/mvdm/wow16/','src/mvdm/dos/v86/')):continue
 try: lines=p.read_text(encoding='latin1').splitlines()
 except OSError:continue
 for no,line in enumerate(lines,1):
  m=re.match(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]',line)
  if m:
   n=PurePosixPath(m.group(1).replace('\\','/')).name.casefold()
   if n in names:hostinc[n].append(f'{p.as_posix()}:{no}')
trees=('msvc-x86-release/worker','msvc-x86-release/wow32','msvc-x64-release/worker','pkg-uuid-x86/worker'); exact=set();graphs=[];texts=[]
for tree in trees:
 p=Path('build/.ninja')/tree; g=(p/'build.ninja').read_bytes(); deps=subprocess.check_output(['ninja','-C',str(p),'-t','deps']); text=deps.decode('utf-8',errors='replace'); stale=sum('(STALE)' in l for l in text.splitlines() if '#deps ' in l)
 assert stale==0,(tree,stale)
 for line in text.splitlines():
  if line and line[0].isspace():exact.add(os.path.normcase(os.path.abspath(os.path.join(p,line.strip()))))
 texts.append((tree,g.decode('utf-8',errors='replace').replace('\\','/').casefold()))
 graphs.append({'tree':tree,'graph_sha256':hashlib.sha256(g).hexdigest().upper(),'deps_sha256':hashlib.sha256(deps).hexdigest().upper(),'stale_blocks':stale})
result=[]
for rel, ws in sorted(witness.items()):
 row=pending[rel];path=row['path']; p=base/path;ref=original/path;basename=PurePosixPath(path).name.casefold();same=ref.is_file() and hashfile(p)==hashfile(ref); deps=os.path.normcase(os.path.abspath(p)) in exact; mentions=[t for t,g in texts if 'src/mvdm/'+path.casefold() in g]; inc=hostinc[basename]
 result.append({'path':path,'source_sha256':hashfile(p),'same_as_original':same,'guest_witnesses':ws,'host_same_basename_includes':inc,'host_exact_recorded_dep':deps,'host_graph_mentions':mentions,'candidate':same and not inc and not deps and not mentions})
meta={'input_sha256':hashfile(source),'trees':graphs,'scan_files':len(files),'unresolved_input':len(pending),'direct_header_witnesses':len(result),'candidates':sum(r['candidate'] for r in result),'remaining':len(pending)-sum(r['candidate'] for r in result),'results':result,'limits':'Lexical quoted local include in verified guest source; conditional includes not evaluated. Same-basename host scan is conservative and excludes original guest trees; alternate/generated configurations and indirect consumers remain unproven. This is a role candidate, not proof of runtime behavior or guest-image lineage.'}
(out/'guest-header-round4-evidence.json').write_text(json.dumps(meta,indent=2)+'\n',encoding='utf-8')
with (out/'guest-header-round4-inventory.csv').open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=['path','candidate','same_as_original','source_sha256','guest_witnesses','host_same_basename_includes','host_exact_recorded_dep','host_graph_mentions']);w.writeheader()
 for r in result:w.writerow({**r,'guest_witnesses':'; '.join(f"{x['source']}:{x['line']} ({x['target']})" for x in r['guest_witnesses']),'host_same_basename_includes':'; '.join(r['host_same_basename_includes']),'host_graph_mentions':'; '.join(r['host_graph_mentions'])})
print(json.dumps({k:v for k,v in meta.items() if k!='results'},indent=2))
