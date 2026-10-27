# Original standalone utilities: build, runtime and replacement boundaries

## Scope

Shard 1 owns `vdmutils/graftabl/graftabl.c`, `vdmutils/pifedit/pifsubs.c`, and
`vdmutils/win/win.c`. These are unchanged original source files for separate
executables, not NTVDM worker-object providers. The project architecture
explicitly identifies `graftabl`, `pifedit` and `win` as original standalone
MVDM tools; absence from the worker link is therefore not an exclusion of
their observable functionality. No tool was compiled or run in this review.

## GRAFTABL and Console output code page

Original `graftabl/sources` defines a separate executable; `graftabl.c::main`
uses `GetConsoleOutputCP`, `SetConsoleOutputCP`, and resource-backed messages.
It states it changes the Console **output** code page, not the input page.
`/STATUS` reports current output page; invalid values and API errors take
separate exit directions. Its resource file is part of the original tool.
A name-filtered read of `O:/winnt/system32` found no GRAFTABL tool; this is
not proof no other PATH entry or external installation can supply it.

Cross-worker state is a concrete compatibility boundary even if the tool is
supplied externally. NTSRV `worker_registry.c` starts native CUI NTVWM using
`CREATE_NEW_CONSOLE`. `ntvwm_capture_begin_output` reads both code pages from
that Console. `presentation.c::native_publication` carries screen/cursor,
video description, title and cells, but no code-page fields; whole-source
search of `src/ntvwm-exe` found the capture fields assigned/checked but no
subsequent uses. Separately, bound NTVDM's `MvdmGetConsoleOutputCP` sends
`CONSOLE_IO_CODE_PAGE` to NTCON, whose handler queries its own current
Console. `tests/app/console_client_test.c` verifies a bound frontend query,
not a native-CUI-to-DOS code-page change followed by ownership transfer.

This is a **source-level gap candidate**, not a reproduced defect: other
initialization, an external Console configuration, or another path may affect
observability. Validate an isolated root COMMAND -> native CUI code-page
change -> COMMAND consumer with before/after pages, error and exit ordering,
plus the same carrier without a page change; do not change the user's live
Console. Trace identity and persistence for both parent-existing Console and
new Console; use an independently authored disposable probe or an authorized
original tool without changing guest. If a discrepancy is proved, preserve
original one-Console semantics through the existing frontend/worker lifecycle
and protocol, with one explicit code-page authority; do not introduce a
parallel shell/parser. Further implementation requires the owning S packet.

## PIF editor versus PIF utility

Original `pifedit/sources` defines an independent PIFEDIT application;
`pifsubs.c` includes original dialog-bound validation/update/save operations
such as `UpdatePifStruct`, `DoFieldsWork*`, and `SavePifFile`. Its interactive
GUI and complete supported field/error semantics are not established by
linking a different tool.

The separately authored project addon `src/addon/pif` builds AMD64 `PIF.EXE`;
T437 S3 records a format-oriented show/create/update tool and its PIF record
and checksum tests, not an interactive PIFEDIT replacement. Current read-only
comparison found `assets/release/PIF.EXE` SHA-256
`9A2265B888DF7F219073B6FD78031FC5C638BFEFC1EF2402E186FA8E11D0B283` and
`O:/winnt/system32/PIF.EXE` SHA-256
`A2C96803CD833FB858017F3F28DFBA9CD7F6152D5513DA2B8BC9825183F06DFC`.
T437 S3 describes addon manifest delivery rather than the ten-image host
manifest, so those unequal files alone do not prove a broken runtime release
or establish the current deployment rule. Audit the applicable addon
manifest and actual caller before any replacement.

## WIN launcher utility

`vdmutils/win/sources` defines a standalone `UMAPPL_NOLIB` target.
The original `win.c::WinMain` removes leading slash/dash switches, calls
`CreateProcess` with the remaining original command line, waits, and returns
its child exit code or an error. Invoking it with no remaining command returns
zero. It is a historical helper for installers that execute `win appname`;
it is not the same requirement as booting full Windows 3.1 and not proven
covered by merely launching an application with `run16`.

The one checked directory `O:/winnt/system32` contained no `WIN.COM` or
`WIN.EXE`; that does not exclude Windows installations or PATH elsewhere.
Original caller discovery, switch/quoted-tail/exit semantics and a testable
separate-tool build or an approved equivalent remain unproved. Neither
additional tool delivery nor application availability follows from a source
file or its independent build recipe alone.

## Disposition

All three paths remain pending: selected runtime ownership and separate tool
packaging must be reviewed by functionality, not by whether they are linked
into the NTVDM worker. The current evidence proves source shapes and a limited
filesystem observation only; it does not claim tested equivalence, a missing
installation system-wide, or a deployed regression.
