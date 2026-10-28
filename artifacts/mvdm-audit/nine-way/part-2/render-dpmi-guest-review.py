import json
from pathlib import Path
p=Path('artifacts/mvdm-audit/nine-way/part-2');d=json.loads((p/'dpmi-guest-family-observations.json').read_text(encoding='utf-8-sig'))
selected={'486/dxboot.asm','486/dxutil.asm','dxbug.asm','dxdisk.asm','dxend.asm','dxendpm.asm','dxrom.asm'}
alternate={'dxboot.asm','dxfunc.asm','dxint31.asm','dxintr.asm','dxmain.asm','dxstrt.asm','dxutil.asm'}
header='''# Part 2: DOSX protected-mode guest sources and interfaces\n\nThis report covers all 32 owned `dpmi/` keys; the companion JSON records each original/current SHA-256 and each literal reference with path, line and source snippet. Both original editions and the current mirrors match for these files. Captured ZIP, installed and source-tree 486 DOSX.EXE are byte-identical, but this does not prove the original recipe built the binary. The original x86 profile uses `dosxi.lnk`, conditional object inputs and the 486-specific source rules. Real/protected-mode transitions, fault handling, task lifecycle, memory mapping and NetBIOS behavior are not proven by source or byte identity.\n\n| Owned key | Original source/recipe evidence | Suggested bounded status |\n| --- | --- | --- |\n'''
lines=[]
for x in d['files']:
 n=x['path'][5:];refs=x['literal_references'];r=', '.join(v['path']+':'+str(v['line']) for v in refs[:3]) or 'No literal filename reference in captured DPMI text scan';r=r.replace('|','\\|')
 if n in selected: status='Historical guest source selected by observed 486 rule/link; runtime and binary lineage unproven.'
 elif n in alternate: status='Same-name root source differs from selected 486 rule; alternate profile and semantics pending.'
 elif n in ('dxvcpi.asm','dxvcpibt.asm'):status='VCPI object group disabled by observed `!if 0`; alternate profile and VCPI capability pending.'
 elif n=='dxhpbios.asm':status='Object rule exists but absent from observed x86 response; alternate profile pending.'
 elif n=='dosx.def':status='Recipe prerequisite, exact use by linker unresolved; pending.'
 elif n.endswith('.inc') or n.endswith('.mas'):status='Original declaration/macro; exact active conditional expansion and all consumers pending.'
 elif n=='program.sed':status='Historical dependency-rewrite tool input; runtime is separate.'
 else:status='Original source; selected consumer/profile and behavior pending.'
 lines.append(f'| `{x["path"]}` | {r}; {x["literal_reference_count"]} literal refs total | {status} |')
tail='''\n\nThe selected 486 input status is a file/build-role recommendation, not a license to delete guest code or remove DPMI functionality. Direct file references can appear in comments, inactive blocks and same-name paths; the cited source/recipe lines must be inspected, and absence of literal references cannot prove absence of consumers. `486/dxmain` is called by a recipe whose prerequisite names root dxmain: this mixed edge must not be flattened into an unsupported identity assumption. Unverified or alternate-path entries remain pending.\n\nReplay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-2/dpmi-guest-family-observer.py` and compare JSON. No guest, build, product, shared ledger or external source was modified.\n'''
(p/'dpmi-guest-family-review.md').write_text(header+'\n'.join(lines)+tail,encoding='utf-8')
