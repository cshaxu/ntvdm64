# M0 T388 S3 — Console-first startup disposition

## Result

The normal product path no longer opens the app presentation window before
original SoftPC startup.  A text-mode session therefore has one guest-facing
surface: the process Console which the original SoftPC host code acquires.

## Original owner evidence

- `src/mvdm-host/softpc.new/host/src/nt_hosts.c::InitScreenDesc` obtains the
  original standard input and output handles, reopens `CONIN$`/`CONOUT$` only
  for redirected standard handles, synchronizes `SCS_hStdIn/Out/Err`, and
  calls `SetupConsoleMode`.
- `nt_hosts.c::SetupConsoleMode` owns the original Console-mode transition.
- `src/mvdm-host/softpc.new/host/src/nt_reset.c::host_applInit` calls
  `init_host_uis`, whose source owner is `nt_hosts.c::InitScreenDesc`.  This
  is before the original event and CPU-start lifecycle.

Consequently `app` must not create a parallel text renderer before those
original owners select graphics or fullscreen state.

## Product change

`src/app/entry.c` now initializes the session and invokes the original SoftPC
entry without calling `app_presentation_window_prepare` or
`app_presentation_window_open`.  The presentation component remains compiled
but dormant; a later S5 display-arbitration path is its only allowed product
admission point.  No guest bytes, Console input, keyboard state, IRQ, PIF
state, BOP record, CPU or SoftPC source body changed.

## Diagnostics

`entry.c` retains only two app-owned diagnostic mechanisms:

- an early, fatal package-path `MessageBoxA`, before guest startup; and
- the default-off `MVDM_SESSION_DISPOSE_REPORT_PATH` report file.

Neither writes application diagnostics to the guest-facing Console.  Normal
Console output and mode selection remain with the original SoftPC host code.

## Verification boundary

The formal CPU40/x86 target relinked successfully as
`build/M0-T386/S3/base-env-x86/original-softpc-process.exe`:

- SHA-256: `FCAE24E22614D7C229F6464995D9552CCA86F5A2806B08E0AF02E55CB768BFD5`;
- only `src/app/entry.c` was recompiled, `app-machine-shell.lib` rebuilt and
  the final executable relinked; and
- the retained `LNK4070` `/OUT:ntvdm.exe` versus formal-output-name warning is
  pre-existing link metadata, not a Console or surface result.

The existing Console-owning, non-debug observer then launched that exact image
with no declared DOS command and waited eight seconds.  Its report is
`artifacts/research/m0-t388-s3-console-first.txt`, SHA-256
`16E221D1DDC68227C74DDDD184A31BBFB35D5FD0E4B5E23A2B2E465743FF5C05`:

- it identified the exact formal image;
- it retained a real `CONIN$`/`CONOUT$` container; and
- it timed out under the expected bounded watchdog instead of observing a
  product exit.

The observer is deliberately not a prompt, keyboard or window-enumeration
test.  The absence of an app presentation window is proven by the product
entry path (there is no prepare/open call); interactive input and real prompt
evidence remain S4/S7 work.  This S also does not claim graphics/fullscreen
selection, which is S5/S6.
