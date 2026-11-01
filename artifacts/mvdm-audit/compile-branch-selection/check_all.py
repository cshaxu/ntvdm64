import concurrent.futures, hashlib, importlib.util, json, pathlib, re, subprocess
spec=importlib.util.spec_from_file_location('audit','build/t473-s13/branch-inventory/audit.py');a=importlib.util.module_from_spec(spec);spec.loader.exec_module(a)
dirs={'x86':['worker','wow32'],'x64':['frontend','hook','launcher','monitor','service','worker']}
envs={k:a.env_for(k) for k in dirs};cls={k:a.shutil.which('cl.exe',path=envs[k].get('PATH',envs[k].get('Path',''))) for k in dirs}
jobs=[]
for arch,names in dirs.items():
 for name in names:
  folder=a.ROOT/f'build/.ninja/msvc-{arch}-release'/name
  db=json.loads(subprocess.run([str(a.NINJA),'-C',str(folder),'-t','compdb'],capture_output=True,text=True,check=True).stdout)
  for x in db:
   source=x.get('file','');cmd=x.get('command','')
   if not re.search(r'(?i)\bcl(?:\.exe)?\s',cmd) or not source.lower().endswith(('.c','.cpp')) or not pathlib.Path(source).is_file():continue
   if not source.lower().replace('\\','/').startswith(str(a.ROOT).lower().replace('\\','/')+'/src/'):continue
   if '/mvdm-guest/' in source.lower().replace('\\','/'):continue
   jobs.append((arch,x))
def run(job):
 arch,x=job; command=re.split(r'(?i)\bcl(?:\.exe)?\s+',x['command'],maxsplit=1)[1]
 command=re.sub(r'(?i)(?<!\S)/Fo(?:"[^"]*"|\S+)','',command);command=re.sub(r'(?i)(?<!\S)/(?:c|showIncludes)(?=\s|$)','',command)
 shadow=re.sub(re.escape(str(a.ROOT).replace('\\','/')+'/src/'),str(a.SHADOW).replace('\\','/')+'/',command,flags=re.I)
 shadow=re.sub(re.escape(str(a.ROOT).replace('\\','/')+'/src"'),str(a.SHADOW).replace('\\','/')+'"',shadow,flags=re.I)
 hashes=[];exits=[]
 for cmd in (command,shadow):
  p=subprocess.run('"'+cls[arch]+'" /EP '+cmd,cwd=x['directory'],env=envs[arch],stdout=subprocess.PIPE,stderr=subprocess.PIPE);exits.append(p.returncode)
  normalized=b'\n'.join(line.strip() for line in p.stdout.splitlines() if line.strip() and b'BRANCH_MARK_' not in line and b'FILE_MARK_' not in line)
  hashes.append(hashlib.sha256(normalized).hexdigest())
 return {'arch':arch,'file':x['file'],'directory':x['directory'],'exits':exits,'equal':exits==[0,0] and hashes[0]==hashes[1],'sha256':hashes}
print('Comparing',len(jobs),'contexts',flush=True);results=[]
with (a.OUT/'equivalence-progress.jsonl').open('w',encoding='utf8') as f,concurrent.futures.ThreadPoolExecutor(max_workers=8) as pool:
 for i,r in enumerate(pool.map(run,jobs),1):
  results.append(r);f.write(json.dumps(r)+'\n');f.flush()
  if i%100==0:print(i,'/',len(jobs),'unequal',sum(not x['equal'] for x in results),flush=True)
(a.OUT/'equivalence-all.json').write_text(json.dumps(results),encoding='utf8')
print('finished',len(results),'unequal',sum(not x['equal'] for x in results),flush=True)
