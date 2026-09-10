# M0 T401 S1 — WOW16 callback-boundary audit

## Question

Can the selected `WRITE.EXE` low-memory dialog be attributed to an original
failure branch through the existing native-window proxy, without relying on a
guest selector/IP, instruction trace, guest mutation, or an environment-driven
diagnostic result?

## Inputs

- The correction in [T397 cursor-resource frontier](m0-t397-s4-cursor-resource-frontier-001.md)
  which withdraws all WRITE-specific selector/IP attribution.
- [T400 S10](m0-t400-s10-write-segment-diagnostic-build-001.md), which proves
  that a live LDT selector is not stable across separate executions.
- Current original WOW32 `wmdisp32.c`, `wumsg.c`, and `wcall16.c`; the admitted
  session-owned projection in `adapter-mvdm-host-out/wow/wow_window_projection.c`.

## Procedure

Read the native proxy, each original message/callback boundary, and the
callback-frame return extraction. No guest media, product source, build graph,
environment, running process, or UI was changed.

## Source ledger

| Boundary | Stable fact | What it cannot prove |
| --- | --- | --- |
| `projection_window_proc` | A projected native `HWND` dispatches every native message through `W32Win16WndProcEx`; it falls back to `DefWindowProcA` only if the session projection cannot supply window words. | It carries no WRITE source-location or WRITE-local allocation result. |
| `W32Win16WndProcEx` | It resolves/uses session-owned window words, marshals known messages, invokes `CallBack16(RET_WNDPROC, ...)`, and returns the callback's `lReturn` when successful. This exposes a well-defined host message, callback target and 16-bit window-procedure return. | A successful `WM_NCCREATE`/`WM_CREATE` callback proves that one window message returned. It cannot identify the later `FInitWinInfo:InzFailed` predecessor or distinguish any WRITE-local allocation/file/GDI branch. |
| `WM32Create` | It converts a native `CREATESTRUCT` into the original `CREATESTRUCT16` temporary frame before the above callback; failure here is a marshalling/projection failure. | It is not the application window procedure and does not expose a WRITE-local initialization return. |
| `WU32CallWindowProc` | A guest `CallWindowProc` either invokes native `CallWindowProc` for a recognized thunk procedure or returns the result supplied by `CallBack16(RET_WNDPROC, ...)`. | It is a general subclass/message API, not an ownership link from the observed error dialog to a particular WRITE branch. |
| `CallBack16` | It records the original `RET_WNDPROC`, callback target and task frame, re-enters the selected CPU40 executor, then copies `AX:DX` back to the caller's supplied return storage. | Its returned value has only the called window procedure's message semantics; it is not a general guest exception/allocation outcome. |

## Observations

`projection_window_proc` calls `W32Win16WndProcEx` directly at
`wow_window_projection.c:159-168`. The original dispatcher obtains projection
words at `wmdisp32.c:82-95`, chooses the marshal operation at `156-173`, calls
`CallBack16` at `200`, and returns `wm32mpex.lReturn` after unthunking at
`235-262`. `WM32Create` starts at `432` and solely constructs the guest
`CREATESTRUCT16`. `WU32CallWindowProc` makes its corresponding recognized
thunk/guest-callback choice at `185-246`. `CallBack16` stores the original
return identifier and procedure in the callback frame at `701-716`, executes
the callback, and reconstructs the output from `AX:DX` at `838-842`.

The existing transition writer is not a qualifying witness for this S: its
ordinary selection depends on `MVDM_WOW32_PROVIDER_REPORT_PATH` and its
diagnostic selection writes a fixed file. Even though the app captures the
ordinary path after guest environment copying, a selected reporter changes the
provider's I/O path; its output may corroborate a message transition but cannot
be the sole proof of WRITE-local causal reachability.

## Interpretation and confidence

High confidence: the native-proxy/callback ABI is stable and gives a valid
future observation point for *window-message* success/failure and returned
`LRESULT`, independent of LDT allocation.

High confidence: it is insufficient to choose a repair for the current
low-memory dialog. The source supplies no identity or result bridge from this
generic callback to the finite original `FInitWinInfo` predecessor set.
Treating a `WM_CREATE` result as an allocation result would be the same kind of
over-attribution that T397 corrected.

## Follow-up

### Superseding addressability result (2026-09-09)

The selected `write16.map` and `write16.sym` do not contain the `STATIC`
`fMessageInzFailed` name. The map does, however, fix `MmwCreate` at segment 2
offset `0x05B0`; immutable selected `write16.exe` bytes at offset `0x0772`
are `C7 06 58 01 01 00`, the original `fMessageInzFailed = TRUE` store. This
establishes DGROUP offset `0x0158` without rebuilding or changing WRITE.

T401 S2 owns a same-run, module-qualified read of that one address after it
binds the non-code DGROUP by immutable data. It may not revive CPU tracing,
WRITE rebuilding, guest changes, private USER/CSRSS emulation, or a synthetic
success path.

T401 S2 must audit the next source-owned, non-selector result boundary that is
actually adjacent to the original error construction (or record that only an
unavailable original/private service owned it). It may not revive CPU tracing,
WRITE rebuilding, guest changes, private USER/CSRSS emulation, or a synthetic
success path.
