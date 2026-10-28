"""Read-only census of currently present Ninja graphs and exact PIF header dependencies."""
import hashlib,json,re,subprocess
from pathlib import Path
root=Path('build/.ninja')
out=[]
for graph in sorted(root.glob('**/build.ninja')):
    text=graph.read_text(encoding='utf-8',errors='replace')
    if not re.search(r'(?i)src[/\\]mvdm[/\\]dos[/\\]command[/\\]cmdpif\.c',text):
        continue
    run=subprocess.run(['ninja','-C',str(graph.parent),'-t','deps'],capture_output=True,text=True)
    if run.returncode:
        out.append({'graph':str(graph),'error':run.stderr.strip(),'returncode':run.returncode});continue
    records=re.split(r'(?m)(?=^[^\s].*: #deps )',run.stdout)
    recorded=[x for x in records if re.match(r'^[^\s].*: #deps ',x)]
    matching=lambda needle:[x.splitlines()[0] for x in recorded if needle in x.replace('\\','/').lower()]
    stale=sum('(STALE)' in x.splitlines()[0] for x in recorded)
    out.append({'graph':str(graph),'sha256':hashlib.sha256(graph.read_bytes()).hexdigest().upper(),
       'deps_output_sha256':hashlib.sha256(run.stdout.encode('utf-8')).hexdigest().upper(), 'dependency_blocks':len(recorded),'stale_blocks':stale,
       'duplicate_matches':matching('/dos/command/cmdpif.h'),
       'selected_matches':matching('/softpc.new/host/inc/nt_pif.h')})
Path('artifacts/mvdm-audit/host-command/cmdpif-cached-graphs-evidence.json').write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
for r in out:
 print(r['graph'], 'blocks',r.get('dependency_blocks'), 'stale',r.get('stale_blocks'), 'duplicate',len(r.get('duplicate_matches',[])), 'selected',len(r.get('selected_matches',[])), 'error',r.get('error',''))
