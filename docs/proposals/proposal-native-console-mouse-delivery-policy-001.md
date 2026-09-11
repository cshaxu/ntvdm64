# Proposal: Native Console mouse-delivery policy

## Withdrawal

Withdraw this proposal.  It inserted a non-OpenNT guest-input policy and its
readiness evidence conflated a stage that P8 did not emit with a real source
transition.

## Evidence and source-first audit

The direct original source owner is `softpc.new/host/src/nt_event.c`, which
enqueues movement, press and release.  The original guest mouse entry
`softpc.new/base/keymouse/mouse_io.c::mouse_io_interrupt` exits stream I/O
before servicing INT 33h.  The correct recovery boundary is therefore the
original host Console transition, not the guest event queue.

## Scope and acceptance

The P10 gate and its `MVDM-HOST-DIV-265` record are removed.  Follow-on work is
tracked by [Native Console OpenNT mouse-contract recovery](proposal-native-console-opennt-mouse-contract-recovery-001.md).
