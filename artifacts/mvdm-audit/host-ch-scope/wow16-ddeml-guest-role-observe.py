"""Read-only snapshot of seven guest DDEML source identities and graph edges."""
import hashlib,json,pathlib,re
root=pathlib.Path(__file__).resolve().parents[3]
names=('dmgdb.c','dmgdde.c','dmghsz.c','dmgmem.c','dmgmon.c','dmgq.c','dmgwndp.c')
origins=(root/'src/mvdm',pathlib.Path('O:/repos.external/OpenNT/base/mvdm'),pathlib.Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
digest=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
graphs=list((root/'build/.ninja').rglob('build.ninja'))
results={}
for name in names:
    rel='wow16/ddeml/'+name
    identities=[digest(x/rel) for x in origins]
    if len(set(identities))!=1: raise ValueError(f'source differs: {rel}')
    edges=[]
    for graph in graphs:
        for i,line in enumerate(graph.read_text(errors='replace').replace('\\','/').lower().splitlines(),1):
            if ('wow16/ddeml/'+name) in line: edges.append({'graph':str(graph.relative_to(root)),'line':i,'text':line.strip()})
    includes=[]
    for path in (root/'src').rglob('*'):
        if path.suffix.lower() not in ('.c','.h') or 'wow16' in path.parts:continue
        for i,line in enumerate(path.read_text(errors='replace').splitlines(),1):
            if re.search(r'^\s*#\s*include\s*[<"](?:[^>"\\/]+[\\/])*'+re.escape(name)+r'[>"]',line,re.I): includes.append({'path':str(path.relative_to(root)),'line':i})
    results[rel]={'sha256':identities[0],'selected_graph_edges':edges,'host_includes':includes}
print(json.dumps({'graph_count':len(graphs),'graph_sha256':{str(p.relative_to(root)):digest(p) for p in graphs},'files':results,'limits':'bounded generated Ninja graphs and named host directories; does not prove all host configurations or guest capability'},indent=2))
