# Proposal: Native Console OpenNT mouse-contract recovery

## Decision

Retain the original OpenNT mouse event, INT 33h callback and movement
semantics.  Recover the original transition from stream I/O to the Console
window/input contract when a guest first uses INT 33h, adapting only the
modern Console boundary where direct OpenNT console-server behavior is absent.

## Source-first audit

The original owner is `softpc.new/base/keymouse/mouse_io.c::mouse_io_interrupt`.
For `NTVDM`, it calls `disable_stream_io()` before servicing INT 33h.
`video.c::disable_stream_io` then calls
`host/src/nt_fulsc.c::host_disable_stream_io`, whose original body initializes
the Console, selects the display mode, enables mouse/window Console input and
restores the host mouse attachment.  The original `nt_event.c` queue then
delivers every movement, press and release to the original mouse interrupt.

P10 instead inserted a new policy before that queue.  It is withdrawn because
OpenNT and the comparison SoftPC machine both treat movement with buttons up
as real mouse input.  Its stage 8 witness also conflicted with the P8 observer:
the observer treated stage 8 as readiness even though P8 did not emit it.

The visible system-pointer branch maps Console cells to the absolute callback
position. That `CX/DX` pair is the authoritative guest hit-test coordinate:
modern Console movement must preserve it rather than substitute relative
motion. The separate `SI/DI` movement fields remain part of the original ABI,
but are not an admitted repair point without source evidence that a particular
original caller requires them for this path.

## Scope and acceptance

First restore P10's source change.  Then make the observer wait for a real,
default-off fact at the original stream-to-window transition, not a synthetic
mouse stage.  If modern `conhost` or Windows Terminal lacks an input/pointer
facility assumed by the original owner, add the smallest host-side facade at
that boundary. Preserve the source-owned absolute-coordinate conversion. No
guest or INT 33h behavior may be filtered, invented or redirected.
