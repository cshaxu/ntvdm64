# T425 S11 typeahead verification and closure audit

## Scope and baseline

Owner directs completion of typeahead then T425 closure. The one-minute goal
and extreme 1000-motion injection are withdrawn; passed 200-motion checks
remain. WINMINE must reach its interface; SOL/WRITE retain established frontiers.
S10's single-instance investigation concludes at pushed 15143473a.

This is test-only work. No production, guest, library, wire or observer-C
change. Sealed S9/r022 runtime still equals O:/winnt, all eight hashes checked.
MSVC14.43 / SDK22621 / Win32 x86 /MT / CCPU40, APP0.0.425, control37 and I/O25
remain selected. S9/r035-r036 each passed 17 Console, 17 Window and three WOW
frontiers. Their unchanged-input build/runtime proofs are reused by identity,
not represented as new executions. No product rebuild/redeployment is needed.

## Diagnosis and test repair

### Retained S11 admission brief

| Field | Contract |
| --- | --- |
| Identifier Mode | M0 T425 S11, Ordinary Mode. |
| Admission And Approval | Owner: continue until T425 closure criteria; repair supplemental typeahead evidence without serializing input; retain VGA defect unless SoftPC repaired it. |
| Objective | Prove continuous nested DOS/native execution, MEM output/count/order and completion without screen history. |
| Non-goals | No production/protocol/guest change, consumption waits, helper or expanded WOW/mouse gate. |
| Reference Baseline | 15143473a, sealed S9/r022 runtime, r035-r036 full passes and r037-r039 retained failures. |
| Files And ABI Surface | Test scripts, CURRENT, plan, evidence and closure; no ABI change. |
| Applicable Rules | Execution, architecture, coding, document and source-policy authorities. |
| Verification | Console/Window continuous cases, assertion negatives, unchanged hashes, cleanup, governance/link/diff checks. |
| Expected Markers | Complete fresh ordered reports, exact count/code, zero milestone waits and no owned-process leaks. |
| Asset Needs | Existing x86 CCPU40 runtime and sealed observer; outputs only build/M0-T425/S11. |
| Reporting Requirements | Separate observation limitations from product failures; preserve failed and unrelated evidence. |
| Stop Conditions | Proven production regression or changed semantics requires bounded root-cause review, not weakened assertions. |
| Exit Criteria | Selected cases/negatives pass, reviewed commit/push and owner-authorized T closure. |
| Original Owner Request | Continue to T425 closure standard; only typeahead remains, 1000-motion gate cancelled. |
| Similar-Issue Sweep | Both modes, nested DOS, DOS/native/DOS and stale/missing/duplicate/order evidence. |

The former assertion attempted contiguous screen-history reconstruction.
Finite projected pages need not overlap. Two actual MEM executions can also
have identical numeric output, so numerical uniqueness alone is insufficient.

Exact original input sequences, pacing and zero inter-line delay are preserved:

```text
command CR command CR mem CR exit CR mem CR exit CR mem CR exit CR
cmd.exe /d CR run16 mem CR exit CR mem CR exit CR
```

No intermediate consumption wait, extra command, redirect, BAT wrapper or
production hook is used in the final test. Initial prompt readiness remains
the original prerequisite. Every actual report has milestone-waits=0.

tests/observation/typeahead_witness.ps1 reads snapshots independently:

- Exact drive-qualified command, followed by all six MEM output markers once,
  executable-size field and the final high-memory residency line of this
  frozen configuration; command echo alone is insufficient.
- Reject pre-input stale reports, duplicate reports within a page, extra,
  missing, premature, misordered and incomplete reports.
- Repeated sampling is not another execution. `run16 mem` and `mem` distinguish
  equal native/DOS results. Nested DOS retains its established distinct-depth
  report witness; MEM's original numeric defect is not interpreted as repaired.
- Require each selected complete result to have been observed before the next
  MEM is submitted. This is post-run attribution, never a wait that serializes
  delivery. Ambiguous evidence fails, rather than guessing.
- Retain actual root exit/code 1, complete delivery, unexpected resolution-error
  checks and identity-checked cleanup. CMD banner presence uses observed pages,
  not invented scroll history. Output counts/order use explicit witnesses.

This is a bounded proof of the selected sequences, not an ability to count
arbitrarily fast identical MEM repeats at one unchanged depth from snapshots.
Default 17-case inputs/assertions and normal rendering coverage are unchanged.
verify-typeahead.ps1 runs both modes serially, restores environment, removes Z:
in finally, verifies exact case count and checks eight runtime hashes.

