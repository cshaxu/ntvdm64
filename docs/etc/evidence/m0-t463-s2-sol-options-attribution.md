# M0 T463 S2 — SOL Options source attribution

## Scope and method

The immutable `SOL.EXE` input runs only from an isolated physical runtime
stage. The observer launches the probe on its already-owned private desktop.
The probe discovers SOL's Options command from SOL's own menu, sends it,
observes the real modal dialog, closes it with `WM_CLOSE`, then verifies the
parent is responsive again. No guest binary, guest source, or desktop/UI
automation is used.

`tests/observation/verify-wow-sol-options.ps1` has explicit ordinary and
`-CaptureFault` modes. The latter retains debugger first-chance attribution;
ordinary mode is the completion witness because a debugger is not normal host
message delivery.

## Root cause and selected owner

`SOL` command `1003` follows the original route:

```text
SOL window procedure
  -> wudlg.c::WU32DialogBoxParam
  -> original dialog-manager modal route
  -> original wuwind.c::WU32EnableWindow
  -> NtUserEnableWindow
```

`WU32EnableWindow` deliberately supplies `HWND32(parg16->f1)`: an original
16-bit USER carrier, not a modern public `HWND`. NT4 USER accepted that
carrier. The project binding previously mapped `NtUserEnableWindow` to generic
`wow_native_EnableWindow`, which passed the short carrier directly to modern
USER. During modal owner disable, modern USER faulted in CoreMessaging.

The debugger witness faulted at `CoreMessaging.dll+0x32f2e` while writing
through a reserved/no-access `ESI` pointer. The same stack continued through
`CoreMessaging+0xa458b`, `+0xa8baa`, `+0x5f91d`, `+0x51cc6`, `+0x273c0`, and
`CoreUIComponents+0x44568`. The value varied per run, proving it was not guest
data or a fixed application pointer.

The existing strict `wow_user_window_public_handle()` is the public-USER
boundary owner: it accepts only an original short carrier and resolves it via
the worker's published object table (including the already committed external
observation exception). The repair uses that function only in
`wow_native_EnableWindow`, before calling modern `EnableWindow`. Original
`HWND32`/`FULLHWND32`, dialog parsing, callback dispatch and guest code remain
unchanged.

## Eliminated project edges

The following isolated candidates retained the original template parser,
original dialog manager, original callback route and immutable guest:

| Candidate | Result |
| --- | --- |
| Omit the synthetic initial `WM_NULL` default-dialog call | Same host fault. |
| Directly call the stored dialog procedure instead of host `CallWindowProc` | Same host fault. |
| Delegate counted dialog/control creation to public `CreateWindowExA/W` | Same host fault with a different staged `WOW32.DLL` hash. |
| Skip the zero-valued public `DWL_DLGPROC` restore for a dialog with no callback | Same host fault. |
| Omit nested USER-view publication or the generic native-call wrapper | Same host fault. |
| Suppress owner-disable callbacks or preserve their nonvolatile registers | Same host fault. |

The last diagnostic also observed that the redundant zero-slot restore can
report `ERROR_ALREADY_EXISTS`; it is a separate current-USER compatibility
smell, but eliminating it did not affect the fault and it is not published as
a causal repair.

## Repair evidence and disposition

The latest staged ordinary witness records:

```text
SOL options: command=1003
PASS SOL Options command, modal dialog, close and parent recovery
observer result=exited
observer exit=0x00000000
```

This proves the source-owned modal owner-disable route, live original Options
dialog, close and parent recovery. It does not claim every dialog/resource
behaviour is covered; those stay with their own admitted owner packets. Guest
media is unchanged.

## Final coherent delivery

The final candidate is `build/t463-s2/product-r3`.  Its ten selected images
were published as one package to both `assets/release` and
`O:\winnt\system32`; `build/t463-s2/publication-r1/published-manifest.json`
records the post-copy SHA-256 equality for every image.  The release also
raises the product version to `0.0.463`, so version-negotiation negatives are
tested against the admitted task rather than a stale T460 fixture.

`build/t463-s2/product-r3-full-gate-r2/timings.json` records a passing
322144-ms Full gate: the three retained WOW frontiers, Console-17, Window-17,
RPC, native GUI, version negatives, strict DIR, native EDIT return, cooked
return, rapid relaunch, interactive relaunch, session isolation, retirement
wiring and nested-window handoff all passed.  The exact final candidate also
passes `build/t463-s2/product-r3-sol-options/result.txt`.
