import json
from pathlib import Path
p=Path('artifacts/mvdm-audit/nine-way/part-2');d=json.loads((p/'dos-tools-dispatch-observations.json').read_text(encoding='utf-8-sig'))
info={
'dos/v86/doskrnl/dos/dispatch.txt':'Contains historical DOS dispatch assembler text, not merely prose; exact build use is unresolved. Pending.',
'dos/v86/doskrnl/dos/makedos.cmd':'Invokes getsize then nmake around size-include update. Historical build script only; not executed.',
'dos/v86/doskrnl/dos/msioctl.asm':'Tiny assembler wrapper includes IOCTL.INC; exact selected profile and downstream equivalence unresolved. Pending.',
'dos/v86/doskrnl/dos/state.rst':'Historical editor state naming source paths. Documentation/editor state role only; does not exclude referenced functionality.',
'dos/v86/redir/fudgedef.inc':'Observed REDIR uses rdrmisc.inc for LM20_CNLEN; exact alternate profiles remain pending. See shard-2/redir-fudgedef-owner-review.md.',
'dos/v86/redir/localmac.inc':'Listed in original REDIR makefile object dependencies. Historical guest-build input in that recipe; runtime redirector unproven.',
'dos/v86/redir/obj/redir.exe':'Current-only binary absent from both original trees and differs from package/installed REDIR.EXE. Pending provenance and actual loader selection.',
'dos/v86/redir/rdrmisc.inc':'Original redir.asm includes this and uses LM20_CNLEN. Historical guest-build declaration; networking semantics unproven.',
'dos/v86/tools/src/buildidx/buildidx.c':'Historical messages build invokes BUILDIDX for COUNTRY.msg; original tool source. Build-tool role only, no live locale proof.',
'dos/v86/tools/src/buildidx/buildidx.def':'Original tool module definition; exact historical shared linker consumption unproven. Pending.',
'dos/v86/tools/src/getsize/getsize.c':'Original DOS build helper uses map files and size include, invoked by makedos.cmd. Historical tool source; no binary provenance claim.',
'dos/v86/tools/src/getsize/getsize.def':'Tool module definition, but shared historical maketool.def link selection unproven. Pending.',
'dos/v86/tools/src/h2inc/h2inc.doc':'Historical documentation for header conversion helper. Documentation role only, no assertion generated files are correct.',
'dos/v86/tools/src/h2inc/h2inc.mak':'Historical helper build recipe; REDIR makefile contains h2inc conversion. Build metadata only; not executed.',
'dos/v86/tools/src/stripdd/stripdd.def':'Historical stripdd tool module definition; HIMEM recipe invokes stripdd. Exact DEF linker consumption unproven; pending.',
'dos/v86/tools/src/stripz/stripz.def':'Historical stripz tool module definition; NTDOS recipe invokes stripz. Exact DEF linker consumption unproven; pending.',
'dos/v86/lib/oldnames.lib':'Original OMF library; exact historical linker consumer unresolved. Pending; see shard-2/guest-library-role-review.csv.',
'dos/v86/lib/slibce.lib':'Original OMF library; exact consumer and output lineage remain unresolved. Pending; see shard-2/guest-library-role-review.csv.',
'dpmi32/i386/dpmimem.c':'Original i386 DPMI allocator; captured worker map names dpmimemr.obj provider instead. Allocation/reporting behavior differs and requires runtime proof; pending; see shard-3/dpmi-allocator-role-review.md.',
'ieuvddex/alpha/helpp.c':'Original Alpha debugger help helper has empty body; ieuvddex help calls helpp. Alpha debugger tooling/other profiles are distinct and remain unverified; historical architecture-specific helper role only.'}
assert set(info)=={x['path'] for x in d['files']}
head='''# Part 2: DOS kernel, redirector, tools, DPMI and debugger residuals\n\nThese are the remaining 20 owned MVDM keys not in the DOS shared inputs, locale, DOSX or fax family reports. The companion JSON records each key and original/current SHA-256: 19 match both original editions; `redir/obj/redir.exe` is current-only. The ZIP REDIR.EXE matches installed System32, but differs from this supplemental source-tree EXE. Historical recipes and docs are inspected only; no build or guest was executed.\n\n| Owned key | Evidence and limited suggested disposition |\n| --- | --- |\n'''
body='\n'.join(f"| `{x['path']}` | {info[x['path']]} |" for x in d['files'])
tail='''\n\nDo not treat file-role decisions as removal of DOS, DPMI, debugger, guest memory or redirector functionality. Earlier independent reports linked in the table contain function/recipe and lifecycle boundaries; source identity, build edges, or map ownership do not establish runtime equivalence. Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-2/dos-tools-dispatch-observer.py` and compare JSON. No product, guest, shared ledger or build tree changed.\n'''
(p/'dos-tools-dispatch-review.md').write_text(head+body+tail,encoding='utf-8')
