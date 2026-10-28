"""Part 1 RTL per-key current project copy and graph-edge comparison."""
from pathlib import Path
import json,hashlib,difflib
items=[x for x in json.loads(Path('artifacts/mvdm-audit/nine-way/part-1/external-remaining-evidence.json').read_text(encoding='utf-8-sig'))['files'] if x['family']=='rtl'];gs=[Path('build/.ninja/msvc-x86-release/worker/build.ninja'),Path('build/.ninja/msvc-x86-release/wow32/build.ninja'),Path('build/.ninja/msvc-x64-release/frontend/build.ninja'),Path('build/.ninja/msvc-x64-release/service/build.ninja')];lines={str(g):g.read_text(encoding='utf-8',errors='replace').replace('$:',':').splitlines() for g in gs}
for x in items:
 s=x['path']; tail=s.split('/ntos/rtl/',1)[1] if '/ntos/rtl/' in s else None;p=Path('src/opennt-host/base/ntos/rtl')/tail if tail else None;x['project_candidate']=str(p) if p else None;x['project_sha256']=hashlib.sha256(p.read_bytes()).hexdigest() if p and p.is_file() else None;x['exact_identity']=bool(x['project_sha256'] and x['project_sha256']==x['sha256']); x['normalized_diff']=None
 if x['project_sha256']:
  old=Path(('O:/repos.external/OpenNT/' if x['edition']=='OpenNT' else 'O:/repos.external/OpenNT-4.5/')+s).read_text(encoding='latin1').splitlines();new=p.read_text(encoding='latin1').splitlines(); edits=[v for v in difflib.SequenceMatcher(None,old,new,autojunk=False).get_opcodes() if v[0]!='equal'];x['normalized_diff']=dict(hunks=len(edits),removed=sum(j-i for tag,i,j,k,l in edits),added=sum(l-k for tag,i,j,k,l in edits))
 term=str(p.resolve()).replace('\\','/').lower() if p else ''; x['candidate_graph_edges']=[dict(graph=g,line=t[:1000]) for g,ls in lines.items() for t in ls if term and term in t.replace('\\','/').lower()];x['suggested_status']='pending-specific-rtl-owner-and-semantics'
print(json.dumps(dict(files=items,limitations='Same relative name is a candidate, not proof of lineage; graph text and full source hashes only.'),indent=2))
