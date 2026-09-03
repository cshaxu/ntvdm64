# M0 T388 S6 original PIF fullscreen disposition

## Question

Can the current CPU40/x86 product use an original PIF window/fullscreen field
to select the app presentation surface without recreating the retired NT4
Console Server hardware-fullscreen protocol?

## Original owner walk

- `mvdm-host/softpc.new/host/inc/nt_pif.h` defines `PIF_DATA.fullorwin`,
  `PF_WINDOWED == 0`, `PF_FULLSCREEN == 1`, and the original default
  `DEFAULTDISPUS == PF_WINDOWED`.
- `mvdm-host/softpc.new/host/src/nt_pif.c::GetPIFData` derives
  `fullorwin` directly from the Windows 386 PIF `fFullScreen` bit.  Its
  `SetPifDefaults` path retains the original windowed default.
- The reached source consumer is
  `mvdm-host/softpc.new/host/src/config.c`.  Under `X86GFX`, it first asks
  `GetConsoleDisplayMode`, then uses `SetConsoleDisplayMode` to request
  `CONSOLE_WINDOWED_MODE` or `CONSOLE_FULLSCREEN_MODE`.  Its fullscreen
  failure order is original: `ERROR_INVALID_PARAMETER` displays the original
  fullscreen-init error; every other failure takes `ErrorExit`.
- These APIs are the NT4 Console Server hardware-fullscreen contract.  The
  current public-Win32 adapter intentionally supplies no controller/state
  provider, and the selected CPU40/x86 product flags do not define `X86GFX`.
  Thus the source consumer is not compiled into this product profile.

## Disposition

- **PIF absent or `PF_WINDOWED`:** original `nt_pif.c` default/parser remains
  owner; no display transfer occurs, so the normal character Console remains
  the only guest-facing text surface.
- **PIF `PF_FULLSCREEN`:** explicitly unavailable in the selected CPU40
  profile.  It is neither guessed from an executable name nor translated into
  an app window.  The full original consumer requires the unavailable private
  Console Server hardware-state/event protocol.
- **Graphics and Alt+Enter:** retain their separate S5 owners.  They are not
  evidence that a PIF fullscreen request was satisfied.

No original PIF parser or guest media is modified.  In particular, a future
`X86GFX` recovery must first provide the complete source-shaped fullscreen
controller transaction and its failure ordering; it may not enable a partial
PIF-only substitute.

## Focused proof

The formal `build/M0-T386/S3/base-env-x86/build.ninja` CPU40 command line
contains `NTVDM`, `CPU_40_STYLE`, `CCPU`, `C_VID`, `SPC386`, `SIM32` and
`V7VGA`, but no `X86GFX`; it still compiles the original `config.c` and
`nt_pif.c`.  Source inspection therefore proves both relevant facts: PIF
parsing is retained, while the only PIF display consumer is unavailable.

This is a disposition closure only.  It makes no claim that an interactive
COMMAND prompt is already runtime-proven; that remains S7.
