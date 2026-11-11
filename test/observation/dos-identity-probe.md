# DOS instance identity diagnostic

Owner-requested side investigation, 2026-11-10. No production fix or publication.
This probe preserves the original observer algorithm and wraps its create and
terminate callbacks. It logs only PSP/header addresses, CCPU CR0/CR3, translated
PSP address and selected observer occurrence/direct identity. The diagnostic
translation uses existing `xtrn2phy`; this is an instrumented execution, not a
claim that instrumentation has zero timing or translation-cache effect.

## Reproduction

- `prepare-dos-identity-probe.py` composes a diagnostic worker from cached
  libraries and an explicit replacement observer object, without editing the
  shared Ninja cache or product source. Run its generated build.cmd in an owned
  background process with the recorded x86 MSVC developer environment.
- `verify-dos-identity-pair.ps1 -DosOnly 1 -Run b` and `-DosOnly 0 -Run a` use
  independent O:/tmp/i311 and i310 copies, disable both addon device entries,
  and reuse the existing private-desktop Win31 matrix runner. The independent
  Win16 controller opens two interactive COMMAND prompts and types A.COM then
  B.COM. The independent DOS fixtures report role, PSP and INT2F/1683 VM ID.
  A is released first; B remains live until its separate release file appears.
- `analyze-dos-identity-probe.py` asserts the actual collision against both
  guest witnesses and host event records. It writes findings.json with evidence
  hashes under build/side-dos-identity.

## Actual results

| DOSONLY | Worker | A/B PSP | CR3 | A physical PSP | B physical PSP |
| --- | --- | --- | --- | --- | --- |
| 1 | 2336 | 10DF | 001DC000 | 00292DF0 | 00333DF0 |
| 0 | 8208 | 10DB | 001DC000 | 00292DB0 | 00333DB0 |

Both fixtures self-reported distinct guest VM IDs 2 and 3. In both runs A
created occurrence 5; B's creation reused the same PSP slot and replaced it
with occurrence 7. A's termination had A's original physical mapping, but
selected occurrence 7. B's later termination had B's physical mapping and no
remaining active slot. This directly establishes the PSP-only observer
collision; no inference from displayed names is needed.

In DOSONLY=0, both inner Run16s existed before release (A PID 39096, B PID 600).
After A returned, only A's Run16 remained: B's waiter was retired incorrectly.
This matches the existing BaseSrv completion path's last-BUSY selection.
The second COMMAND creation was also assigned direct=2, despite being another
guest prompt: the single ambient direct label is not a per-context association.

## Limits and failed checks

The initial dos1 attempt was rejected before launch because the diagnostic
package manifest still described the original worker. The next run stamped
and hashed the diagnostic image in its own manifest; production assets stayed
untouched.

Both runs captured both child creation and normal DOS termination events and
both END witnesses. The Win16 controller did not complete Windows shutdown;
the existing matrix observer reached its diagnostic deadline and cleaned its
owned runtime. These are successful bug reproductions, NOT passing integration
or normal-shutdown acceptance results. No EDIT mouse/UI acceptance is claimed.

Identical CR3 values disprove CR3+PSP as sufficient identity in these runs.
Distinct physical mappings identify the two objects here, but do not prove
that physical addresses survive arbitrary guest paging/remapping. This probe
does not authorize adopting physical addresses as permanent identifiers, nor
does it prove a driver-free lifetime key for every configuration.

The original guest binaries and installed O:/w31 remain unchanged. All added
assembly is independent test code, not a rebuilt original guest. Test sources
are retained for review; there is no fix commit or default release.
