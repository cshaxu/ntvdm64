"""Check pending physical headers against actually recorded worker header bytes."""
import collections,hashlib,json,pathlib,subprocess
out=pathlib.Path('artifacts/mvdm-audit/host-ch-scope'); snapshot=out/'ccpu-cvidc-pending-header-evidence.json';data=json.loads(snapshot.read_text(encoding='utf-8-sig'));tree=pathlib.Path('build/.ninja/msvc-x86-release/worker');d=subprocess.check_output(['ninja','-C',str(tree),'-t','deps']);index=collections.defaultdict(set);stale=0;valid=False
for line in d.decode('utf8',errors='replace').replace('\\','/').splitlines():
 if line and not line[0].isspace():valid='(VALID)' in line;stale+=('(STALE)' in line);continue
 if valid:
  p=tree/line.strip()
  if p.suffix.lower()=='.h' and p.is_file():index[hashlib.sha256(p.read_bytes()).hexdigest().upper()].add(p.resolve().as_posix())
rows=[]
for x in data['rows']:
 p=pathlib.Path('src/mvdm')/x['path'];digest=hashlib.sha256(p.read_bytes()).hexdigest().upper();same=sorted(v for v in index[digest] if v.casefold()!=p.resolve().as_posix().casefold());rows.append({'path':x['path'],'sha256':digest,'different_selected_same_bytes':same})
meta={'input_sha256':hashlib.sha256(snapshot.read_bytes()).hexdigest().upper(),'deps_sha256':hashlib.sha256(d).hexdigest().upper(),'graph_sha256':hashlib.sha256((tree/'build.ninja').read_bytes()).hexdigest().upper(),'stale_blocks':stale,'paths':len(rows),'selected_distinct_duplicate_paths':sum(bool(x['different_selected_same_bytes']) for x in rows),'rows':rows,'limit':'Exact bytes only among existing VALID worker dependency headers; a dependency may be an inactive include; no semantic equivalence, generated source lineage, alternate tree or compile-time state is established.'}
(out/'ccpu-cvidc-selected-duplicate-evidence.json').write_text(json.dumps(meta,indent=2)+'\n',encoding='utf8');print('paths',len(rows),'distinct duplicate paths',meta['selected_distinct_duplicate_paths'],'stale',stale)
