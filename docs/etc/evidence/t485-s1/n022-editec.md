# S15 P48 N022 EDIT common operations - delivered

## Scope and fixed inputs

Start e98043df7; P47 runtime55 delivered. Complete client/editec.c migration.
XP blob315c6d8b28544566cca0dd7599b51ef1b1fa80db at pinned SP1 commit
 a2f6d7c93aa4f11efc51147566d5961c59a8ef89; OpenNT blob
2b51b1e0e8a8e8025bc16d7361910e13c65d66ae. Prior nonerror1point/5954lines.
Global1329files/784exact/795nonerror same/534different/7804points/113169lines.

## Implementation and direct dependencies

Retain complete XP common text, DBCS, undo, menu and IME algorithms and original
unselected guards as #error/#line. Pair ECInsertText pointer-count ABI in
usercli.h and both existing editml/editsl callers. Preserve existing canonical
control-finalization adapter and native control-color edge. No second lifecycle.

Modern public INPUTCONTEXT ends at dwReserve[3] (SDK immdev.h); XP private immp.h
adds uSavedVKey, fChgMsg, fdwFlags and fdw31Compat after that public prefix.
Two XP edit/imectl private flag handshakes cannot write modern IMM memory via
that private layout. Remove these private field accesses, retaining public IMM
calls and original EDIT-owned composition state. This does not claim full XP
private IME-window compatibility. Public synchronous SyncSoftKbdState is reused
from XP imectl.c1549-1570 in existing native binding, preserving symbol and ABI.

## Final verification and delivery

runtime57 cached full build, dual13-image26hash publication, controls40/40,
real CCPU window-borrow and genuine guest SOL Game > Options pass.
[Receipt](n022-editec-result.json), [census](n022-editec-census.json).
No guest changes. N02219/39; overall57/159 and S15 directories9/14 unchanged.
Seven completed mirrors since P42 Full24/24. Next editml.c. No S/T closure.

## Paired resource and native binding audit

Restore the complete XP EDIT MENUEX template and five IME strings in existing
res.rc, which is already linked into WOW32 and loaded through hmodWOW32.
This is a direct dependency, not a completed migration of unrelated USER
resources; its whole-file baseline remains OpenNT. XP resource IDs and the
UserIsFullWidth declaration are paired in original usercli.h; UserIsFullWidth
already comes from migrated original RTL drawtext.c. No duplicate algorithm.
Existing menu provider owns DeleteMenu; clipboard operations use public native
providers. UserVerify preserves original retail evaluation; VER50 is the
original0x0500. RTL_UI uses the same exact Arabic/Hebrew default language IDs
as XP user.h3964-3966. XP kernel/init.c2927 obtains UILangID with
ZwQueryDefaultUILanguage; modern GetUserDefaultUILanguage supplies that user
setting without accessing the private SERVERINFO layout.

First cached build failed only on missing paired declarations. Second build
passed and runtime56 was dual-published, but control fixture linking exposed
missing imm32.lib in two existing text-fixture link lists; no test ran in
that attempt. Those lists are now fixed. The final RTL_UI comparison correction
requires build-r3 and a new runtime57 before acceptance. runtime56 is not the
final accepted package. Do not reuse its preliminary status as final evidence.

## Nonerror mirror accounting

| File | Before points/lines | After points/lines | Baseline |
| --- | ---: | ---: | --- |
| src/non-mvdm/windows/core/ntuser/client/editec.c | 1/5954 | 13/81 | OpenNT to XP |
| src/non-mvdm/windows/core/ntuser/client/editml.c | 1/7554 | 1/7554 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/client/editsl.c | 1/4920 | 1/4920 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/client/res.rc | 2/149 | 10/204 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/client/usercli.h | 1/4672 | 1/4706 | OpenNT to OpenNT |

Global1329files,784byte-identical,795nonerror-identical,534different,7824points/107385lines. Delta0/0/0/0/+20/-5784. Baselines1286XP/43OpenNT. Original LF restores distinct actual adapter/line-map blocks formerly joined by CRLF differences; not20new behavioral adaptations. Raw newline/EOF differences are counted; original #error bodies excluded. editec.c retains13points/81lines: existing lifetime and native color edges, private IME layout boundary and diagnostic mappings. Paired res.rc is not a complete original USER resource import.
