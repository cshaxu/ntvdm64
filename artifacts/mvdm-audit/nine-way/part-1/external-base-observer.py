"""Part 1 BASE client/server external keys and project-copy graph edges."""
from pathlib import Path
import json,hashlib,difflib
items=[x for x in json.loads(Path('artifacts/mvdm-audit/nine-way/part-1/external-remaining-evidence.json').read_text(encoding='utf-8-sig'))['files'] if x['family']=='native-user-client'];gs=[Path('build/.ninja/msvc-x86-release/worker/build.ninja'),Path('build/.ninja/msvc-x86-release/wow32/build.ninja'),Path('build/.ninja/msvc-x64-release/frontend/build.ninja'),Path('build/.ninja/msvc-x64-release/service/build.ninja')];lines={str(g):g.read_text(encoding='utf-8',errors='replace').replace('$:',':').splitlines() for g in gs}
for x in items:
 p=Path('src/opennt-host')/x['path'];x['project_candidate']=str(p);x['project_sha256']=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None;x['exact_identity']=bool(x['project_sha256'] and x['project_sha256']==x['sha256']);term=str(p.resolve()).replace('\\','/').lower();x['candidate_graph_edges']=[dict(graph=g,line=t[:1000]) for g,ls in lines.items() for t in ls if term in t.replace('\\','/').lower()];x['suggested_status']='pending-specific-base-owner-and-semantics'
 if x['project_sha256']:
  old=Path('O:/repos.external/OpenNT')/x['path'];a=old.read_text(encoding='latin1').splitlines();b=p.read_text(encoding='latin1').splitlines(); edits=[v for v in difflib.SequenceMatcher(None,a,b,autojunk=False).get_opcodes() if v[0]!='equal'];x['normalized_diff']=dict(hunks=len(edits),removed=sum(j-i for tag,i,j,k,l in edits),added=sum(l-k for tag,i,j,k,l in edits))
print(json.dumps(dict(files=items,limitations='Same path is a candidate not automatic lineage; graph edge not runtime execution.'),indent=2))
