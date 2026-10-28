import json
from pathlib import Path
p=Path('artifacts/mvdm-audit/nine-way/part-5');d=json.loads((p/'host-input-family-observations.json').read_text(encoding='utf-8-sig'));head='''# Part 5: SoftPC host inputs, historical build resources and suballocator declarations\n\nAll 39 owned paths match both frozen original editions. The observer resolves exact paths in VALID `ninja -t deps` records for captured worker/WOW32 graphs, recording up to all object hits for each, and checks full paths in their Ninja files. It also inspects code-section flags for the two selected C objects. Dependency presence is not proof a particular macro or function is active; dependency absence is bounded by cached records and does not prove all profiles or runtime irrelevance.\n\n| Owned key | Captured graph/dependency evidence | Suggested bounded status |\n| --- | --- | --- |\n''';lines=[]
for x in d['files']:
 n=x['path'];deps=x['valid_dependency_objects'];graphs=x['exact_graphs'];ev=f"graphs {','.join(graphs) or 'none'}; {len(deps)} exact VALID object deps";ev+=('; e.g. '+deps[0]) if deps else ''
 if n=='softpc.new/base/support/get_env.c':status='Selected source and object has code sections; pending linked function/actual environment semantics.'
 elif n=='softpc.new/base/video/ega_trcr.c':status='Selected/compiled object has no code section in captured profile; retain EGA_DUMP/other profile and video behavior separately.'
 elif deps:status='Exact current dependency; candidate partial input evidence, not proof every declaration is used.'
 elif n=='suballoc/foo.bar':status='Historical private-header-like file, not byte-identical to suballcp.h; actual purpose/consumer pending.'
 elif n=='suballoc/suballoc.txt':status='Historical suballocator documentation; actual allocator behavior stays separate.'
 else:status='Exact consumer/profile unresolved; pending. Do not infer unused from no captured dependency.'
 lines.append(f'| `{n}` | {ev} | {status} |')
tail='''\n\n`yoda.h`, `host_gen.h`, `host_hfx.h`, `nt_fulsc.h`, and `suballcp.h` have concrete current-object dependency records; follow their actual usage and original conditionals before marking semantic restoration. Resource scripts and icon/definition inputs may be transformed/generated, so a missing literal graph path does not close their lifecycle. No guest, build tree, product or shared ledger was modified. Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-5/host-input-family-observer.py` and compare JSON.\n''';(p/'host-input-family-review.md').write_text(head+'\n'.join(lines)+tail,encoding='utf-8')
