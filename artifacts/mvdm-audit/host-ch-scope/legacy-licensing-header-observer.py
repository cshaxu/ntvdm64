import hashlib, json, pathlib, re, subprocess

root = pathlib.Path('src/mvdm/softpc.new')
names = ['flexlmif.h', 'lm_attr.h', 'lm_clien.h']
headers = [root/'base/inc'/n for n in names]
graphs = {}
for name in ('worker','wow32'):
    p = pathlib.Path('build/.ninja/msvc-x86-release')/name
    result = subprocess.run(['ninja','-C',str(p),'-t','deps'],capture_output=True,text=True,check=True)
    blocks = re.split(r'(?m)^(?=\S.*: #deps )', result.stdout)
    valid = [b for b in blocks if re.search(r'^\S.*: #deps .*\(VALID\)', b)]
    found = {n:sum(bool(re.search(r'(?i)(?:^|[/\\])'+re.escape(n)+r'\s*$',b,re.M)) for b in valid) for n in names}
    graphs[name] = {'valid_object_blocks':len(valid),'matching_valid_blocks':found}
include = {n:[] for n in names}
for p in root.rglob('*'):
    if p.suffix.lower() not in ('.c','.h'): continue
    s=p.read_text(encoding='utf-8',errors='replace')
    for n in names:
        if p.name.lower()!=n and re.search(r'#\s*include\s*[<"]'+re.escape(n)+r'[>"]',s,re.I): include[n].append(str(p).replace('\\','/'))
out={'headers':{p.name:hashlib.sha256(p.read_bytes()).hexdigest().upper() for p in headers},'literal_direct_includes':include,'graphs':graphs,'scope':'Captured x86 release dependency snapshot; literal direct C/H includes; not a complete proof of other profiles, generated or external consumers.'}
pathlib.Path('artifacts/mvdm-audit/host-ch-scope/legacy-licensing-header-observation.json').write_text(json.dumps(out,indent=2)+'\n')
print(json.dumps(out,indent=2))
