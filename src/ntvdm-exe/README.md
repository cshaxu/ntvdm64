# NTVDM worker owner

The Console input adapter bounds each original five-record read by available
relative-mouse FIFO capacity under the original ICA lock. Only the event
thread produces these samples; the CPU consumer frees slots. At capacity it
leaves records with the frontend, yields briefly and returns to the original
alertable suspend/input wait. It does not discard motion/button edges, merge
direction changes, enlarge the queue or change original IRQ timing. See
tests/observation/verify-window-mouse-pressure.ps1 and the S11 evidence ledger.

Original mouse IRQ cancellation flushes pending samples and button/delta
state, but does not disconnect the frontend input source. The bridge retains
the last accepted ENTER/LEAVE ownership so queued upstream movement or source
retirement remains valid after guest reset or original input suspension.
Successful DOS frontend deactivation is a distinct boundary: the frontend
discards its DOS mouse records, and console_client clears the worker route
only after acknowledging that handoff. Failed deactivation preserves it.

This component owns worker-local bootstrap, session, guest-memory and thread
bindings around the original MVDM execution core. It does not own frontend
Console/Window presentation or broker record policy.

`package_layout.c` and its header configure worker-owned media roots and validate
the original COMMAND configuration limits. They were moved from product-package
without changing path or failure behavior; they are not shared product ABI.

The win32 directory keeps private declarations beside their implementation:
environment projection, thread alert, WOW hard-error presentation and the scoped
original-host CRT redirect. Their historical divergence records remain indexed
in opennt-abi/host-compat/README.md. Shared original declarations and genuinely
multi-owner bindings remain separate; this relocation does not introduce a
generic compatibility library or modify original guest media.
