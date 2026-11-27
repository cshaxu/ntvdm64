# S15 P46 N022 text drawing - delivered

## Scope and original objects

Start HEAD 0cf47904dda310bfc693fa555da6e8f0d0605bb6. P45 is delivered.
Client drawtext.c OpenNT blob37ede1b9a914e517254825a6fb8a0755e7bd6cb0;
XP blob326ab0961914eef3b9a68c82ccfaaefcac367eba. Starting nonerror4points/140lines.
Paired N025 rtl/drawtext.c XP blob0e96779bab9c64ac44fad85d6a597ab021247659.
All XP objects use fixed a2f6d7c93aa4f11efc51147566d5961c59a8ef89.
Snapshots and comparisons are under build/t485-s15/n022-drawtext.

## Whole-chain decision

XP adds the charset argument to TabTextOut and DrawTextExWorker, and moves
PSMTextOut from RTL into client with a DWORD prefix-flags argument. Consequently
client-only replacement is not a valid ABI migration. Pair the complete RTL
body and update their call ABI and the mngrayc prefix-flags calculation. lboxctl2
retains its existing zero tab origin until its complete XP layout migration;
its selected structure has no fRightAlign field (confirmed compiler C2039). These
caller files remain individually registered against their existing baseline
until their whole-file DIR audit; do not claim them fully migrated.

Restore the original DRAWTEXTDATA/LPK declarations in user.h and original
clglobal defaults. XP clglobal:108-110 defaults to UserLpkTabbedTextOut,
UserLpkPSMTextOut and NULL DrawTextEx LPK. XP clinit:1313/1317/1321 installs
optional LPK hooks. Preserve this distinction; no replacement text algorithm
or invented dispatch state. The current product has no installed custom LPK.
Existing task client info, character conversion and native GDI providers own
the external leaves. Preserve LocalAlloc ownership shared with MBToWCSEx.

RTL is independently compiled here and keeps its existing precomp include.
Original kernel, DBG, LATER and #if0 arms retain their conditions with #error
and original logical line mapping. Actual client code is retained.

## Final delivery

runtime54 cached full build, dual13-image26hash publication, controls40/40,
real CCPU window-borrow and SOL Game > Options passed. No guest change.
[Receipt](n022-drawtext-result.json), [census](n022-drawtext-census.json).
N02217/39, paired N025 RTL file delivered; S15 directories9/14 and overall57/159
unchanged. Five completed mirrors since P42 Full24/24; no S/T closure.

## Necessary native boundaries and attempts

Production owner compile/link passed after reusing the installed x86 GDI
exports GdiGetCodePage and GdiValidateHandle in the existing once-only native
control resolver. dumpbin confirmed both exports. SDK import libraries lacked
them. Source-local aliases retain restored ANSI wrappers alongside native USER
import consumers; no original wrapper body is removed.

The new XP system-font path reads ghdcBits2. No product assignment initializes
that original global; the established drawing owner instead has scoped DCs.
Existing wow_edit_capture_context already measures SYSTEM_FONT into tmSysFont.
Use that captured tmCharSet in one expression, preserving the adjacent original
metrics and avoiding another DC/state owner. This is the sole functional RTL
adaptation in addition to its independent-compilation include and line mapping.
The owner's standing small mirror-change approval covers this one-point change.

First whole-product attempt found the missing old-listbox field. Owner retry
then found DBCS_CHARSIZE and two original macros; restoring their exact XP
values and provider binding completed owner compilation/link. runtime51 was
published, then16 redundant inactive #line directives were removed; runtime52
was published after rebuilding. Neither is the final acceptance package: the
system-font correction follows them. Final build-final-r3 and publisher-r3
prepare runtime53. PID/start receipts remain under the batch build directory.

The native cohort initially found a generated mnemonic include path and then
missing test provider linkage after the XP function relocation. Tests now use
live-source projections for the prefix-only target and the actual client PSM
body/default pointer, plus explicit fixture-owned task input. Original failed
attempt logs are retained; no passing result is yet claimed for P46.

## Paired conversion contract

Final call-chain review found that XP ANSI drawing passes -2 to MBToWCSEx
for a count of -1, while the still-OpenNT conversion body rejected -2. Restore
the exact XP negative-count block in rtl/chartran.c, including its original
constant-value assertion. Retain the OpenNT whole-file baseline and existing
allocation/provider ownership; this is a paired partial migration, not a
completed N025 file. The original compile assertion is counted as a real
difference, not a project unsupported-branch exclusion. Existing string and
drawing fixtures now cover the conversion and actual ANSI wrapper together.
The runtime53 targeted cohort40/40, window-borrow and SOL passed but lacked
this contract; final build/publication runtime54 and affected tests supersede
that candidate. Preserve initial and intermediate evidence.

## Final nonerror mirror accounting

| File | Before points/lines | After points/lines | Baseline |
| --- | ---: | ---: | --- |
| src/non-mvdm/windows/core/ntuser/client/clglobal.c | 8/161 | 8/158 | XP to XP |
| src/non-mvdm/windows/core/ntuser/client/drawtext.c | 4/140 | 0/0 | OpenNT to XP |
| src/non-mvdm/windows/core/ntuser/client/lboxctl2.c | 1/7039 | 1/7039 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/client/mngrayc.c | 1/1175 | 1/1185 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/inc/user.h | 1/5223 | 1/5267 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/rtl/chartran.c | 1/1014 | 1/1036 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/rtl/drawtext.c | 1/2387 | 17/20 | OpenNT to XP |

Global1329files:784byte-identical,795nonerror-identical,534different,7795points/114547lines. Delta0/+1/+1/-1/+12/-2434. Baselines1284XP/45OpenNT. The RTL point increase expands the former newline-joined block into actual include/line-map/charset adaptations; it does not represent new algorithms. Declared unsupported branch bodies remain #error. Partial header/caller work does not switch their whole-file baseline.

Paired conversion source: XP SP1 `windows/core/ntuser/rtl/chartran.c`,
blob `94f249e87adcc8d0edf60cc753eab8cac18ad173` at the pinned revision.
Only MBToWCSEx negative-count handling is paired here; the remaining
file retains its registered OpenNT baseline for the ordered N025 migration.
