import hashlib,json,re,subprocess
from pathlib import Path
header=Path('src/mvdm/inc/monregs.h')
source=Path('src/mvdm/softpc.new/host/src/nt_thred.c')
obj=Path('build/.ninja/msvc-x86-release/worker/obj/host/nt_thred.obj')
objdump=Path(r'C:\Users\neko\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.MSVCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw32\bin\objdump.exe')
names=set(re.findall(r'\b(?:extern\s+\w+\s+)([gs]et\w+)\s*\(',header.read_text()))
tokens=set(re.findall(r'\b(?:get|set)\w+\b',source.read_text()))
symbols=subprocess.check_output([str(objdump),'-t',str(obj)],text=True)
undefined=[]
for line in symbols.splitlines():
    if re.search(r'\(sec\s+0\)',line): undefined.append(line.split()[-1])
uses=[x for x in undefined if x.lstrip('_').split('@')[0] in names]
deps=subprocess.check_output(['ninja','-C',str(obj.parents[2]),'-t','deps','obj/host/nt_thred.obj'],text=True)
r={'inputs':[{'path':str(x).replace('\\','/'),'sha256':hashlib.sha256(x.read_bytes()).hexdigest().upper()} for x in (header,source,obj)],
   'header_function_declarations':len(names),'matching_source_tokens':sorted(names&tokens),
   'matching_undefined_object_symbols':uses,'all_undefined_object_symbols':undefined,
   'dependency_lines':[x for x in deps.splitlines() if 'nt_thred.obj' in x or 'monregs.h' in x],
   'scope':'nt_thred object only; inline, types, preprocessor effects, alternate consumers/profiles require separate review'}
Path('artifacts/mvdm-audit/host-inc/monregs-selected-object-evidence.json').write_text(json.dumps(r,indent=2)+'\n',encoding='utf-8')
print(json.dumps(r,indent=2))
