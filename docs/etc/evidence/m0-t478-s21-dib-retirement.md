# T478 S21 — original DIB retirement boundaries

Baseline: e95b2b739. Finding 50 retains original failure/count limitations;
normal nested retirement is verified. No product changes, new retry contract
or claim that all failed selector restoration is safe.

## Original contract and current selection

Five current retirement bodies (find/lock, check/free, free handle, free node,
FindAndReleaseDib) are token-identical to both original families after removing
comments/#line. Current WOW32 map resolves the public retirement functions to
wdib.obj. Repeated CreateDC for the same selector returns the existing DC and
increments di_lockcount. Ordinary DeleteDC consumes a nested reference; the
last reference invokes selector restoration and resource retirement.

The original guest contract was read only to interpret this host entry:
3ginterf.asm GlobalReAlloc calls FindAndReleaseDib and continues without testing
its Boolean result; GlobalFree branches on it and may proceed with its own
free logic. Therefore FALSE is not a universal promise that a whole guest
operation aborted without changes. This is not a guest audit or modification.
It would be unsound to invent such a contract solely from the host return.

The original last-reference decrement occurs before restoration; failed restore
retains the node with count zero. Original FindAndReleaseDib with a nested
count decrements and returns FALSE. WG32DeleteDC also falls back to native
DeleteDC on FALSE, which combines not-found and failed-restoration cases.
These remain explicit original failure/lifetime boundaries. No supported
end-to-end repeat-failure/reentry scenario was demonstrated that requires a
new transactional owner, retry policy or stronger task survival guarantee.
Do not claim these transitions are universally safe or newly repaired.

## Verification

[Fixture](../../../test/integration/wow_dib_retirement_test.c),
[runner](../../../test/integration/verify-wow-dib-retirement.ps1), and
[receipt](../../../artifacts/mvdm-audit/shard-3/dib-retirement-s21-results.json).
The runner extracts the five complete current bodies and original DIBINFO and
FindAndReleaseDib argument declarations, compiling x86 with MSVC /W4 /WX.
Real native DC/file/section/DIB/view ownership is exercised; selector restore
and VDM removal are explicit controlled boundaries, not a real guest callback.

All 57 checks pass. Nested find/lock gives count two, first ordinary close leaves
one without restoration, and final close restores once and removes the node
and real GDI resources. Single-reference FindAndReleaseDib also retires normally.
The normal original DeleteObject-before-DeleteDC sequence leaves no GDI resource
in this witness; it is not changed based on a speculative selected-bitmap leak.

Controlled failed restore records node retained/count zero. Controlled nested
FindAndReleaseDib records FALSE/count one. The fixture then explicitly retires
its own state through the original free helper; this is not a production retry
or evidence that the outer WG32DeleteDC fallback preserves the live DC. No
forced task death, naturally occurring restore failure or arbitrary reentry is
claimed. Initial fixture compilation needed its missing WORD-based HAND16
prerequisite; no product ABI was changed.

All ten release hashes match S20 package
e6000fef-d461-4e0f-b17c-decb791bd449. Its 16/16 Full result remains applicable
to unchanged product bytes. No new product build/publication or Full run is
claimed; only the new bounded host fixture was compiled/run. No O:/winnt write,
push, guest modification or external repository write. S22/finding 51 follows.
