from pathlib import Path
import csv,json,hashlib,subprocess,os,re,collections
root=Path('artifacts/mvdm-audit/host-ch-scope'); src=Path('src/mvdm'); ninja=r'C:\Users\neko\AppData\Local\Microsoft\WinGet\Packages\Ninja-build.Ninja_Microsoft.Winget.Source_8wekyb3d8bbwe\ninja.exe'
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest().upper()
inv=[x for x in csv.DictReader((root/'inventory.csv').open(encoding='utf8',newline='')) if x['scope']=='guest-target-observed' and x['path'].endswith(('.c','.h'))]; keys={x['path'].casefold() for x in inv}; receipts=collections.defaultdict(list)
for name in ('guest-target-candidate','guest-target-followup','guest-target-round3','dos-v86-target-current'):
 meta=json.loads((root/(name+'-evidence.json')).read_text()); rh={r['recipe']:r['sha256'].upper() for r in (meta.get('recipes',[]) if isinstance(meta.get('recipes'),list) else [])}
 for r in csv.DictReader((root/(name+'-inventory.csv')).open(encoding='utf8',newline='')):
  key=r['path'].casefold();isguest=r.get('group')=='guest-source-observed-profile' or r.get('candidate')=='True'
  if key not in keys or not isguest:continue
  recipe=r.get('makefile') or r.get('recipe'); h=r.get('recipe_sha256') or rh.get(recipe,''); f=Path(recipe) if recipe else None
  receipts[key].append({'name':name,'target':r.get('guest_target') or r.get('target'),'recipe':recipe,'recipe_valid':bool(f and f.is_file() and h and sha(f)==h.upper()),'file_valid':sha(src/r['path'])==(r.get('source_sha256') or r.get('file_sha256') or '').upper()})
trees=('msvc-x86-release/worker','msvc-x86-release/wow32','msvc-x64-release/worker','pkg-uuid-x86/worker');deps=set();graphs=[];graphtext=[]
for tree in trees:
 p=Path('build/.ninja')/tree;g=(p/'build.ninja').read_bytes();d=subprocess.check_output([ninja,'-C',str(p),'-t','deps']); txt=d.decode('utf8',errors='replace'); stale=sum('(STALE)' in l for l in txt.splitlines() if '#deps ' in l)
 for line in txt.splitlines():
  if line and line[0].isspace():deps.add(os.path.normcase(os.path.abspath(os.path.join(p,line.strip()))))
 graphs.append({'tree':tree,'graph_sha256':hashlib.sha256(g).hexdigest().upper(),'deps_sha256':hashlib.sha256(d).hexdigest().upper(),'stale_blocks':stale});graphtext.append((tree,g.decode('utf8',errors='replace').replace('\\','/').casefold()))
refs=collections.defaultdict(list)
for folder in ('src','tools'):
 for f in Path(folder).rglob('*'):
  if not f.is_file() or f.suffix.lower() not in ('.c','.h'):continue
  norm=f.as_posix().casefold()
  if norm.startswith(('src/mvdm/wow16/','src/mvdm/dos/v86/')):continue
  try:ls=f.read_text(encoding='latin1').splitlines()
  except OSError:continue
  for no,line in enumerate(ls,1):
   m=re.match(r'^\s*#\s*include\s*([<"])([^>"]+)[>"]',line)
   if m:refs[Path(m.group(2).replace('\\','/')).name.casefold()].append({'source':f.as_posix(),'line':no,'text':line.strip(),'resolve':os.path.normcase(os.path.abspath(f.parent/m.group(2).replace('\\','/'))) if m.group(1)=='"' else ''})
result=[]
for row in inv:
 rel=row['path']; p=src/rel; isdep=os.path.normcase(os.path.abspath(p)) in deps;mentions=[t for t,g in graphtext if 'src/mvdm/'+rel.casefold() in g];possible=refs[p.name.casefold()]; exact=[x for x in possible if x['resolve']==os.path.normcase(os.path.abspath(p))]; local32=[]
 for d in (p.parent,p.parent.parent):
  for nm in ('sources','makefile','makefile.dos','makefile.nt'):
   q=d/nm
   if not q.is_file():continue
   text=q.read_text(encoding='latin1',errors='replace'); win32=bool(re.search(r'(?i)(?:-DWIN32|/DWIN32|UMTYPE\s*=windows|TARGETTYPE\s*=DYNLINK)',text)); tok=bool(re.search(r'(?i)(?<![a-z0-9_])'+re.escape(p.stem)+r'\.(?:c|obj)(?![a-z0-9_])',text))
   if win32 and tok:local32.append(f'{q.as_posix()} [sha256 {sha(q)}]')
 rr=receipts[rel.casefold()];valid=any(x['file_valid'] and x['recipe_valid'] for x in rr)
 decision='keep-guest-observed-profile'
 if isdep or mentions or exact:decision='return-host-shared-physical'
 elif local32:decision='dual-target-candidate-needs-recipe-review'
 elif possible:decision='same-basename-host-include-needs-resolution'
 elif not valid or any(x['stale_blocks'] for x in graphs):decision='return-pending-stale-evidence'
 result.append({'path':rel,'sha256':sha(p),'inventory_scope':row['scope'],'decision':decision,'valid_receipt':valid,'guest_witnesses':rr,'host_exact_dependency':isdep,'host_exact_graph_mentions':mentions,'host_local_exact_includes':exact,'host_basename_includes':possible,'historical_win32_recipe_token':local32})
out=Path('artifacts/mvdm-audit/s10-guest-observed-counterexamples.json');out.write_text(json.dumps({'graphs':graphs,'counts':dict(collections.Counter(x['decision'] for x in result)),'rows':result,'limits':'Captured graphs only; same-basename include is not proof of same physical source; lexical recipe match can be alternate target. No guest build or execution.'},indent=2)+'\n',encoding='utf8')
with Path('artifacts/mvdm-audit/s10-guest-observed-counterexamples.csv').open('w',encoding='utf8',newline='') as f:
 fields=['path','sha256','decision','valid_receipt','host_exact_dependency','host_exact_graph_mentions','host_local_exact_includes','host_basename_includes','historical_win32_recipe_token','guest_witnesses'];w=csv.DictWriter(f,fieldnames=fields);w.writeheader()
 for x in result:w.writerow({k:json.dumps(x[k],ensure_ascii=False) if isinstance(x[k],(dict,list)) else x[k] for k in fields})
print('rows',len(result),'decisions',collections.Counter(x['decision'] for x in result));print('graphs stale',[(g['tree'],g['stale_blocks']) for g in graphs])
