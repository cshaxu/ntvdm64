# Proposal — NTVDM execution-performance recovery

## Status and objective

Owner-requested planning dated 2026-10-04 initially placed this unnumbered
candidate at the head. The owner's subsequent same-day ordering direction
places it second, after worker control/lifecycle/naming unification. It does
not admit a numeric T, interrupt M0 T427 S4, or
claim that the diagnosed repairs are already delivered. Admission must refresh
the source, build and runtime identities, select bounded S work, and record
the resulting evidence in [CURRENT](../states/CURRENT.md) under the
[Execution Rules](../rules/EXECUTION.md).

Owner subsequently closes T428 and admits this now-head package on 2026-10-04.
The live T429 S1 packet is solely in CURRENT; the sequence below remains its
supporting proposal, not simultaneous admission of S2-S4. The preceding
planning dates/positions describe history, not the current active packet.

Recover interactive DOS performance in the selected Win32/x86 CCPU40 product:
`EDIT.COM` startup and mouse response must not be materially slower merely
because `ntvdm-exe` adds project diagnostic work or avoidable worker transport
overhead around the same CCPU core used by SoftPC. Preserve original guest
execution, DOS/Win16 semantics, ordered input, explicit video publication and
the existing worker/frontend ownership boundaries.

The first source audit identifies an unconditional per-decoded-instruction
call from the selected CCPU loop to
`mvdm_softpc_report_nt_transition`. Even when its optional trace path is not
configured, that routine performs TLS, last-error and state work. The
comparison SoftPC CCPU route has no equivalent unconditional observer. This
is the first repair hypothesis, not a completed causal claim. The candidate
also measures the actual Window-to-mouse-IRQ and producer-to-presentation
paths before changing batching, backpressure or video transport.

## Bounded findings and ownership

| Current finding | Required disposition |
| --- | --- |
| `softpc.new/base/ccpu386/c_main.c` calls `mvdm_softpc_report_nt_transition` at `DECODE` for every guest instruction under `NTVDM`. | Remove it from the production inner loop. Retain tracing only in a separately selected diagnostic build or at an existing coarse boundary, default-off with no normal-path per-instruction call. Preserve CCPU instruction semantics and original mirror ownership; register any minimal divergence. |
| `ntvdm-exe/softpc/mvdm_softpc_termination.c` reads optional trace configuration once but still updates TLS and last-error state on every call. | Keep the diagnostic capability only where it is not a release executor cost. Do not replace it with a branch or function call that remains in every decoded instruction. |
| Window input crosses `ntcon` queue, the direct worker I/O named pipe, `ntvdm` event processing and the relative-mouse queue before the original mouse interrupt route. `console_compat.c` currently bounds a read to five raw records and sleeps for one millisecond only when its consumer capacity is exhausted. | Establish measured queue depth, batch and latency evidence. Change batching/backpressure only if the evidence identifies it as a material source of delay; preserve input order, suspend/close behavior and original mouse IRQ ownership. No polling cadence is introduced merely for symmetry with SoftPC. |
| Text video copies and synchronously transfers explicit frames from `ntvdm-exe` through NTCON before reconstruction/presentation. | Measure producer, transfer and presentation timing separately. Do not revive receiver/transport frame deduplication: explicit published frames remain observable. If surplus work is proven, repair the source-owned producer trigger or a separately justified copy/transport cost without filtering emitted frames. |
| SoftPC receives window input in-process and wakes its executor directly; NTVDM has the extra worker boundary. | Use it only as a behavioral/performance comparator. Do not import SoftPC lifecycle, scheduler or platform code, add a second executor, or collapse the authenticated worker/frontend boundary. |

`ntvwm` smoothness is evidence that the Window/raw-input component alone is
not sufficient to explain the symptom. It is not a substitute for exercising
the complete `ntcon` → `ntvdm-exe` → CCPU route used by DOS `EDIT.COM`.

## Proposed implementation sequence

| Candidate S | Scope | Exit condition |
| --- | --- | --- |
| S1 | Freeze baseline identities and reproduce slow `EDIT.COM` startup and continuous mouse movement against the comparable SoftPC workload. Add default-off, aggregate boundary measurement only: raw-input receipt, frontend delivery, worker input read, mouse submission/IRQ consumption, text publication, transfer and present completion. | A reproducible baseline has elapsed-time/latency distributions, queue/batch counts and a control case; instrumentation has no per-instruction logging or production behavior change when disabled. |
| S2 | Remove or compile-select the per-instruction NT transition tracing from the release CCPU path while retaining bounded diagnostic tracing outside the hot loop. | The selected release object has no per-decode trace transition call; CCPU/DOS behavior and selected diagnostic capability have focused evidence. Benchmark comparison demonstrates the expected startup/execution improvement or records a contrary result. |
| S3 | Use S1/S2 evidence to repair at most the proven input-path or producer-side video-path bottleneck. | Each change has exact ownership, ordering/lifetime proof, focused negative cases and before/after measured benefit. An unproven path remains unchanged and is recorded as a limit. |
| S4 | Integrate performance and semantic regressions across Console and Window `COMMAND`/`EDIT`, nested return, continuous input, video publication, lifecycle and selected WOW frontiers. | The performance claim is backed by repeatable measurements and retained product tests; no forbidden scheduler, guest-media or wire-policy change is hidden in the package. |

## Verification and exit criteria

Use the formal MSVC Win32/x86 `/MT` CCPU40 graph and an identity-checked
runtime package. Record hardware/desktop or RDP context, frame geometry,
input method, exact command line, source/build hashes, warm-up policy,
iteration count and elapsed time. Compare medians and tail latency rather than
one run. The test matrix includes `EDIT.COM` launch, continuous relative
mouse movement and click/selection behavior, `EDIT` exit then `MEM`, Console
and Window execution, nested COMMAND return, input under concurrent text
updates, and an unaffected Win16/WOW smoke frontier.

Review the release binary/object or map evidence to prove the inner-loop
observer is absent, then run focused CCPU/mouse/input/video fixtures and the
retained product Console/Window/WOW/lifecycle gates. A performance regression,
unavailable physical input test, or non-reproducible result remains explicit
evidence rather than a pass.

No guest or firmware mutation, CCPU algorithm rewrite, CPU30 route, new
worker/helper, generic scheduler, authenticated-wire redesign, task lifecycle
policy change, receiver-side frame filtering, or import of SoftPC application
code is included. Any need for one of those changes pauses this candidate for
renewed admission. This proposal itself is planning only.
