# M0 T313 S4 P3 — Device Endpoint Reconciliation

S4 selects complete original device packages and the original BIOS reset root.
It does not introduce another device dispatcher or repeat isolated fixture
implementations.  This record reconciles the bounded device evidence that
remains applicable after the fresh S4 x86 CCPU40 rebuild.

## Video and C-video

- Original `base/video` and `base/cvidc` bodies are selected directly by the
  fresh 403-edge candidate.
- The existing host-video contract verifies the source-shaped unbound
  presenter result and bound copied palette/rectangle notification order.
  It does not create a window or replace a controller:
  `m0-t310-s27-p2-host-video-formal-assembly-001.md`.
- The underlying controller package remains direct original source, with no
  S4 adapter video backend: `m0-t310-s26-video-controller-dispatch-closure-20260830.md`.

## Storage

- Original `base/disks` and host fixed/floppy bodies are selected directly.
- The retained bounded storage endpoint uses an impossible host name and
  proves a non-success public file-open result without mounting or modifying
  media.  The original no-fixed-disk zero-transfer direction and the
  condition-unselected remote-floppy product shell remain explicit rather
  than fabricated: `m0-t310-s22-host-storage-bindings-closure-20260830.md`.

## Communications and printer

- The complete original `base/comms` archive is selected directly.
- The retained bounded serial endpoint uses an impossible COM name and proves
  safe original host-open failure without changing a real device's DCB or
  performing I/O: `m0-t310-s24-host-comms-bindings-closure-20260830.md`.
- Parallel/printer/screen-print code is not silently substituted: under the
  selected profile it remains the original `PRINTER`-unselected unavailable
  path, as recorded by
  `m0-t310-s23-comms-controller-callbacks-closure-20260830.md`.

## Timer/PIT relation

The reset root starts `timer_init` before video and device post-initialization.
The source-coupled PIT/EOI body is compiled in the fresh selected graph;
existing T313 S3 evidence separately proves the original heartbeat suspension,
original PIC IRQ0 acknowledgment and the fact that a live period requires the
whole PIT/EOI cohort.  S4 does not claim a newly fabricated live timer loop.

## Result

Every S4 family now has both an original startup owner and one bounded positive
or explicit unavailable endpoint.  Full guest-visible display, real media I/O,
live serial traffic, printer product shell and a continuously running PIT are
intentionally deferred to S5/integration work; none is represented by a local
shim.
