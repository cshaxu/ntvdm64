import csv,json,hashlib,subprocess,io
from pathlib import Path
rev='af4deca6f76f53ab31935edb6da0593daad8c13e';base='artifacts/mvdm-audit/';raw=subprocess.check_output(['git','show',rev+':'+base+'combined-initial-status.csv']);roots=[Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')];sha=lambda d:hashlib.sha256(d).hexdigest();rows=[]
source_ext={'.c','.h','.asm','.inc','.rc','.rcv','.def','.ico','.bmp','.cur','.dlg','.equ','.str','.ext','.skl','.res','.msg','.bin','.idx','.ver','.cl3','.cl4','.cl5'}
for r in csv.DictReader(io.StringIO(raw.decode('utf-8-sig'))):
 q=r['relative_path'];z=Path(q);ext=z.suffix.lower()
 if r['status']!='excluded' or not q.startswith(('dos/v86/','wow16/')):continue
 if '/tests/' in q or q.startswith(('dos/v86/tools/','wow16/test/hello/','wow16/wowdeb/')):role='test-or-build-tool'
 elif ext in source_ext:role='other-source-or-resource'
 elif ext in ('.obj','.map','.sym','.lib','.lnk','.lrf','.lst','.pdb','.rst','.tag','.tmp','.exc'):role='historical-artifact'
 elif 'makefil' in z.name.lower() or ext in ('.cmd','.bat'):role='build-recipe-or-script'
 else:role='other-manual-review'
 h=[]
 if role in ('test-or-build-tool','other-source-or-resource','other-manual-review'):
  v=subprocess.run(['git','show',rev+':src/mvdm/'+q],capture_output=True);h=[sha(v.stdout) if v.returncode==0 else None]+[sha((p/q).read_bytes()) if (p/q).is_file() else None for p in roots]
 rows.append(dict(path=q,shard=r['shard'],role=role,hashes=h))
print(json.dumps(dict(head=rev,aggregate_sha256=sha(raw),rows=rows,limits='Git revision snapshot for project files; external original hashes read current filesystem. Physical file roles are not exclusion of guest capabilities.'),ensure_ascii=True,indent=2))
