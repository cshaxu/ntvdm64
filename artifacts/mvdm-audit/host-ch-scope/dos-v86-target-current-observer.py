"""Observe historical DOS MEM/SETVER target membership without building guest."""
import csv,hashlib,json,os,re,subprocess,collections
from pathlib import Path,PurePosixPath
out=Path('artifacts/mvdm-audit/host-ch-scope'); root=Path('src/mvdm'); original=Path('O:/repos.external/OpenNT/base/mvdm')
h=lambda p:hashlib.sha256(p.read_bytes()).hexdigest().upper()
base=out/'guest-target-round3-inventory.csv'; head=out/'guest-header-round4-inventory.csv'
rows=list(csv.DictReader(base.open(encoding='utf-8'))); done={r['path'].casefold() for r in csv.DictReader(head.open(encoding='utf-8')) if r['candidate']=='True'}
pending={r['path'].casefold():r['path'] for r in rows if r['group']=='unresolved' and r['path'].casefold() not in done}
candidates={}; recipes={}
for folder,target in [('dos/v86/cmd/mem','mem.exe'),('dos/v86/cmd/setver','setver.exe')]:
 make=root/folder/'makefile'; ref=original/folder/'makefile';assert h(make)==h(ref)
 text=re.sub(r'\\[ \t]*\r?\n[ \t]*',' ',make.read_text(encoding='latin1')); assert re.search(r'(?im)^\s*dest\s*=\s*'+re.escape(target)+r'\s*$',text)
 match=re.search(r'(?im)^\s*\$\(dest\)\s*:\s*(.*)$',text);assert match
 objs=set(x.casefold() for x in re.findall(r'([a-z0-9_]+)\.obj\b',match.group(1),re.I));recipes[folder]={'target':target,'sha256':h(make),'objects':sorted(objs)}
 for line in text.splitlines():
  m=re.match(r'^\s*([\w]+)\.obj\s*:\s*(.*)$',line,re.I)
  if not m or m.group(1).casefold() not in objs:continue
  stem=m.group(1).casefold()
  for token in m.group(2).split():
   if not re.fullmatch(r'[\w.-]+\.(?:c|h)',token,re.I):continue
   rel=f'{folder}/{token}'; key=rel.casefold();p=root/rel; ref=original/rel
   if key not in pending or not p.is_file() or not ref.is_file():continue
   if token.lower().endswith('.c') and Path(token).stem.casefold()!=stem:continue
   candidates.setdefault(key,{'path':pending[key],'target':target,'recipe':make.as_posix(),'recipe_sha256':h(make),'file_sha256':h(p),'same_as_original':h(p)==h(ref),'recipe_objects':[]})['recipe_objects'].append(stem+'.obj')
folder='dos/v86/cmd/setver'; rel=folder+'/usa-ms/message.h'; key=rel.casefold(); make=root/folder/'makefile'; text=make.read_text(encoding='latin1'); source=root/rel; generated=root/folder/'message.h'; ref=original/rel
assert key in pending and ref.is_file() and source.read_bytes()==generated.read_bytes() and source.read_bytes()==ref.read_bytes()
assert re.search(r'(?im)^message\.h\s*:\s*\$\(COUNTRY\)\\message\.h',text)
assert re.search(r'(?im)^\s*copy\s+\$\(COUNTRY\)\\message\.h\s+\.',text)
candidates[key]={'path':pending[key],'target':'setver.exe','recipe':make.as_posix(),'recipe_sha256':h(make),'file_sha256':h(source),'same_as_original':True,'recipe_objects':['usa-ms country copy -> message.h -> setver.obj']}
names={PurePosixPath(k).name for k in candidates}; inc=collections.defaultdict(list)
for name in sorted(subprocess.check_output(['rg','--files','src','tools','-g','*.c','-g','*.h']).decode().splitlines(),key=str.casefold):
 p=Path(name); n=p.as_posix().casefold()
 if n.startswith(('src/mvdm/wow16/','src/mvdm/dos/v86/')):continue
 for no,line in enumerate(p.read_text(encoding='latin1').splitlines(),1):
  m=re.match(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]',line)
  if m:
   v=PurePosixPath(m.group(1).replace('\\','/')).name.casefold()
   if v in names:inc[v].append(f'{p.as_posix()}:{no}')
exact=set();graphs=[];texts=[]
for tree in ('msvc-x86-release/worker','msvc-x86-release/wow32','msvc-x64-release/worker','pkg-uuid-x86/worker'):
 p=Path('build/.ninja')/tree;g=(p/'build.ninja').read_bytes();d=subprocess.check_output(['ninja','-C',str(p),'-t','deps']);stale=sum('(STALE)' in x for x in d.decode('utf8',errors='replace').splitlines() if '#deps ' in x);assert stale==0
 for line in d.decode('utf8',errors='replace').splitlines():
  if line and line[0].isspace():exact.add(os.path.normcase(os.path.abspath(os.path.join(p,line.strip()))))
 texts.append((tree,g.decode('utf8',errors='replace').replace('\\','/').casefold()));graphs.append({'tree':tree,'graph_sha256':hashlib.sha256(g).hexdigest().upper(),'deps_sha256':hashlib.sha256(d).hexdigest().upper(),'stale_blocks':stale})
results=[]
for key,item in sorted(candidates.items()):
 p=root/item['path'];item['host_same_basename_includes']=inc[PurePosixPath(key).name];item['host_exact_recorded_dep']=os.path.normcase(os.path.abspath(p)) in exact;item['host_graph_mentions']=[t for t,g in texts if 'src/mvdm/'+key in g];item['candidate']=item['same_as_original'] and not item['host_same_basename_includes'] and not item['host_exact_recorded_dep'] and not item['host_graph_mentions'];results.append(item)
meta={'input_inventory_sha256':h(base),'input_header_sha256':h(head),'pending_count':len(pending),'recipes':recipes,'graphs':graphs,'results':results,'witness_count':len(results),'candidates':sum(x['candidate'] for x in results),'remaining':len(pending)-sum(x['candidate'] for x in results),'limitations':'Historical DOS link16 target and listed object prerequisites, not evidence of guest runtime, image provenance, all build profiles or unscanned consumers. Same-basename lexical scan is conservative; conditional compilation is not evaluated.'}
(out/'dos-v86-target-current-evidence.json').write_text(json.dumps(meta,indent=2)+'\n',encoding='utf8')
with (out/'dos-v86-target-current-inventory.csv').open('w',encoding='utf8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=list(results[0]));w.writeheader()
 for x in results:w.writerow({**x,'recipe_objects':'; '.join(x['recipe_objects']),'host_same_basename_includes':'; '.join(x['host_same_basename_includes']),'host_graph_mentions':'; '.join(x['host_graph_mentions'])})
print('witness',len(results),'candidate',meta['candidates'],'remaining',meta['remaining'])
