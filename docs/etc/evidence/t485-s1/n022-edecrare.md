# S15 P47 N022 EDIT rare operations - delivered

## Scope and inputs

Start HEAD e39ca5e1cfa653315e8e6b7468c080bd2b768a31. P46 is delivered.
Primary client/edecrare.c: OpenNT b5ec4b8c6242509feae80ad88ba085033343c680,
XP 597a00b2c5edf618a3d00c0f20575ca3757acc9b at pinned XP SP1 revision
 a2f6d7c93aa4f11efc51147566d5961c59a8ef89. Initial nonerror1point/1592lines.

## Whole-file and direct dependencies

Restore complete XP EDIT font, margin, password, creation/destruction and IME
composition algorithms. Keep original retail/CUAS-unselected guards with
#error and #line. The old SUPPORT_LPK branches no longer exist upstream;
restore XP's typed LPK table and NULL default, not another local algorithm.
Pair original ED layout and LPK declarations in their current original owners,
ECCreate's two callers and all old caret-width callers. Pair the original
ECInOutReconversionMode and ECGetDBCSVector helpers in editec.c. These paired
files retain existing whole-file baselines until their ordered migration.

Native IMM APIs provide synchronous leaf operations; no substitute IME state
machine. The existing scoped system-input capture obtains SPI_GETCARETWIDTH.
Reuse the existing GDI export resolver for QueryFontAssocStatus: installed
SysWOW64/gdi32.dll export confirmed in gdi-exports.log. The original font
association and DBCS algorithms remain in editec.c.

## Final validation and delivery

Cached full build passed. Runtime55 dual publication verified13images/26hashes.
Controls40/40, real CCPU window-borrow and genuine guest SOL Game > Options pass.
The first lifecycle attempt stopped at fixture linking: missing imm32.lib;
the fixture's existing shared library list was corrected. Only that failed
case and the unexecuted SOL case were rerun. The product was unchanged.
[Receipt](n022-edecrare-result.json), [census](n022-edecrare-census.json).
No original guest change/rebuild. No S/T closure. N02218/39; overall57/159,
S15 directories9/14. Six completed mirrors since P42 Full24/24. Next editec.c.

## Remaining boundaries and line mapping

edecrare.c retains only10nonerror lines, all original diagnostic #line mappings.
Ten original unselected branches remain explicit #error; no invented guards.
The selected original XP algorithms and typed LPK state are restored intact.
Read-only caret position uses GetCaretPos; existing scoped caret mutators remain.
The scoped input snapshot captures system caret width. Public native IMM leaves
are synchronous; QueryFontAssocStatus reuses the existing native GDI resolver.
These are actual compiled consumers, not a duplicate IME implementation.
Paired ED layout receives XP line mapping; remaining old declarations retain
original diagnostic line numbers. Original create/caret callers were updated
together. Other files keep OpenNT baselines until their complete migration.

## Nonerror mirror accounting

| File | Before points/lines | After points/lines | Baseline |
| --- | ---: | ---: | --- |
| src/non-mvdm/windows/core/ntuser/client/clglobal.c | 8/158 | 8/157 | XP to XP |
| src/non-mvdm/windows/core/ntuser/client/edecrare.c | 1/1592 | 10/10 | OpenNT to XP |
| src/non-mvdm/windows/core/ntuser/client/editec.c | 1/5834 | 1/5954 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/client/editml.c | 1/7554 | 1/7554 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/client/editsl.c | 1/4920 | 1/4920 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/client/edmlonce.c | 1/233 | 1/233 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/client/edslrare.c | 1/383 | 1/383 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/client/usercli.h | 1/4638 | 1/4672 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/inc/user.h | 1/5267 | 1/5318 | OpenNT to OpenNT |

Global1329files,784byte-identical,795nonerror-identical,534different,7804points/113169lines. Delta0/0/0/0/+9/-1378. Baselines1285XP/44OpenNT. Restoring original line endings splits the former whole-file block into10actual line-map differences; this adds9points while removing1582diff lines in edecrare.c. Paired ABI/helpers contribute204diff lines, yielding net1378reduction. Raw newline/EOF differences remain counted; #error bodies are excluded.
