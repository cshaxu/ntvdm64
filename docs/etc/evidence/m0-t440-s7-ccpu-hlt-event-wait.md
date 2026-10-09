# M0 T440 S7 — CCPU HLT event wait

## Scope

S6 established that the post-S5 graphics protocol was not the retained
Windows 3.1 Standard bottleneck: a 640x480 DIB is delivered in one 307,200
byte payload record with a 64-sample p50 of 405 microseconds and p95 of 583
microseconds. The retained NTVDM process consumed approximately one CPU core
while NTCON and NTSRV remained nearly idle.

S7 therefore audits the CCPU40 `HLT` body rather than changing frame cadence,
mouse processing, guest binaries, or the graphics transport.

## Source conclusion

`src/mvdm/softpc.new/base/ccpu386/c_main.c` executes the original ring-0 HLT
loop by repeatedly checking the CCPU event map, timer work, and quick-event
count. With no event it continued immediately; it had no host wait. The
standalone host already raises the original CCPU events through
`c_cpu_interrupt()` for reset, timer, hardware interrupt, and I/O work.

The repair adds a process-local auto-reset wake carrier. It is created before
the worker starts host producers, signalled after the original
`c_cpu_interrupt()` event-map update, and waited only after the HLT loop has
performed its original timer/quick-event processing and observes both:

- no CCPU event-map bit; and
- no scheduled quick-event instruction count.

It does not replace CCPU event bits, reorder interrupt handling, synthesize
timer work, or alter quick-event deadlines. This is registered as
`MVDM-HOST-DIV-330`.

## Focused proof and measurement

`ccpu-halt-reset-test.exe` executes a synthetic guest `HLT`, waits 1.5 seconds
on a separate producer, then raises the original reset event. The test proves
the CPU restarts at the original reset vector (`AX=BEEF`), not the instruction
after HLT. Immediately before reset, `c_cpu_q_ev_get_count()` was zero.

For an exact old/new comparison, a build-only test carrier emulated the
pre-DIV-215 empty HLT iteration. It is not source-controlled and is never
linked into a product image. CPU times sampled from the same fixture process:

| Carrier | CPU at 750 ms | CPU at 1250 ms | 500 ms steady-window CPU |
| --- | ---: | ---: | ---: |
| Pre-fix busy loop | 0.703125 s | 1.218750 s | 0.515625 s |
| Event wait | 0.484375 s | 0.484375 s | 0.000000 s |

Both variants exited 0 and printed `original-CCPU HALT RESET: AX=beef
producer=0 quick=0`. The initial CPU cost is CCPU/SAS setup and is excluded
from the steady window.

## Boundary

This establishes the idle-HLT bottleneck and its repair. It does **not** claim
that a guest actively executing Windows 3.1 GUI work will become faster:
those periods do not necessarily execute HLT and require separate product-level
measurement. A reversible diagnostic replacement confirmed that distinction:
after a 15-second Win3.1 Standard warm-up, NTVDM consumed 9.671875 CPU seconds
over the following 10 seconds. The production image was then restored to its
prior SHA-256 `5F7AF08DE717DB2CA54184D2C3E70F17540EAE6301944ADD6FBFA6889EF90269`.
That busy GUI interval is not an HLT-idle case, so it is not a reason to remove
the correct idle repair or to claim a throughput improvement from it.
