import importlib.util,json,pathlib,re,subprocess,difflib,sys
s=importlib.util.spec_from_file_location('a','build/t473-s13/branch-inventory/audit.py');a=importlib.util.module_from_spec(s);s.loader.exec_module(a)
arch='x86';env=a.env_for(arch);cl=a.shutil.which('cl.exe',path=env.get('PATH',env.get('Path','')));folder=a.ROOT/'build/.ninja/msvc-x86-release/worker'
db=json.loads(subprocess.run([str(a.NINJA),'-C',str(folder),'-t','compdb'],capture_output=True,text=True,check=True).stdout)
suffix=sys.argv[1] if len(sys.argv)>1 else '/ccpu386/c_reg.c';x=next(x for x in db if x.get('file','').endswith(suffix))
command=re.split(r'(?i)\bcl(?:\.exe)?\s+',x['command'],maxsplit=1)[1];command=re.sub(r'(?i)(?<!\S)/Fo(?:"[^"]*"|\S+)','',command);command=re.sub(r'(?i)(?<!\S)/(?:c|showIncludes)(?=\s|$)','',command)
shadow=re.sub(re.escape(str(a.ROOT).replace('\\','/')+'/src/'),str(a.SHADOW).replace('\\','/')+'/',command,flags=re.I);shadow=re.sub(re.escape(str(a.ROOT).replace('\\','/')+'/src"'),str(a.SHADOW).replace('\\','/')+'"',shadow,flags=re.I)
outs=[]
for cmd in (command,shadow):
 p=subprocess.run('"'+cl+'" /EP '+cmd,cwd=x['directory'],env=env,stdout=subprocess.PIPE,stderr=subprocess.PIPE);print('exit',p.returncode)
 outs.append([l.strip() for l in p.stdout.decode('mbcs',errors='replace').splitlines() if l.strip() and 'BRANCH_MARK_' not in l and 'FILE_MARK_' not in l])
diff=list(difflib.unified_diff(*outs,n=2));(a.OUT/'c-reg-diff.txt').write_text('\n'.join(diff),encoding='utf8');print('\n'.join(diff[:80]))
