# Original standalone utilities: build, runtime and replacement boundaries

## Scope

Shard 1 owns `vdmutils/graftabl/graftabl.c`, `vdmutils/pifedit/pifsubs.c`, and
`vdmutils/win/win.c`. These are unchanged original source files for separate
executables, not NTVDM worker-object providers. The project architecture
explicitly identifies `graftabl`, `pifedit` and `win` as original standalone
MVDM tools; absence from the worker link is therefore not an exclusion of
their observable functionality. The original tools were not compiled or run; a separately authored native code-page probe was compiled and run as described below.

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

### Follow-up: original and current process/IO path (source review)

* Original `src/mvdm/dos/command/cmdexec.c::cmdCreateProcess` forwards
  standard handles and calls `CreateProcess` with `CREATE_SUSPENDED` and
  `CREATE_DEFAULT_ERROR_MODE`, without `CREATE_NEW_CONSOLE`: the native CUI
  child normally shares the caller's Console and its output code page. This
  conclusion applies to that path, not every original launch scenario.
* Current `src/ntsrv-exe/opennt/source/worker_registry.c` starts the text
  NTVWM carrier with `CREATE_NEW_CONSOLE` and hides its window. Current
  `src/ntvwm-exe/execution.c::launch_request` invokes
  `run16_native_launch_start_suspended`; `src/common/native_process.c` calls
  `CreateProcessW` without `CREATE_NEW_CONSOLE` or `DETACHED_PROCESS`, so the
  native CUI target normally joins the carrier's Console. Standard-handle
  redirection does not itself move a process to another Console.
* `src/ntvwm-exe/presentation.c` takes the output-page sample indirectly via
  `ntvwm_capture_begin` and publishes Unicode screen cells; the publication
  object and the Console I/O opcode contain no output-page setter. The
  sample is not proof of copying the page to NTCON: repository-wide search of
  the NTVWM source found only the assignments and checks in
  `console_state.c`, not a setter or later consumer of those two fields.
* When an NTVDM session is bound, `console_client.c::console_code_page`
  asks NTCON for `CONSOLE_IO_CODE_PAGE`; `console_frontend.c` responds by
  `GetConsoleOutputCP` for output. The unbound fallback queries the local
  Console, so it cannot be treated as evidence about a bound DOS session.
  The existing `console_client_test.c` 437/850 check changes the test's own
  Console and proves the getter, not cross-carrier propagation or survival.

These are current-source findings, not fresh process or runtime observations.
The exact linkage from a later guest code-page consumer to the bound getter,
identity of both Consoles in a live session, initialization/handoff effects,
and whether externally supplied GRAFTABL can run remain to be demonstrated.
A conclusive test must directly record each Console identity and code page,
including the output-page setter's return value, before and after return to
DOS and with a no-change control; a Unicode frame alone is not that test.

The **carrier code-page mismatch and one DOS text-output consequence are
reproduced** below. Other initialization, external Console configuration, and
other output paths can affect observability. Validate an isolated root COMMAND -> native CUI code-page
change -> COMMAND consumer with before/after pages, error and exit ordering,
plus the same carrier without a page change; do not change the user's live
Console. Trace identity and persistence for both parent-existing Console and
new Console; use an independently authored disposable probe or an authorized
original tool without changing guest. If a discrepancy is proved, preserve
original one-Console semantics through the existing frontend/worker lifecycle
and protocol, with one explicit code-page authority; do not introduce a
parallel shell/parser. Further implementation requires the owning S packet.

### Isolated native-CUI transition witness (2026-10-26)

A fresh short physical stage made by `New-PhysicalRuntimeStage` copied the
S9 `build/t473-s9/package-r10` package, with unmodified COMMAND.COM. On a
private desktop the existing `console-startup-observer` ran
`run16 COMMAND.COM`, then injected `CPTEST control1`, `CPTEST set`,
`CPTEST control2`, and `exit` after the preceding visible markers. The
independent CUI probe source and exact runner are archived alongside this
note as `codepage-carrier-probe.c` and `codepage-carrier-run.ps1`. CPTEST
queries its inherited Console, optionally calls `SetConsoleOutputCP`, then
attaches to the sole isolated NTCON process to query its actual Console.
The helper refuses to identify NTCON when multiple processes match. The run
used a desktop-capable execution context and a checked-in observer/helper;
the process token integrity and supervising parent PID were not recorded.

```text
mode=control1 prior=437 requested=850 changed=0 hidden=437 ntcon_pid=29724 attached=1 frontend=437 error=0
mode=set prior=437 requested=850 changed=1 hidden=850 ntcon_pid=29724 attached=1 frontend=437 error=0
mode=control2 prior=850 requested=437 changed=0 hidden=850 ntcon_pid=29724 attached=1 frontend=437 error=0
observer exit=0; result=exited; exit=0x00000000
scripted-console-input=delivered; all three next markers ready
```

