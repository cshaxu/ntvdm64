# M0 T440 S6: post-S5 responsiveness attribution

Date: 2026-10-09.  This investigation made no production change.

## Inputs and procedure

The published `O:\winnt\system32\ntvdm.exe` and
`assets\release\ntvdm.exe` matched SHA-256
`5F7AF08DE717DB2CA54184D2C3E70F17540EAE6301944ADD6FBFA6889EF90269`.
The current source 640x480 capture-to-worker/NTCON transport probe ran with
the S5 variable-record protocol.  A real retained Win3.1 Standard worker was
sampled after startup; a separate `run16 command` idle sample was retained as
a control.

## Measurements

| Observation | Result | Interpretation |
| --- | --- | --- |
| 64 current 640x480 graphics payload/dispatch samples | p50 405 us; p95 583 us; min 259 us; max 662 us | S5's 307,200-byte DIB travels as one payload record after `VIDEO_BEGIN`; graphics transport is not the multi-frame bottleneck that S4 identified. |
| Win3.1 Standard retained worker | NTVDM accrued 17.45 CPU seconds in 18 wall seconds after startup; NTCON accrued about 0.078 seconds | The material current cost is inside NTVDM/CCPU, not NTCON presentation. |
| Ordinary `run16 command` retained worker | NTVDM accrued 0.00 CPU seconds in an 8-second idle sample | The fault is reached by the Win3.1 workload, not a universal worker busy loop. |

The automatic launcher could not independently produce a durable desktop
witness; the Win3.1 sample is therefore a retained current worker observation,
not a new physical-desktop acceptance result.

## Source attribution

`src/mvdm/softpc.new/base/ccpu386/c_main.c` handles ring-0 opcode `F4` by
repeatedly checking the CCPU pending-event map, servicing timer/quick events,
then immediately looping again. `nt_timer.c` and the PIC route raise the
existing map through `c_cpu_interrupt`, but there is no OS wait at that HLT
boundary. The original `IdleEvent` in `nt_unix.c` has different accounting and
must not be reused.

The read-only SoftPC comparison's recorded M9 T85 repair waits on a dedicated
executor wake after the equivalent event-map/quick-event checks. It reports a
roughly 97% idle-core reduction. S7 may adopt only the same bounded host-wait
shape: retain CCPU's map and ordering, add no executor, timer, helper or guest
semantic.

## Follow-up

S7 owns the minimal CCPU HLT wake carrier, focused reset/IRQ/quick-event proof
and before/after CPU sampling. Win3.1 physical responsiveness remains an
owner-facing acceptance boundary after that repair.
