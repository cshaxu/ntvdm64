# S15 P54 N022 USER global declarations - delivered

## Scope and fixed inputs

Start HEAD9b58c3c93, runtime62 delivered. Whole globals.h migrates from
OpenNT blob07c580fc36435b817cd572c391ca6580bd890973 to XP SP1
blob3cbda520a6dea4986a07735e74361a64f33676c1 at pinned revision
a2f6d7c93aa4f11efc51147566d5961c59a8ef89. Before nonerror1point/277lines.
Global1329files/785byte-identical/796nonerror-identical/533different/
7835points/93164lines. Original guest unchanged and unbuilt.

## Whole-file restoration and direct type dependency

Restore all XP global externs, const-qualified tables, resource callbacks,
WOW registered callbacks and unused declarations. Preserve original guards;
DBG, JANUS and MESSAGE_PUMP_HOOK are not selected in the current control build,
so their bodies use #error and original #line. RC_INVOKED retains its original
macro branch; no unconditional declaration is removed.

user.h includes globals.h under the selected client-view/EDIT context.
Reuse wow_user_globals_binding.h, already supplying the original ALWAYSZERO,
ACCELCACHE and INITUSERAPIHOOK declarations for clglobal.c. Include it before
the view's original USER declaration pass. Forward-declare original SERVERINFO
and USERAPIHOOK tags without reproducing layouts or creating storage. Pair the
original CFNSCSENDMESSAGE pointer-sized XP prototype in usercli.h and the
existing compile binding; usercli.h retains its OpenNT whole-file baseline.
No new runtime owner, callback registration or initialization state.

## Final verification and delivery

runtime63 cached full build, dual13-image26hash publication, controls40/40,
real CCPU window-borrow and genuine guest SOL Options pass.
[Receipt](n022-globals-h-result.json), [census](n022-globals-h-census.json).
N02225/39; overall57/159 and S15 directories9/14 unchanged. Three completed
mirror since P51 runtime60 Full24/24; no new Full or S/T closure claimed.

## Mirror accounting

globals.h OpenNT1point/277lines becomes XP2points/2lines, all diagnostic#line.
Three original unselected retail arms retain #error. Every matching
executable original line maps correctly; selected algorithms have no changes.
Original LF restoration exposes the two separate line-mapping blocks.
Global before:1329files/785byte-identical/796nonerror-identical/533different/
7835points/93164lines. After:1329/785/796/533/7836/92889.
Paired usercli.h remains OpenNT1point/4708lines before and after.
Delta:0/0/0/0/+1/-275. Baselines1292XP/37OpenNT. Newline/EOF counted.

## Paired declaration conflict


First build reports only C2373 for usercli.h's old mutable szOneChar extern.
XP usercli.h has no duplicate declaration; XP globals.h and the already
migrated clglobal.c definition both use CONST WCHAR. Remove the obsolete
usercli declaration, retain its blank source line for old diagnostic mapping,
and use the restored original globals declaration. Actual consumers are
btnctl.c and comboini.c, both in this same control-client inclusion graph.
Keep build.log as the failed attempt; final build/publication/tests passed as recorded above.


The first targeted cohort failed at fixture compilation, before executing any
case: wow_button_dc_unreached.c defined mutable szOneChar. Match the original
CONST WCHAR definition in this test-only stub. Product source and runtime63
are unchanged; retry the failed/unexecuted cohort under control-r2 and retain
the first empty-case result and build log. No passing runtime result discarded.
