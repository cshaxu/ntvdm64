# M0 T381 — `cmdExec32` predecessor transfer

T381 is not functionally closed.  Its active S2 completed P1's bounded
payload observation and P2's current-formal predecessor classification.

The one current-formal fixed observation never reaches `54:08`; it stops in
the original CPU40 SoftPC LPT status path (`printer_.c:printer_io -> ios.c`)
at port `0x3BD`.  The owner-directed pure-DOS-first rule and the queue's
dependency-first rule therefore transfer the active implementation slot to
M0 T382 before any further `cmdExec32` work.

T381 remains the next blocked COMMAND continuation after T382 proves the
current-formal LPT cohort.  No child-process, exit-37, or `54:08` runtime
completion is claimed by this transfer.
