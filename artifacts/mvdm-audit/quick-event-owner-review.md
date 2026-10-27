# Original NT quick-event owner and actual-object witness

This S10 audit/test checkpoint compares against `308d2e0b7`; no runtime,
mirror, guest or release binary is changed. The
[three dispositions](quick-event-owner-dispositions.csv) exclude the
unselected generic implementation and partially verify the selected NT
provider and its signed 64-bit handle declaration. They do not establish
whole-file or published-package acceptance.

## Original ownership, rather than a guessed generic-scheduler defect

Original `softpc.new/base/system/sources:59` selects `qevnt.c`, not
`quick_ev.c`. Current Ninja selects `obj/system/qevnt.obj` from that same
original file and does not select the generic source. The NT provider
explicitly identifies itself as the replacement for generic quick events;
its original header says unused tick events are unsupported. Actual callers
include `nt_timer.c::host_timer_init`/reset initialization, timer batching,
keyboard deferred refill, printer/serial delays and `nt_eoi.c` cancellation.
Their specific device and guest paths remain separate verification work.

The generic source's small-number handle sequence is therefore not evidence
of a defect in the selected NT provider. Reviving it would introduce a
second scheduler and alter the NT owner/handle/calibration contract. Its
file-only exclusion follows this positive original-owner trace, not merely
its absence from a graph. It remains retained comparison source; no code or
guest capability is deleted by the disposition.

The NT code uses an original local `QEVHANDLE` union: a queue-entry pointer
plus a sequence ID, exposed as `LONGLONG`. Deletion scans the owned queue
for both values before retiring an entry; a fabricated address is not
dereferenced as an external NT private object. That union belongs to the
original provider, not to modern Windows. This audit is x86: it does not
authorize compiling the pointer-containing NT implementation as native x64.

The source comment's historical x86-immediate description is controlled by
`MONITOR`, not simply the host machine being x86. The selected CPU40
non-MONITOR object queues events; the controlled zero-delay test confirms
deferred delivery. `add_q_event_i` also deliberately interprets its argument
as time, not raw instruction-jump counts. These original NT distinctions
must not be overwritten with generic scheduler assumptions.

## Bounded executable evidence

[The checked-in runner](../../tests/observation/verify-qevnt-object-contract.ps1)
builds [a fixture](../../tests/mvdm-host/qevnt_object_contract_fixture.c)
against the **actual selected cached `qevnt.obj`**, not a copied queue
algorithm. It directly includes the original `host_qev.h`. Controlled
allocator, clock, ICA-lock and CPU-count boundaries make the normal and
negative transitions deterministic; they are not production host bindings.

The final [receipt](quick-event-owner-fixture-result.json) records all seven
input identities before and after the run, actual compiler/test commands,
PIDs, exit codes and fixture identity. The final build is warning-free.
Its 37 assertions and eight callbacks verify:

* Time ordering and distinct handles; head/middle/tail cancellation; an
  empty queue and repeated, zero, reserved, malformed and wrong-ID handles.
* Unlocked callback invocation, callback-time cancellation and scheduling,
  and no residual queue allocations after dispatch. The original source
  separately shows removal/free before invocation; the fixture does not
  instrument the exact instant of that free.
* Initialization preserving an existing queue rather than inventing blanket
  cancellation; the original deferred zero-delay behavior.
* Allocation-failure token 1 with both empty and populated queues, preserving
  the previous queue and CPU-count state.
* The original eight-byte handle ABI through real calls; a deliberately
  wrong four-byte declaration is [rejected with C2118](quick-event-owner-abi-negative.txt).

The first 36-assertion exploration used a local equivalent handle typedef;
the final 37-assertion version instead includes the original declaration,
adds populated-queue allocation failure and records complete input hashes.
Only the final version is used for this checkpoint's dispositions.

## Status and limits

`qevnt.c` and `host_qev.h` become **partial**, not full. Exact original
source is retained: [349/349 code lines and 1/1 declaration line](quick-event-owner-code-retention.csv),
respectively. The recorded 100% metric is source retention, **not** whole-file
behavioral restoration or test coverage. Full calibration, late-event
adjustment, real ICA concurrency, actual device/guest dispatch and reset/
retirement, allocator-address reuse and published-image/object equivalence
remain to verify in S13. No original/source correction is admitted by this
test result and no mirror exception is introduced.

Counts become pending 2,145, excluded 2,257, partial 230, full 52.
The earlier 454-row native pending census is a historical snapshot: one of
its rows (`qevnt.c`) now has this partial proof. Its fixed-cohort identity
checker is not a current pending-count gate after these legitimate changes.
The remaining proposals and main-session native review remain open.

## Reproduction

The receipt and three-row ledger checkpoint below describe the historical
`e94f3fb48` delivery. The CLI now delegates to shared test plumbing; its
[fresh 37-assertion control](ios-owner-qev-control-result.json) and the
current six-row ledger gate are recorded in the [IOS checkpoint](ios-owner-review.md).
Do not use this fixed historical delta/count check as a current whole-ledger
gate after later legitimate dispositions, or overwrite its original receipt.

Launch in the required owned background context, using modern `pwsh`:

```text
tests/observation/verify-qevnt-object-contract.ps1 -BuildRoot build/t473-s10/qevnt-object-contract-r3
python -B -X utf8 artifacts/mvdm-audit/quick-event-owner-checkpoint.py build/t473-s10/qevnt-object-contract-r3/checkpoint.json
```

The [checkpoint result](quick-event-owner-verification.json) passed with
exact three-row delta, untouched unrelated shard rows, original/frozen
identities, source-retention metrics, actual-object/ABI receipt, original NT
selection and current drift checks. Native fixture PID 12180 exited 0;
the deliberately invalid ABI compile PID 33296 exited 2 as expected;
checkpoint PID 6868 exited 0. This changes tests/audit evidence only and
does not require or claim a new product publication or S10/T closure.

Documentation governance (PID 5756), relative links (PID 13136) and
`git diff --check` passed; CURRENT is 15,062 bytes. Other sessions' setup/
video candidates and their test edits remain outside this delivery.