Thus the control starts with both Consoles at 437; the setter succeeded and
changed the native carrier to 850 while the frontend stayed at 437; a later
native child still saw 850. This proves an actual state mismatch and native
carrier persistence for this existing-Console observer scenario. It does not
prove that guest COMMAND itself reads this value, that GRAFTABL is installed
or launches successfully, that a particular guest character renders wrongly,
or that the sole-member/new-Console scenario behaves identically. The probe
reads the frontend during the setter's lifetime and again from a subsequent
native child; it is not a direct DOS-side code-page query. No product or guest
file was modified. The test-owned stage and scoped processes were cleaned.

SHA-256 identities: probe exe
`9F917B2C64FE837B7F9B1B623018100CBF98CC311C9ACBFFA816776D909B5AFF`;
NTVWM `11AF2D426A4F4B9A19197500E374AABBD9C01203B4137577A6ECFAE86849336D`;
NTCON `963F4EB8F531AE8223CB191EE0A0A73ADB2677F55BB38EB42F471ED1B4326298`;
COMMAND.COM `908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`;
observer `4A73E58F555EBCBEBA37DB9F10EEC580A8160CD0C3D95F1F5ECE911110D4BCC7`;
runner `F50EC6C84FB7AE57366278FE7D22604688829E6B122BBEA9E3C163B9E699FA4B`.
An earlier restricted-token attempt returned `not-started` at private desktop
creation; it is runner setup evidence only, not a product result.

### DOS output observation after native code-page change (2026-10-26)

An independent tiny `.COM` fixture uses DOS `INT 21h/AH=40h` to write an
ASCII marker, byte `9B`, and a newline. Before/after fixtures have distinct
`CPTEXT1:` and `CPTEXT2:` markers and otherwise identical behavior. They are
not changes to COMMAND.COM or the product guest. `codepage-guest-observer.c`
is a native CUI probe that records its carrier CP, optionally calls
`SetConsoleOutputCP`, then attaches to the uniquely identified isolated NTCON
and reads its actual output CP and Unicode Console cells. It scans the last
`CPTEXT1:` or `CPTEXT2:` cell marker; marker and subsequent character must be
on one screen line. The exact runner is `codepage-guest-run.ps1`. It stages
the S9 package through the checked-in physical-stage helper and uses the
same private-desktop observer; the prior section records package identities.

```text
control1: hidden=437, frontend=437, glyph=U+00A2
set:      hidden=850, SetConsoleOutputCP success, frontend=437, glyph=U+00A2
control2: hidden=850, frontend=437, glyph=U+00A2
```

The observer records distinct visible `CPTEXT1:` and `CPTEXT2:` markers, and
all input gates through the second fixture as ready. Its final screen dump
contains `CPTEXT1:` and later `CPTEXT2:` followed by the later control marker.
Thus the second probe's last-marker scan observes the later DOS output, after
the native code-page change and before the isolated session is torn down.
Byte 9B maps to U+00A2 in CP437 and U+00F8 in CP850. The displayed DOS
character therefore follows the unchanged frontend CP437, not the successfully
selected carrier CP850. This demonstrates a guest-output consequence of the
separate Console state for this scenario; it does not execute GRAFTABL itself,
prove the original NT4 run empirically, or cover all encodings, output paths,
root/new-Console creation, or guest applications.

The runner deliberately waits for an impossible final input marker so that
`exit` cannot race with the last control's read; the observer reports
`result=timeout` and `scripted-console-input=failed` at that deliberate
35-second test deadline. This is a **test-observer timeout**, not proof that
the product was unable to exit. Its runner exits zero despite this observer
result; the success claim above depends only on the recorded earlier steps,
not on exit status. Scope cleanup terminated its own temporary processes.
A previous attempt used immediate exit and lost the final read; another tried
writing the completion marker directly to NTCON and the marker was overwritten;
neither establishes product failure. No product or original guest file changed.

Hashes: before COM
`7A8219C57594E6CD8C9971D7C5D79B219DDD7DB8B1F5DD2F1C4B8E82C19930B3`;
after COM `054078E8FB21ABA0BEC42459C8E3F950DCD5244EE27FA0AD7BE4EA9B82546AB1`;
native probe `DDD068AA9B817078E5FBF9ECF173B5CE05F40324D29EE89E147903DD2FC7E0E0`;
runner `38CA72D58DE9FA30A435CA06E3B50A5B63CAB488825029E38107A8FEEA0F648A`.

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
