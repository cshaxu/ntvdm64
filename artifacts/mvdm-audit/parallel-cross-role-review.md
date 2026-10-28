# Original WOW menu/COMM and BIOS time owners

The independent shard observers were replayed at base `f7a974aba`. Original
`wumenu.c` and `wucomm.c` own 20 menu and 17 COMM initial USER dispatch
targets in the released WOW32 image; selected menu conversion and task cleanup
calls are also linked. This proves initial dispatch and specific call edges,
not full menu behavior, actual serial I/O, callback reentry, concurrent
termination or resource retirement. The original COMM source's multi-port
cleanup behavior requires explicit compatibility analysis.

Original `time_day.c` owns the BIOS 1Ah dispatch target; three emitted bodies
match the release image outside relocation words. Clock progression, rollover,
RTC state, interrupt ordering, failures and reset are not proven by that
binding. The separate inactive debug environment helper proposal is not merged.

All three originals retain 100% source bytes and remain partial; their
restored-behavior percentage is unmeasurable. No guest or product image changed.
See [WOW USER](shard-2/parallel-review-fourth.md) and
[BIOS time](shard-3/support-clock-env-role-review.md) for the full boundaries.
