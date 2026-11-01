from pathlib import Path
import re,subprocess,shlex,json,concurrent.futures,sys,collections
root=Path.cwd();arch=sys.argv[1];targets={'x86':[('worker','ntvdm.exe'),('wow32','wow32.dll')],'x64':[('worker','ntvwm.exe'),('frontend','ntcon.exe'),('monitor','ntmon.exe'),('launcher','run16.exe'),('service','ntsrv.exe'),('hook','nthook64.dll')]}[arch];allcmd=[]
for graph,target in targets:
 d=root/'build/.ninja'/('msvc-'+arch+'-release')/graph
 output=subprocess.check_output(['ninja','-C',str(d),'-t','commands',target],text=True,errors='replace')
 for line in output.splitlines():
  if 'cl.exe ' not in line.lower():continue
  m=re.search(r'("[^"]+\.(?:c|cpp)"|\S+\.(?:c|cpp))\s*$',line,re.I)
  if not m:continue
  src=m[1].strip('"').replace('\\','/').lower()
  if '/src/mvdm-host/' in src or '/src/non-mvdm/' in src or '/detours/' in src or '/worker/generated/base_vdm_config.c' in src:continue
  line=line[line.lower().find('cl.exe '):];args=[x.replace('"','') for x in shlex.split(line,posix=False)];args=[x for x in args if not (x.lower() in ('/c','/showincludes') or x.lower().startswith('/fo'))];args[1:1]=['/E','/d1PP'];allcmd.append((graph,str(d),src,args))
linepat=re.compile(r'^\s*#line\s+\d+\s+"([^"]+)"');defpat=re.compile(r'^\s*#\s*define\s+([A-Za-z_]\w*)');undefpat=re.compile(r'^\s*#\s*undef\s+([A-Za-z_]\w*)')
def process(item):
 graph,d,source,args=item;p=subprocess.run(args,cwd=d,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
 if p.returncode:return {'graph':graph,'source':source,'code':p.returncode,'error':p.stderr.decode(errors='replace')[-800:]}
 active={};observed={};current='';visits=0
 for line in p.stdout.decode('utf8',errors='replace').splitlines():
  m=linepat.match(line)
  if m:
   current=m[1].replace('\\\\','/').replace('\\','/').lower()
   if ('/src/' in current or current.startswith('obj/')) and not any(x in current for x in ('/src/mvdm-host/','/src/mvdm-guest/','/src/non-mvdm/','/detours/')):
    visits+=1
    for name,owner in active.items():
     if owner:observed.setdefault(name,set()).add(owner)
   continue
  m=defpat.match(line)
  if m:
   active[m[1]]=current if ('/src/mvdm-host/' in current or '/src/non-mvdm/' in current) else ''
   continue
  m=undefpat.match(line)
  if m:active.pop(m[1],None)
 return {'graph':graph,'source':source,'code':0,'visits':visits,'macros':{k:sorted(v) for k,v in observed.items()}}
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:rows=list(pool.map(process,allcmd))
fails=[{k:v for k,v in x.items() if k!='macros'} for x in rows if x['code']];mac=collections.defaultdict(lambda:{'units':set(),'providers':set()})
for u in rows:
 for k,owners in u.get('macros',{}).items():mac[k]['units'].add(u['source']);mac[k]['providers'].update(owners)
res={'architecture':arch,'commands':len(rows),'unique_sources':len({u['source'] for u in rows}),'failures':fails,'macro_count':len(mac),'macro_names':{k:{'units':len(v['units']),'providers':sorted(v['providers'])} for k,v in mac.items()},'unit_results':rows}
(root/f'build/t473-s12-flat/macro-live-{arch}.json').write_text(json.dumps(res,indent=2));print(arch,'commands',len(rows),'unique',res['unique_sources'],'failures',len(fails),'macros',len(mac));print('failure examples',fails[:3]);raise SystemExit(bool(fails) or not rows)
