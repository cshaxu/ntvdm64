# T95 S7 BOP 5A Runtime Result 011

## Inputs

- Fresh r12 root:
  `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r12`.
- r12 executable SHA-256:
  `F62A05E339C0FB9F4B2BA8DA387621C877EB93547BF794D1F5AD53AAFAED12A6`.
- One immutable 15-second observation:
  `artifacts/analysis/t95-s7-runtime-trace-011-20260811-001`.
- Source caller: `dos/v86/cmd/keyb/keybi9c.asm:1118-1138`.
- Owner audit: `t95-s7-top-level-5a-5f-owner-audit-001.md`.

## Procedure and observed result

The generated r12 root was built once with the unchanged default-off generic
exception bridge and BOP catalogue listener, then observed once through the
existing watchdog wrapper.  `observation.json` records `watchdogTerminated`
as true, exit code 1, four existing transactions, and 40,025 BOP observations.

Immediately after the already admitted `50:42` read and `50:02` close, the
listener records `5F` at `8dc8:45a6` as pass-through.  It then records `5A`
at `0032:350b`; each instance is accepted with `next=350e` and `delta=00`.
The final observation consists of repeated identical `5A` calls until the
watchdog, not a new exception or a new BOP identity.

## Source interpretation

`keybi9c.asm` reaches this exact BOP after setting `CX=16` and `AX=0`, before
testing the keyboard hold-state flag.  This is an input/hold waiting loop.
The result is therefore consistent with a machine that has no completing
keyboard input/interrupt lifecycle in this profile.  It is not evidence that
the `5A` return ABI is wrong: the captured resume is exact and preserves the
required guest state.

The source audit's `5F` distinction remains material.  The preceding
historical `MS_bop_F` establishes keyboard/timer/IVT state and restarts the
historical interrupt controller.  Its pass-through cannot be upgraded to a
resume merely to escape the loop.  The trace does not prove that `5F` alone
caused the loop, but it establishes the next proof obligation: map every
`MS_bop_F` field and interrupt release to an existing Bochs CPU/PIC/PIT/
keyboard/IVT owner, classify it as native, composition input, or unavailable,
and only then decide whether a minimal composition action exists.

## Boundary decision

`5A` is now implemented and runtime-observed as a source-shaped continuation.
No host input API, idle event, VDD, PIF behavior, keyboard injection, timer,
PIC, IVT, or Bochs modification is claimed.  `5F` remains listener-only.
The next work is a read-only owner mapping, not another trace, BOP handler,
or device enablement.
