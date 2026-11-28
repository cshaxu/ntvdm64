# S15 P50 N022 single-line EDIT - delivered

## Scope and fixed inputs

Start HEAD2e7704201; runtime58 remains delivered. Complete editsl.c migration
from OpenNT blob d2b2c4e8e0a061e7eafe066a678c5648482878d1 to fixed XP SP1
blob2cbfe5583d5c8630cf4217239e53495b130bc24b. Pinned XP revision:
a2f6d7c93aa4f11efc51147566d5961c59a8ef89.
Before nonerror1point/4920lines; global1329files/784byte-identical/
795nonerror-identical/534different/7829points/99839lines.

## Restoration and direct dependencies

Restore the entire XP single-line algorithms: aligned text positioning,
DBCS selection and scrolling, typed LPK hooks, IME composition placement,
caret painting and message dispatch. Original DBG and two literal0 branches
retain #error with original #line; no new conditions or selected-code omission.
Existing native services and P47/P48 ED/LPK/ECInsertText ABI provide dependencies.
SLCreate's old HWND argument is unused in its complete body. Pair its provider
and declaration with XP two-argument caller, retaining the original remaining
OpenNT body and original line positions. Pair SLEditWndProc pointer-sized ABI.
These two whole files keep their OpenNT baseline pending ordered migration.
No new algorithm, callback state, original guest change or rebuild.

## Final verification and delivery

runtime59 cached full build, dual13-image26hash publication, controls40/40,
real CCPU window-borrow and genuine guest SOL Game > Options pass.
[Receipt](n022-editsl-result.json), [census](n022-editsl-census.json).
No guest change. N02221/39; overall57/159 and S15 directories9/14 unchanged.
Nine completed mirrors since P42 Full24/24. No S/T closure.

## Nonerror mirror accounting

| File | Before points/lines | After points/lines | Baseline |
| --- | ---: | ---: | --- |
| src/non-mvdm/windows/core/ntuser/client/editsl.c | 1/4920 | 3/3 | OpenNT to XP |
| src/non-mvdm/windows/core/ntuser/client/edslrare.c | 1/383 | 1/383 | OpenNT to OpenNT |
| src/non-mvdm/windows/core/ntuser/client/usercli.h | 1/4708 | 1/4708 | OpenNT to OpenNT |

Global1329files,784byte-identical,795nonerror-identical,534different,7831points/94922lines. Delta0/0/0/0/+2/-4917. Baselines1288XP/41OpenNT. Restoring LF exposes3actual diagnostic #line blocks rather than one CRLF whole-file block. editsl.c has no nonexcluded algorithm alteration. Three original unselected branches remain #error. All matched executable original lines retain correct diagnostic line mapping. Paired files retain OpenNT baseline. Raw newline/EOF differences remain counted.