## Commands and actual results

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests/observation/typeahead_witness_test.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File tests/observation/verify-typeahead.ps1 -RuntimeRoot build/M0-T425/S9/r022/runtime -Observer build/M0-T425/S9/r033/console-startup-observer.exe -LogRoot build/M0-T425/S11/r006
```

Unit: identical native/DOS output and repeated samples accepted; eight stale,
missing, reordered, duplicate, premature, incomplete, extra and ambiguous
negatives rejected. All four modified/new PowerShell scripts parse successfully.

| Final live r006 | Root completion | Complete MEM count | First snapshot lines |
| --- | --- | --- | --- |
| Console nested DOS | exited, 1 | 3 | 4, 6, 8 |
| Console DOS/native/DOS | exited, 1 | 2 | 3, 5 |
| Window nested DOS | exited, 1 | 3 | 4, 6, 9 (final page) |
| Window DOS/native/DOS | exited, 1 | 2 | 3, 5 |

Console pair 16582 ms, Window pair 17111 ms including explicit test cleanup.
No owned processes remain, Z: is unmapped, runtime manifest remains identical.
r003/r004 independently passed all four scenarios before final attribution and
case-count strengthening; those earlier scripts are not claimed unchanged.
Final r006 is one full passing invocation with no retries.

Failed predecessors remain non-pass:

- r001 exploratory BAT/redirection alternative times out (0x53504354) after a
  begin marker. This added scenario is withdrawn, not used as proof for the
  unchanged commands. No root cause or repaired BAT capability is claimed;
  raw watchdog evidence is retained and this unproved boundary is in TODO.
- r002 passes Console; Window target exits correctly, but a new assertion
  incorrectly treats GetNumberOfConsoleInputEvents as remaining characters.
  It counts all host records. That added assertion is removed; it was never
  part of the original test contract.
- r005 passes both Console cases; a new runner collection check misreads
  Windows PowerShell 5.1 JSON-array wrapping. Final runner corrects assignment,
  retaining strict case count. This is not a product repair.
- S9/r037-r039 original merge failures remain historical failures, superseded
  by corrected same-input live evidence, not retroactively passed.

## Source identities and conditional VGA disposition

| Input | SHA-256 |
| --- | --- |
| typeahead_witness.ps1 | 20101941712BA98995D80DDEBA47D8602EA5A67A6321547FBACB903D0B2E59F0 |
| typeahead_witness_test.ps1 | E51297BD2734D655411247F621EE0BE8DDFE7E4878732372EABEEF0448DBE649 |
| verify-typeahead.ps1 | 8C22D08A4A7C78B0572DE1DB9AD6350D9B0C81D91D4EA8428CC643010D624B31 |
| Verify-CommandExitStatus.ps1 | 4F2D0A533C61CABD004BF80ED0A52B97747B2CCAA53ED87CD8975439B78802CA |
| reused console-startup-observer.exe | 7558E352B2F837C4C395175335DB852D31DA97D0741700E5867293B56F03C252 |

Observer C, input-milestone and snapshot-header hashes match the S9/r033 seal:
BBCFBCB1BA4DD1308B7ED620B95DE90D843A483B815BC6CBA1B298929B4ED372,
F558483D4DC2883834663908FB8F31DBCEB82E1000A05F9A77164DC8C2DA54E2,
D1EE0BC553F435A95B6100E085ABB7FF5569E7728CC3463659E28792E6883C34.

Read-only SoftPC current vga_prts.c still checks old cursor_off and updates
the register only when start bits change. Its only dirty path is an unrelated
snapshot asset, not this repair. Hash
6BCFC24718A4DCA2B081B2B994645E4D58AB4F5366C107BDCA2A90848E69244F;
our unchanged mirror
7D30109853FA444935BA6FD1E22CCBE1B197DB4A35C20A309D0DE098F2101333.
Owner's conditional instruction retains this original defect; no import or
workaround. No other project is changed.

The selected typeahead row is now verified, not deferred. Physical RDP and
broader WOW usability retain prior explicit boundaries. Final governance/link
checks and actual diff review precede S11/T closure commit and push. Owner
approves including reviewed other-session Queue/proposal planning; it does not
expand S11 or admit the next T within this closure delivery.
