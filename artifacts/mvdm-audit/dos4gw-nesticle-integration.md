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
