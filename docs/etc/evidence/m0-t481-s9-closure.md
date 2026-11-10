# T481 S9 closure — mouse callback reentry

The owner accepted Win95 desktop mouse and EDIT.COM interaction on the
reentry-fixed diagnostic build and requested formal delivery before the
separate EDIT.EXE mouse investigation. Commit `41fd60439` published the
complete host package `90b4ec7f-bdf0-4011-8b2d-af156031f27d` to
`assets/release` and `O:/winnt/system32`; all 11 hashes were verified. W31
addon driver hashes matched their released artifacts. Win95 WIN.CMD was
returned to the formal package. The original guest binaries were not changed.

A real same-VM nested mouse callback overwrote the single saved-register
instance state during virtualized PIC EOI. The candidate uses a per-callback
guest stack frame. The extracted actual-body fixture fails the old nested
restore and passes the candidate, including 16-bit stack wrapping. Runtime
trace on the diagnostic candidate saw real clicks and nested callbacks with
all 226 paired callbacks restoring the recorded general registers.

The selected Full gate passed 16/16; its external DOS4GW/Nesticle case was
skipped for lack of the external fixture. Win3.1 standard and 386 DOSONLY=0
startup/desktop/normal-exit checks and Win95 guest-USER-synthesized dual
editor save checks passed. The Win95 observer exited 1, so these do not prove
normal Win95 shutdown, physical native EDIT.EXE mouse behavior or overall
multitask acceptance. A separately attempted historical mirror-guard script
failed against its stale S13 c_main.c line inventory; c_main.c was unchanged
from HEAD and this old check is not one of the 16 Full gates.

The owner requested EDIT.EXE window mouse investigation after this delivery.
Its distinct physical input path is admitted as S10; the previously planned
Win3.1/Win95 batch-PE32 investigation follows as S11, and the remaining
multitask and integrated acceptance work follows as S12. These are sequence
changes only, not waivers. See [S9 evidence](m0-t481-s9-softpc.md).
