import csv,hashlib,os,re,subprocess
from pathlib import Path
from collections import defaultdict
base=Path('artifacts/mvdm-audit/shard-1'); repo=Path.cwd(); source=base/'file-audit.csv'; frozen=Path('artifacts/mvdm-audit/frozen-file-reconciliation.csv')
with source.open(encoding='utf-8',newline='') as f: rows=list(csv.DictReader(f)); fields=list(rows[0])
with frozen.open(encoding='utf-8-sig',newline='') as f: allrows=list(csv.DictReader(f))
products={r['relative_path'].lower():r['product_link_selection'] for r in allrows if r['product_link_selection']!='not-product-selected'}
selected=[r for r in rows if r['audit_status']=='待核验' and r['audit_disposition'] in ('declaration-or-build-carrier','tooling-or-build-carrier')]; assert len(selected)==259
by={r['relative_path'].lower():r for r in selected}; depend=defaultdict(list); direct=defaultdict(list); graphs=[]
for name in ('worker','wow32'):
 tree=repo/'build/.ninja/msvc-x86-release'/name; graph=tree/'build.ninja'; gh=hashlib.sha256(graph.read_bytes()).hexdigest().upper(); graphs.append((name,gh))
 buildmap={}
 for line in graph.read_text(encoding='latin1').splitlines():
  if not line.startswith('build '):continue
  match=re.match(r'build\s+([^\s|:]+).*?:\s+\S+\s+(.+)$',line)
  if not match:continue
  obj,inputs=match.groups(); norm=inputs.replace('$:',' :').replace(' :',':').replace('\\','/'); needle='/src/mvdm/'; i=norm.lower().find(needle)
  if i<0:continue
  rel=norm[i+len(needle):].split()[0].strip('"').lower()
  if rel in products and obj.lower().endswith('.obj'):buildmap[obj.lower()]=(rel,products[rel])
 proc=subprocess.run(['ninja','-C',str(tree),'-t','deps'],stdout=subprocess.PIPE,stderr=subprocess.PIPE,check=True); data=proc.stdout.decode('utf-8',errors='replace').splitlines(); current=None
 for line in data:
  if not line.startswith(' '):
   m=re.match(r'([^:]+): #deps \d+, deps mtime \d+ \(([^)]+)\)',line); obj=m.group(1).lower() if m else ''; current=(obj,m.group(2),buildmap[obj]) if m and obj in buildmap else None;continue
  if not current:continue
  raw=line.strip(); candidate=(tree/Path(raw)).resolve(); prefix=(repo/'src/mvdm').resolve()
  try: rel=candidate.relative_to(prefix).as_posix().lower()
  except ValueError:continue
  if rel in by:depend[rel].append({'graph':name,'object':current[0],'ninja_deps_status':current[1],'consumer':current[2][0],'consumer_product':current[2][1]})
 # Restrict literal include search to selected source files mapped by this graph.
 for rel,_kind in sorted(set(buildmap.values())):
  sp=repo/'src/mvdm'/rel
  if not sp.is_file() or sp.suffix.lower() not in ('.c','.h','.cpp','.cxx'):continue
  text=sp.read_bytes().decode('latin1'); includes=re.findall(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]',text,re.M)
  for inc in includes:
   tail=inc.replace('\\','/').lower().rsplit('/',1)[-1]
   for target in by:
    if target.rsplit('/',1)[-1]==tail: direct[target].append({'graph':name,'consumer':rel,'literal':inc})
out=[];edges=[]
for r in selected:
 rel=r['relative_path']; key=rel.lower(); p=repo/'src/mvdm'/rel; digest=hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''
 identity_status='matches-frozen' if digest==r['current_sha256'] else 'changed-after-freeze'
 if identity_status!='matches-frozen': print('post-freeze identity change:',rel)
 refs=depend[key]; literals=direct[key]; ext=p.suffix.lower(); istext=ext in ('.h','.inc','.md','.mac','.nt') or p.name.lower() in ('makefile','sources')
 text=p.read_bytes().decode('latin1') if istext and p.is_file() else ''; guards=[line.strip()[:120] for line in text.splitlines() if re.match(r'^\s*#\s*(if|ifdef|ifndef|elif)\b',line)][:5]
 role=('original declaration/include carrier' if ext in ('.h','.inc') else 'original build/tool carrier or retained historical asset; specific role requires review')
 if refs:
  summary=f'{len(refs)} selected-source compile dependency edges; first '+', '.join(x['consumer']+' ['+x['ninja_deps_status']+']' for x in refs[:3]); action='Trace declaration or build effect in each reached owner and targeted runtime behavior; dependency collection alone proves neither full semantic equivalence nor completed functionality.'
 elif literals:
  summary=f'No resolved selected-source Ninja dependency; {len(literals)} literal-include candidates require path/condition resolution';action='Resolve header path and preprocessing condition, then verify selected consumers and behavior; filename matches are not resolved includes.'
 else:
  summary='No selected-source compile dependency or literal include identified in the two current Ninja dependency databases';action='Determine guest, generated/resource, other product, conditional, or historical role before any classification; absence from these databases is not proof of exclusion.'
 marker='per-file carrier and selected dependency review: artifacts/mvdm-audit/shard-1/carrier-review.csv'
 if marker not in r['evidence']:r['evidence']+='; '+marker
 r['next_required_evidence']=action
 out.append({'relative_path':rel,'frozen_sha256':r['current_sha256'],'observed_current_sha256':digest,'identity_status':identity_status,'current_identity':r['current_identity'],'original_role':role,'selected_dependency_edges':len(refs),'dependency_summary':summary,'literal_include_candidates':len(literals),'literal_include_examples':'; '.join(x['consumer']+' -> '+x['literal'] for x in literals[:3]),'source_guard_examples':' | '.join(guards),'graph_context':'CPU_40_STYLE/CCPU/SPC386/NTVDM/PROD in worker; /showIncludes records selected product-source compilations; consult exact Ninja build.ninja per object','historical_evidence':'docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md; docs/etc/operations/policy/source-policy.md','remaining':action,'status':'pending-evidence'})
 for x in refs:edges.append({'carrier_path':rel,**x})
assert len(out)==259
for file,rs in ((base/'carrier-review.csv',out),(base/'carrier-dependency-edges.csv',edges)):
 with file.open('w',encoding='utf-8',newline='') as f:
  w=csv.DictWriter(f,fieldnames=list(rs[0]) if rs else ['carrier_path']);w.writeheader();w.writerows(rs)
tmp=base/'carrier-file.tmp'
with tmp.open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=fields);w.writeheader();w.writerows(rows)
os.replace(tmp,source)
print('rows',len(out),'with_deps',sum(bool(x['selected_dependency_edges']) for x in out),'dependency_edges',len(edges),'with_literal',sum(bool(x['literal_include_candidates']) for x in out));print('graphs',graphs)
