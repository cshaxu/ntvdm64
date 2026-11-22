"""Task-local lexical inventory. Not a build input or reachability proof."""
from pathlib import Path
import csv, hashlib, json, re, subprocess

root = Path(__file__).resolve().parents[3]
out = root / 'artifacts/build-migration/t484'
out.mkdir(parents=True, exist_ok=True)
files = subprocess.check_output(['git','ls-files','-z'], cwd=root).decode().split('\0')
texts = {}
for name in files:
    if not (name.startswith(('tools/', 'test/', 'src/addon/')) or name in ('CMakeLists.txt', 'CMakePresets.json')):
        continue
    if Path(name).suffix.lower() not in ('.cmd','.bat','.ps1','.py','.mjs','.js','.cmake','.json') and not name.endswith('CMakeLists.txt'):
        continue
    texts[name] = (root/name).read_text(encoding='utf-8-sig', errors='strict')
producer = re.compile(r'(?i)(?:\b(?:cl|link|lib|ml|nasm|wlink|rc|midl)(?:\.exe)\b|\bcmake\s+(?:--build|--preset)|\bninja(?:\.exe)?\b|GenerateCvidc|Stage-.*Release|Set-PackageIdentity)')
interpreter = re.compile(r'(?i)\b(?:pwsh|powershell(?:\.exe)?|python(?:3|\.exe)?|node(?:\.exe)?|cscript(?:\.exe)?|wscript(?:\.exe)?)\b')
rows, references, commands = [], [], []
for name, body in sorted(texts.items(), key=lambda x:x[0].lower()):
    if not (name.startswith('tools/build/') or name in ('CMakeLists.txt','CMakePresets.json') or producer.search(body)):
        continue
    basename = Path(name).name
    callers = []
    for caller, text in texts.items():
        if caller == name: continue
        for number, line in enumerate(text.splitlines(),1):
            if basename.lower() in line.lower():
                callers.append(caller)
                references.append(dict(target=name,caller=caller,line=number,text=line.strip()))
    hits = []
    for number,line in enumerate(body.splitlines(),1):
        if interpreter.search(line) or producer.search(line):
            hits.append(number)
            commands.append(dict(file=name,line=number,text=line.strip()))
    scope = ('host-configuration' if name in ('CMakeLists.txt','CMakePresets.json') else
             'addon' if name.startswith('src/addon/') else
             'build-directory' if name.startswith('tools/build/') else 'test-or-audit-compilation-candidate')
    rows.append(dict(file=name,scope=scope,bytes=(root/name).stat().st_size,
        sha256=hashlib.sha256((root/name).read_bytes()).hexdigest(),
        callers=';'.join(sorted(set(callers))),matching_lines=';'.join(map(str,hits)),
        review='lexical-candidate-not-reachability-proof'))
for filename,data in [('files.csv',rows),('references.csv',references),('commands.csv',commands)]:
    with (out/filename).open('w',encoding='utf-8',newline='') as f:
        writer=csv.DictWriter(f,fieldnames=list(data[0]),lineterminator='\n')
        writer.writeheader(); writer.writerows(data)
manifest=json.loads((root/'assets/release/addon-manifest.json').read_text(encoding='utf-8-sig'))
summary=dict(baseline=subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip(),
    inspected_script_or_metadata_files=len(texts),candidate_files=len(rows),
    lexical_reference_occurrences=len(references),candidate_command_lines=len(commands),
    addon_outputs=[dict(file=x['file'],source=x['source']) for group in ('drivers','utilities') for x in manifest[group]],
    limits='Lexical candidates include comments and inactive historical paths. Call-site review and generated command evidence remain required; zero literal references is not permission to delete.')
(out/'inventory.json').write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8',newline='\n')
print(json.dumps(summary,indent=2))
