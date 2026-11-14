# DOS4GW / NESticle integration and investigation

The owner reports `run16 dos4gw nesticle.exe` exiting with DOS/4GW exception
0Dh at 023f:00006f12, error 4151, while DOS4GW without an argument reports
its normal usage message. The current external fixtures are pinned by hash in
`tests/observation/verify-dos4gw-nesticle.ps1`; they match the previous S11
observations. The Full suite accepts `-Dos4gwFixtureRoot` to run this case;
without the owner-supplied fixtures the suite prints an explicit skip.

The test launches the real run16 and original DOS4GW and NESticle in an
isolated ten-component package and private desktop. A clean-environment control
reached a visible `LibKvmWindow` while the application remained alive after
15 seconds; this is a window-progress witness, not rendered-pixel, gameplay,
or normal-exit acceptance. The inherited-environment control instead reached
an NTVDM illegal-instruction dialog. Shortening only PATH produced R6009
`not enough space for environment`. These different failures do not establish
the root cause of the owner's specific 0Dh or prove that these outcomes are
the same defect. The test normally inherits the caller's environment and
fails on absence of graphics; `-MinimalEnvironment` is a labelled control,
not a workaround used to pass the normal gate. The original fixtures remain
unchanged.

Previous S11 reached graphics using an instrumented frontend, not a full
production-frontend acceptance. Protected-mode CPU40 monitor dispatch is
still present in `nt_inthk.c`; retired hardware V86 monitor code must not be
restored. The actual faulting bytes and descriptor state for the owner's
reported 023f:6f12 fault remain unverified.

A separate controlled test preserved a minimal baseline environment and added
one synthetic variable of 0, 1024, 2048, and 4096 characters respectively.
The first three observations reached the visible graphics window; the 4096
case exited with R6009 and no graphics window. This establishes an
environment-size-sensitive failure in this fixture, not the cause of the
owner's selector-valued 0Dh. The external fixture test is currently red for
the inherited environment and is not an unconditional Full-suite pass.

The Full gate inherits the environment of its suite runner, which may itself
be sanitized; callers must test a representative outer CMD environment as a
separate run. The graphics-window assertion is a bounded startup witness, not
a guarantee that all rendered pixels or input interactions are correct.

## Confirmed PATH overflow (2026-11-13)

This follow-up supersedes the unresolved 6F12 attribution above. With the same
pinned guest hashes, the installed-package isolated reproduction and a normal
Windows-generated user environment produce 0237:00006F12 / 4151. Both direct
NESticle and DOS4GW plus NESticle reproduce it. The instruction is `CA 04 00`,
`RETF 4`, at original DOS4GW file offset 1C5D6.

Two hardware data-watchpoint runs on owned workers establish the writer chain.
DOS4GW calls its unbounded string copy with PATH and destination DS:16FA.
Its `REP MOVSW` at 0237:09C7 overwrites the original `COMSPEC` constant at
DS:190D. The subsequent query at 730E passes that corrupted constant to 6EA9.
That function copies the now-long string into its stack buffer at BP-26 and
uppercases it. The return slot becomes text (4152:474F, bytes `OGRA`), and
`RETF 4` faults. The host watchpoints do not patch guest code. Undebugged runs
reproduce the same failure. [Machine-readable evidence](dos4gw-path-overflow.json)
retains guest identities, writer register snapshots and control results.

Keeping the normal user environment and changing only child PATH to 46
characters reaches a visible graphics window. Repeating `C:\Windows;` eight
times (88 characters) also reaches graphics; 93 repetitions (1023 characters)
reproduce 0237:6F12 with error 5C39. This controlled case removes dependence
on private directory names. These observations do not establish a universal
safe PATH threshold, gameplay correctness or clean application exit.

This is an unchanged DOS4GW 1.97 guest overflow, not evidence requiring a
RETF/descriptor correction in NTVDM. Preserve the original binaries and do
not silently truncate the host environment or hot-patch the guest. An explicit
caller-local short PATH is a temporary workaround, not a product fix. The
separate large-environment COMMAND failure and R6009 remain separate issues.
No matching lost fix was located in the current five stashes or relevant
reflog search; the earlier DOS4GW commits retain investigation and controls.
