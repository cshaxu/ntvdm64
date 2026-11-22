from pathlib import Path
import csv, json, re, hashlib
root=Path(__file__).resolve().parents[3]
out=root/'artifacts/build-migration/t484'
rows=list(csv.DictReader((out/'files.csv').open(encoding='utf-8')))
core={
 'CMakeLists.txt':('S2','Replace interpreter configure hooks with native declarative targets; remove released-image bootstrap inputs.'),
 'CMakePresets.json':('S2','Retain x86/x64 release/cache identity; preserve stable architecture roots.'),
 'New-T310OriginalSoftpcNinja.ps1':('S2','Move source groups/flags/link edges to canonical build metadata; retain original SOURCES selections and per-source flags; remove generator after callers migrate.'),
 'New-T404S5Wow32ProviderNinja.ps1':('S2','Preserve 72 original WOW bodies, support owners, parent import library, flags and exports in same canonical metadata.'),
 'New-NativeWorkerNinja.ps1':('S2','Replace x86 graph scraping with architecture-specific selection from shared source metadata; retain finite original-source allowlist.'),
 'New-NativeHook64Ninja.ps1':('S2','Declare Hook64 closure and private fixtures; retain Hook32/64 distinctions and service IDL ordering.'),
 'Initialize-NinjaTree.ps1':('S2','Consolidate tree validation/cache setup into CMD entry plus existing native build configuration.'),
 'GenerateCvidcCpuBinding.mjs':('S2','Preserve original metadata extraction, exact slot/declaration counts and changed-only writes; no frozen generated ABI copy.'),
 'Generate-FrontendFont.ps1':('S2','Preserve pinned ROM identity and 256x14/256x16 extraction; native binary transformer needed or equivalent native resource use with identical consumers.'),
 'GenerateCvidcVectorProviderStubs.mjs':('S3','Produce focused test stubs through same source-generation owner; never link stubs into production.'),
 'Set-PackageIdentity.ps1':('S4','Preserve same GUID bytes/RCDATA identity across eleven images; replace embedded C# runtime compilation with bounded native resource operation.'),
 'Stage-System32ProductPackage.ps1':('S4','CMD stage fresh complete set, explicit image producers, machine validation and shared identity; no previous-task baseline dependency.'),
 'Stage-NativeWorkerImage.ps1':('S2','Remove product graph copies where consumers can use explicit producer outputs; remaining fixture copies retain PE/subsystem/hash validation.'),
 'BuildTree.cmake':('S2','Keep native declarative configuration/cache validation; remove script-mode adapter once CMD owns entry.'),
 'Clear-BuildOutput.ps1':('S4','Replace exact owned-target cleanup in CMD; retain root/junction checks and evidence lifetime policy.'),
 'extract-guest-memory-fixture.py':('S3','Preserve source-bound test projections and markers; migrate generator without changing original guest or production bodies.'),
 'wire-guest-memory-fixture.py':('S3','Declare fixture dependency edges once instead of post-editing generated Ninja.'),
 'Verify-VdmTibStorage.mjs':('S2','Retain post-link TIB storage validation, replacing required Node invocation; not merely an optional test.'),
}
for row in rows:
    name=row['file'];leaf=Path(name).name
    if name in core or leaf in core:
        stage,action=core.get(name,core.get(leaf))
    elif name.startswith('src/addon/'):
        stage='S3';action='Retain existing native CMD utility producer or migrate assembly producer to CMD; reconcile declared inputs and released output.'
    elif '/win31-setup/' in name:
        stage='S4';action='Trace package preparation caller; preserve authorized immutable-media boundaries; merge or retire only after current patch producer owns behavior.'
    elif name.startswith('tools/build/'):
        if leaf.endswith(('.json','.cmake')):
            stage='S2';action='Retain declarative metadata and input identities; do not interpret as executable scripting.'
        elif leaf.startswith(('Stage','Set-Package')):
            stage='S4';action='Consolidate active publication contract into CMD release path; historical names/defaults are not current package authority.'
        else:
            stage='S3';action='Trace recorded callers; migrate actual fixture compilation to CMD/native targets; keep independent test assertions separate; no deletion from zero lexical hits alone.'
    else:
        stage='S3';action='Inspect recorded compilation references; move actual producer to shared CMD/native target; preserve standalone runtime/assertion runner; comments alone do not prove invocation.'
    row.update(stage=stage,disposition=action)
with (out/'files.csv').open('w',encoding='utf-8',newline='') as f:
    w=csv.DictWriter(f,fieldnames=list(rows[0]),lineterminator='\n');w.writeheader();w.writerows(rows)
graphs=[]
for architecture in ('x86','x64'):
    base=root/f'build/.ninja/msvc-{architecture}-release'
    for graph in sorted(base.glob('*/build.ninja')):
        if graph.parent.name not in ('worker','worker-reference-x86','wow32','frontend','monitor','launcher','service','hook'):continue
        raw=graph.read_bytes();text=raw.decode('utf-8-sig')
        hits=[dict(line=i,text=line.strip()) for i,line in enumerate(text.splitlines(),1)
              if line.lstrip().startswith('command =') and re.search(r'(?i)\b(node|python|powershell|pwsh)(?:\.exe)?\b',line)]
        graphs.append(dict(path=graph.relative_to(root).as_posix(),sha256=hashlib.sha256(raw).hexdigest(),
            build_edges=len(re.findall(r'^build ',text,re.M)),forbidden_command_rules=hits))
(out/'existing-graphs.json').write_text(json.dumps(dict(scope='Existing cache graph inspection only; not a fresh configuration or build result.',graphs=graphs),indent=2)+'\n',encoding='utf-8',newline='\n')
print(json.dumps(dict(files=len(rows),graphs=len(graphs),command_rules=sum(len(x['forbidden_command_rules']) for x in graphs),stages={s:sum(r['stage']==s for r in rows) for s in ('S2','S3','S4')})))
