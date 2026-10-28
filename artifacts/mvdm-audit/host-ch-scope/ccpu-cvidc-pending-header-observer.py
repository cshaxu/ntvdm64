"""Read-only pending CCPU/C-VID header dependency observation."""
import csv,hashlib,json,pathlib,subprocess
root=pathlib.Path('artifacts/mvdm-audit');files=[('shard-0/review.csv','classification'),('shard-1/file-audit.csv','audit_status'),('shard-2/file-audit.csv','decision'),('shard-3/file-review.csv','review_class')];pending={};inputs=[]
for file,key in files:
 p=root/file;raw=p.read_bytes();inputs.append({'file':file,'sha256':hashlib.sha256(raw).hexdigest().upper()})
 for row in csv.DictReader(raw.decode('utf-8-sig').splitlines()):
  path=row['relative_path'].lower()
  if path.startswith(('softpc.new/base/ccpu386/','softpc.new/base/cvidc/')) and path.endswith('.h') and row[key] in ('pending','待核验'):
   if path in pending:raise ValueError('duplicate '+path)
   pending[path]={'path':row['relative_path'],'shard':file,'objects':[]}
tree=pathlib.Path('build/.ninja/msvc-x86-release/worker');deps=subprocess.check_output(['ninja','-C',str(tree),'-t','deps']);graph=tree/'build.ninja';obj=None;valid=False;stale=0
for line in deps.decode('utf-8',errors='replace').replace('\\','/').splitlines():
 if line and not line[0].isspace():
  obj=line.split(':',1)[0];valid='(VALID)' in line;stale+=('(STALE)' in line);continue
 if valid:
  norm=line.strip().lower()
  for path,entry in pending.items():
   if norm.endswith('/src/mvdm/'+path) or norm=='src/mvdm/'+path:entry['objects'].append(obj)
for x in pending.values():x['objects']=sorted(set(x['objects']))
result={'inputs':inputs,'graph_sha256':hashlib.sha256(graph.read_bytes()).hexdigest().upper(),'deps_sha256':hashlib.sha256(deps).hexdigest().upper(),'stale_dependency_blocks':stale,'pending_count':len(pending),'matched_count':sum(bool(x['objects']) for x in pending.values()),'rows':sorted(pending.values(),key=lambda x:x['path'].lower()),'limit':'One worker dependency cache; a VALID include proves neither emitted declarations nor active execution. Missing entries do not exclude unbuilt profiles or consumers.'}
out=root/'host-ch-scope/ccpu-cvidc-pending-header-evidence.json';out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf8');print('pending',result['pending_count'],'matched',result['matched_count'],'stale',stale)
