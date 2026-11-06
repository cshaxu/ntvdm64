# T478 S19 — EDIT tab-stop Boolean return

Baseline: d5ba77899. Finding 47 is the original UnThunkEMMsg16 fallthrough:
EM_SETTABSTOPS releases its temporary array and then enters the distinct
EM_GETWORDBREAKPROC conversion. A successful Boolean return may become a
callback address or be replaced by a previously cached procedure identity.
Both retained original source editions contain the missing break.

## Minimal repair

DIV-875 adds one break after the existing signed-count buffer release. It
preserves DIV-346 acquisition/free symmetry, allocation policy, native result,
legitimate word-break conversions and both existing word-break globals. No
new EDIT behavior, pointer reset, callback fallback or guest change.

Source routes SendMessage, SendDlgItemMessage and CallWindowProc converge on
the EDIT class unthunker after storing the native result. This is source route
evidence; the real guest witness below exercises SendMessage, not all three.
The defect and fix reside in their shared selected unthunker.

## Focused current evidence

[Runner](../../../test/integration/verify-wow-edit-tabstop.ps1) links the actual
current MSVC wmsgem.obj without unresolved-symbol/FORCE options, with existing
explicit fail-closed seams for unrelated routes. Twenty-four tab-stop cases
combine counts -1/0/1, stack/heap storage, native return 0/1 and empty/populated
word-break cache. They require unchanged native return/globals, no callback,
and exactly the applicable original heap release. Negative-count/non-null heap
combinations are predicate controls, not claims about constructor reachability.
Six existing legitimate word-break controls cover zero, tagged guest pointers,
high-bit restoration, native callback success/failure and cached identity.
Baseline: 30 cases, 12 failures. Current: 30 cases, zero failures.

The existing message-construction exact-body suite passes 36 scenarios,
including its selected buffer allocation/failure/unwind controls. Its historical
source reader needed the existing src/mvdm-host -> src/mvdm baseline-path
translation; only that test lookup is corrected. This suite uses controlled
lower services and is not actual guest execution or proof of every caller.

The [guest runner](../../../test/integration/verify-wow-edit-tabstop-guest.ps1)
uses the existing independently authored NE probe, assembled without modifying
original guest media. An isolated same-package copy runs on a private desktop.
The staged manifest binds the raw build DLL before map-derived symbol lookup;
the worker image/module paths are checked. Gated non-atomic snapshots of the
same worker show W32WordBreakProc 0 before and 0 after the first successful
64-tab heap-sized EM_SETTABSTOPS. Guest records have zero errors, successful
set/reset returns 1, final phase 3 and normal exit. This fixes the previously
observed 0-to-1 corruption; no ensuing crash had been established or is claimed.

The runner now decodes each 46-byte guest record, in addition to snapshot and
exit checks. The completed run's retained report was independently decoded
with the same field offsets after adding this assertion; all five records pass.
No second product run is claimed for that offline evidence validation.

## Delivery

Coherent x86/x64 builds and all 16 Full gates passed.
[Receipt](../../../artifacts/mvdm-audit/shard-3/edit-tabstop-s19-results.json)
records original identities, complete baseline/current cases, guest snapshots
and reports, Full timings and all ten release hashes. Package
e012c0d5-1590-40a6-95a1-d09db981cbed is published only to worktree assets/release.
DOS4GW/NESticle remain owner-excluded, not counted as passing. No O:/winnt
write or push. S20/finding 49 follows.
