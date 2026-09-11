# Proposal: Native Console mouse interaction contract

## Decision

Preserve the original OpenNT mouse route and do not introduce a coordinate
translation or button-policy change. P12 admits one narrowly evidenced CPU40
callback-entry repair: a direct registered guest callback must begin at its
registered offset, not at the historical BOP-adjusted address preceding it.
The global BOP adjustment and CCPU decoder remain unchanged.

## End-to-end contract

OpenNT's route is:

```text
Console Server mouse cell -> nt_event mouse FIFO -> nt_mouse coordinate map
-> INT 33h callback -> guest application
```

The imported `nt_event.c` assumes the first cell is in the visible VDM display
plane. Modern `ReadConsoleInputW` instead supplies a screen-buffer cell. A
scrolled console makes the difference observable: a physical cell at visible
row 11 can be reported as buffer row 8987. The unchanged original source then
clamps that row to the bottom of its 25-row display and delivers the wrong
guest `CX/DX` hit-test position.

The first P12 experiment tried to subtract `srWindow.Left/Top` from the
worker's standard output buffer. A bounded fixture disproved that shortcut:
the input record was delivered in a different Console-buffer view from the
one the worker queried. That experimental source change is withdrawn. P12
will instead establish how the original Console Server associated its mouse
input endpoint with the VDM output/presentation buffer before any adapter
change is admitted.

## Scope and verification

Button state is a separate contract. The original handoff to the guest
callback is `AX` reason, `BX` current button state, `CX/DX` absolute position,
and `SI/DI` motion. P12 must verify that a zero-button movement remains
zero-button at that handoff, and that press and release retain their distinct
states, including after the EDIT welcome modal has been dismissed. A visual
guest reaction is not by itself evidence that the host synthesized a press.

The P11 startup-timing experiment is withdrawn after owner runtime rejection;
the P7 native-input baseline is restored. The CPU40 callback-entry change is
limited to `jump_to_user_subroutine`, after evidence showed EDIT registers an
ordinary handler prologue at `371A:0223` while the stale CPU40 `-2` adjustment
selects `371A:0221` (`02 00`, a non-BOP instruction). It must be verified
with automatic zero-button and press/drag/release controls before staging.
