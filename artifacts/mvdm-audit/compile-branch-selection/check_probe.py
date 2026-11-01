import importlib.util, json, pathlib, re, subprocess, hashlib
p=pathlib.Path('build/t473-s13/branch-inventory/audit.py')
s=importlib.util.spec_from_file_location('audit',p); a=importlib.util.module_from_spec(s);s.loader.exec_module(a)
contexts=json.load(open(a.OUT/'equivalence-all.json',encoding='utf-8'))
chosen=[x for x in contexts if not x['equal']]
results=[]
for item in chosen:
    arch=item['arch']; env=a.env_for(arch)
    cl=a.shutil.which('cl.exe',path=env.get('PATH',env.get('Path','')))
    db=json.loads(subprocess.run([str(a.NINJA),'-C',item['directory'],'-t','compdb'],capture_output=True,text=True,check=True).stdout)
    entry=next(x for x in db if x.get('file')==item['file'] and re.search(r'(?i)\bcl(?:\.exe)?\s',x.get('command','')))
    command=re.split(r'(?i)\bcl(?:\.exe)?\s+',entry['command'],maxsplit=1)[1]
    command=re.sub(r'(?i)(?<!\S)/Fo(?:"[^"]*"|\S+)','',command)
    command=re.sub(r'(?i)(?<!\S)/(?:c|showIncludes)(?=\s|$)','',command)
    shadow=re.sub(re.escape(str(a.ROOT).replace('\\','/')+'/src/'),str(a.SHADOW).replace('\\','/')+'/',command,flags=re.I)
    shadow=re.sub(re.escape(str(a.ROOT).replace('\\','/')+'/src"'),str(a.SHADOW).replace('\\','/')+'"',shadow,flags=re.I)
    outs=[]; exits=[]
    for cmd in (command,shadow):
        proc=subprocess.run('"'+cl+'" /EP '+cmd,cwd=item['directory'],env=env,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        exits.append(proc.returncode)
        lines=[line.strip() for line in proc.stdout.splitlines() if line.strip() and b'BRANCH_MARK_' not in line and b'FILE_MARK_' not in line]
        outs.append(b'\n'.join(lines))
    results.append({'arch':arch,'file':item['file'],'directory':item['directory'],'exits':exits,'equal':outs[0]==outs[1], 'sha256':[hashlib.sha256(v).hexdigest() for v in outs]})
print(json.dumps(results,indent=2));(a.OUT/'probe-equivalence.json').write_text(json.dumps(results,indent=2))
