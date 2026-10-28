"""Read-only search for C-VID empty guard consumers and generated references."""
import hashlib, json, re, subprocess
from pathlib import Path

ROOT=Path('src/mvdm/softpc.new/base/cvidc')
NAMES=('bound','c2cpug','conv','dec','gdinit','inc','light','nanoa','pm','simple','stub')
SEARCH=('src','tools','tests','build/.ninja/msvc-x86-release/worker/build.ninja',
        'build/.ninja/msvc-x86-release/worker/generated')

def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest().upper()

def search(patterns):
    args=['rg','--json','-i','-F','--hidden','--no-ignore','-g','!*.map','-g','!*.lst']
    for p in patterns: args += ['-e',p]
    p=subprocess.run(args+list(SEARCH),capture_output=True,text=True)
    assert p.returncode in (0,1),p.stderr
    result=[]
    for line in p.stdout.splitlines():
        data=json.loads(line)
        if data['type'] != 'match': continue
        d=data['data']; result.append(dict(path=d['path']['text'],line=d['line_number'],text=d['lines']['text'].strip()))
    return sorted(result,key=lambda x:(x['path'].lower(),x['line'],x['text']))

def inspect():
    headers=[]
    for name in NAMES:
        path=ROOT/(name+'_c.h'); lines=[x.strip() for x in path.read_text(encoding='latin1').splitlines() if x.strip()]
        assert len(lines)==3
        match=re.fullmatch(r'#ifndef\s+(\w+)',lines[0]); assert match
        guard=match.group(1)
        assert lines[1]=='#define '+guard and lines[2].startswith('#endif')
        headers.append(dict(name=path.name,path=str(path),guard=guard,sha256=sha(path)))
    guards=search([h['guard'] for h in headers]); filenames=search([h['name'] for h in headers])
    non_guard=[]
    for match in guards:
        matching=[h for h in headers if re.search(r'\b'+re.escape(h['guard'])+r'\b',match['text'],re.I)]
        valid=(len(matching)==1 and Path(match['path']).name.lower()==matching[0]['name'].lower()
               and re.fullmatch(r'#\s*(?:ifndef|define)\s+'+re.escape(matching[0]['guard'])+r'|#\s*endif\s*/\*\s*!\s*'+re.escape(matching[0]['guard'])+r'\s*\*/',match['text'],re.I))
        if not valid: non_guard.append(match)
    return dict(result='pass',headers=headers,search_roots=list(SEARCH),guard_hits=guards,
                non_guard_hits=non_guard,filename_hits=filenames,
                build_graph_sha256=sha(Path(SEARCH[3])),
                scope='Literal guard and filename references in checked roots, with generated worker files; not proof of all build configurations or dynamic include generation')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
