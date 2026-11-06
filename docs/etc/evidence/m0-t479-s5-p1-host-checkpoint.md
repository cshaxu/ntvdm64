# T479 S5 P1 host continuation checkpoint

This is an intermediate delivery, not S5 closure or complete interactive guest
multitasking. Win3.1 is the current priority; Win95 work remains deferred.

## Reviewed ownership and entry

- `nt_bop.c` adds one seven-line registered mirror seam (DIV-871), before
  original COMMAND dispatch. Only08/0A, software V86 and a registered bridge
  select it. CCPU CALLF owns the real return frame; ordinary dispatch remains.
- The independent addon obtains the real VMM handle and client frame before
  admitting a request. Its semaphore blocks that VM's command callback, not
  the host CPU interpreter. The existing S4 published addon is unchanged.
- Capture owns command/environment/handles/directory; the original environment
  transform is reused. No borrowed guest pointer survives scheduling. Per-request
  continuation and generation replace the old singleton only on this path.
- The executor owns launch/text/provider references and joins producers before
  frontend registry destruction. NTSRV still owns native-chain lifecycle.
  Same-number generation reuse waits off CPU for older frontend retirement.
- Frontend video validation moves from NTCON to common/frontend-base for shared
  use. NTCON and VM text consumers retain their own presentation ownership.
  Explicit publication commits only after guest acknowledgement; failures roll
  back the proposed text state. Unsupported operations are not successful stubs.

## Evidence and publication

[Hash-bound diagnostic evidence](m0-t479-s5-continuation-diagnostic.json) and
[the implementation record](../operations/m0-t479-s5-command-continuation.md)
retain failures as well as subsequent passes.

- Cached complete x86/x64 builds and shared suites: run155.
- Focused execution, ingress, capture, provider and text suites:151/157.
- Full product gates: release160,16/16 selected gates; external DOS4GW fixture
  skipped explicitly. All ten images preserved their verified package hashes.
- Actual Win3.1 command continuations:159; command-in-flight retirement:161;
  real handle reuse and second command:163; pre-cleanup CMD absence:164.
- Both controlled native process exit orders:166/167; both54:08 forms return
  37/38,54:0A returns0 with translated input,320 callback-state checks per run.
- Documentation governance:171 passes after fixing historical CURRENT encoding
  and the two links affected by moving video code. CURRENT is below16KiB.
- Publication172 succeeds at assets/release and O:/winnt/system32 with complete
  ten-image hashes and hook license. Package: a901a87d-ff85-4e4f-a300-74a0cfc1d781.
  The normal NTVDM input hash is EFAA3F1FC1300CA7377D59CE64CCBB11191D2F833BB63BC5E3E7CC15E0547959;
  the staged hash is5F27BA44F1D2B0555BFCAC8BE2678FAC096E3254D39EC80FBADDDDA66C8A21A1.
  Package stamping accounts for that distinction. The diagnostic worker is not
  a publication input. Original guest files and the published addon are unchanged.

## Remaining boundaries

Physical keyboard, exact font application, complete I/O, returned keys and
nested acceptance remain S6 work. The source addon has broader S5 capability
than the published S4 candidate; no new addon publication is claimed here.
Forced overlap of real VMM reuse with unfinished native retirement has host
unit evidence, not a forced real-guest witness. Fatal resource exhaustion and
all allocation/retirement failures are not exhaustively fault-injected.
Native process exit order is not a promise about guest callback scheduling.
S5 remains active pending final scope/delivery review; this checkpoint cannot
be used as a claim that the whole T479 objective is complete.
