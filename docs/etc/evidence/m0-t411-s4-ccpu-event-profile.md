# M0 T411 S4 — CCPU event-profile disposition

## Result

`c_main.c` has five producers (reset, timer, SIGIO, hardware interrupt and
SAD) and two consumer regions (HALT and normal inter-instruction processing).
The original source uses `|=` and `&= ~` on one shared map.  In the standalone
worker these producers may run on host threads, so restoring those ordinary
read/modify/write forms can erase a newly raised unrelated bit.

The current snapshot/OR/CAS-take helpers preserve every original bit, its
consumer order (RESET, timer, SAD, hardware interrupt), PIC acknowledgement
and guest delivery.  They are the minimum unavailable-host transport, not an
event policy replacement.  No E01 code is safely removable: **0 lines**.

## Verification and disposition

The source sweep found no alternate selected event queue or duplicate C-VID
owner. The prior U01 lost-RESET finding is explained by the original RMW
race; a blind rollback would reintroduce it.  CPU instruction semantics,
heartbeat semantics and C-VID timing remain unchanged.  This closes E01's
source-first disposition and transfers only whole-product integration to S5.
