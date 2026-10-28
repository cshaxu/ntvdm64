"""Bounded role census: no guest compilation, no shared-ledger changes."""
import collections,csv,hashlib,io,json,os,re,subprocess
from pathlib import Path,PurePosixPath
root=Path('src/mvdm'); original=Path('O:/repos.external/OpenNT/base/mvdm')
inventory=Path('artifacts/mvdm-audit/host-ch-scope/inventory.csv'); raw=inventory.read_bytes()
allrows=list(csv.DictReader(io.StringIO(raw.decode('utf-8-sig'),newline='')))
rows=[r for r in allrows if r['scope']=='role-pending']
host=[r for r in allrows if r['scope']=='host-candidate']
hashfile=lambda p:hashlib.sha256(p.read_bytes()).hexdigest().upper()
# Host C includes of same basenames require per-path resolution.
names={PurePosixPath(r['path']).name.casefold() for r in rows}
includes=collections.defaultdict(list)
for r in host:
 p=root/r['path']
 if not p.is_file():continue
 for no,line in enumerate(p.read_text(encoding='latin1').splitlines(),1):
  m=re.match(r'\s*#\s*include\s*[<"]([^>"]+)[>"]',line)
  if m and PurePosixPath(m.group(1).replace('\\','/')).name.casefold() in names:
   includes[PurePosixPath(m.group(1).replace('\\','/')).name.casefold()].append(f"{r['path']}:{no}")
# Select three explicit historical Win16 guest targets; object lists, not any
# loose filename mention, determine membership. Paths only at target root.
targets={'wow16/write/':('write.exe',('OBJ1','OBJ2','OBJ3','OBJ4','OBJ5','OBJ6')),
         'wow16/ddeml/':('ddeml.dll',('OBJ1','OBJ2')),
         'wow16/commdlg/':('commdlg.dll',())}
recipes={}
for prefix,(target,variables) in targets.items():
 p=root/prefix/'makefile';ref=original/prefix/'makefile';text=p.read_text(encoding='latin1')
 assert hashfile(p)==hashfile(ref),'historical recipe mismatch '+str(p)
 joined=re.sub(r'\\[ \t]*\r?\n[ \t]*',' ',text)
 if variables:
  chunks=[]
  for v in variables:
   m=re.search(r'(?im)^\s*'+v+r'\s*=\s*(.*)$',joined)
   if not m:raise ValueError('missing explicit object list '+prefix+v)
   chunks.append(m.group(1))
  objs=set(x.casefold() for x in re.findall(r'([A-Za-z0-9_]+)\.obj\b',' '.join(chunks),re.I))
 else:
  m=re.search(r'(?im)^\$\(BLD\)\\commdlg\.dll\s*:\s*(.*)$',joined)
  if not m:raise ValueError('missing commdlg target')
  objs=set(x.casefold() for x in re.findall(r'([A-Za-z0-9_]+)\.obj\b',m.group(1),re.I))
 recipes[prefix]={'target':target,'recipe':str(p).replace('\\','/'),'sha256':hashfile(p),'objects':objs}
trees=('msvc-x86-release/worker','msvc-x86-release/wow32','msvc-x64-release/worker','pkg-uuid-x86/worker')
graphs=[]; exact=set(); graph_texts=[]
for tree in trees:
 p=Path('build/.ninja')/tree;g=p/'build.ninja';graph=g.read_bytes();run=subprocess.check_output(['ninja','-C',str(p),'-t','deps'])
 graph_texts.append((tree,graph.decode('utf-8',errors='replace').replace('\\','/').casefold()))
 text=run.decode('utf-8',errors='replace');stale=sum('(STALE)' in line for line in text.splitlines() if '#deps ' in line)
 for line in text.splitlines():
  if line and line[0].isspace():exact.add(os.path.normcase(os.path.abspath(os.path.join(p,line.strip()))))
 graphs.append({'tree':tree,'graph_sha256':hashlib.sha256(graph).hexdigest().upper(),'deps_sha256':hashlib.sha256(run).hexdigest().upper(),'stale_blocks':stale})
results=[]
for r in rows:
 rel=r['path'];p=root/rel;name=PurePosixPath(rel).name.casefold();prefix=next((x for x in targets if rel.lower().startswith(x) and len(PurePosixPath(rel).parts)==3),None)
 item={'path':rel,'status':r['ledger_status'],'group':'unresolved','guest_target':'','source_sha256':hashfile(p) if p.is_file() else '',
       'makefile':'','host_same_name_includes':'; '.join(includes[name]),'host_exact_recorded_dep':os.path.normcase(os.path.abspath(p)) in exact,
       'host_graph_mentions':'; '.join(t for t,g in graph_texts if ('src/mvdm/'+rel.casefold()) in g),
       'reason':'Header, different guest recipe/profile, host consumer or source identity requires separate review.'}
 if prefix and p.suffix.casefold()=='.c':
  rec=recipes[prefix];ref=original/rel;other=p.with_suffix('.asm')
  if p.stem.casefold() in rec['objects'] and ref.is_file() and hashfile(p)==hashfile(ref) and not other.exists() and not includes[name] and not item['host_exact_recorded_dep'] and not item['host_graph_mentions']:
   item.update(group='guest-source-observed-profile',guest_target=rec['target'],makefile=rec['recipe'],reason='Original 16-bit object in target recipe; same source bytes; no host lexical same-name C include, exact captured host dependency or graph source mention; alternate/generated profiles remain unproved.')
 results.append(item)
results.sort(key=lambda x:x['path'].casefold())
out=Path('artifacts/mvdm-audit/host-ch-scope');dest=out/'guest-target-candidate-inventory.csv'
with dest.open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=list(results[0]));w.writeheader();w.writerows(results)
meta={'inventory_sha256':hashlib.sha256(raw).hexdigest().upper(),'graphs':graphs,
 'recipes':[{'prefix':k,'target':v['target'],'recipe':v['recipe'],'sha256':v['sha256'],'object_names':len(v['objects'])} for k,v in recipes.items()],
 'total':len(results),'groups':dict(collections.Counter(x['group'] for x in results)),
 'guest_targets':dict(collections.Counter(x['guest_target'] for x in results if x['guest_target'])),
 'limits':'Source/recipe and observed dependency graph only; no guest source build, guest image identity or runtime acceptance; no exclusion of the corresponding application functionality.'}
(out/'guest-target-candidate-evidence.json').write_text(json.dumps(meta,indent=2)+'\n',encoding='utf-8')
print(json.dumps(meta,indent=2))
