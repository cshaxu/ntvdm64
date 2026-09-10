# M0 T276 S12 P7: Win32/NTDLL capability interface baseline

## Question

Which original `softpc.new/host/src` Win32/NTDLL-facing interfaces can retain
their OpenNT form through a modern public API binding, and which instead
belong to a wider MVDM composition boundary?

## Inputs and method

The review starts from the corrected 56-file P2 disposition and P3 density
ledger, then reads the original source spans by complete interface family. It
records interface shape, historical dependency, one future binding owner and a
predecessor gate. It does not compile a source unit, change a mirror, create an
adapter body or infer a provider from a lexical call.

## Findings

- `config.c` is launch composition, not a Win32 helper. Its PIF, console,
  environment, monitor and machine calls require app/session and the relevant
  adapters to be reviewed together.
- `fprt.c`, `nt_keycd.c` and `nt_nls.c` are candidates for same-shaped public
  diagnostic, keyboard/NLS and resource bindings. Their historical console or
  debugger product policy is not enabled by the existence of a public API.
- `nt_error.c` retains the strongest observable error contract: reason-aware,
  same-thread deduplicated Terminate/Ignore handling. A future public dialog
  may preserve this behavior, but Terminate is a typed stop of its owning
  session—not process termination—and product-only NT4 dialog branches remain
  explicit profile exclusions.
- `nt_ntfun.c` has a recoverable public serial API shape, but its native handle
  and pending I/O state must be session-owned opaque resources. Neither x86 nor
  x64 may expose a native handle into the imported 32-bit source state.
- `nt_pif.c` retains the original parser/layout decisions; app owns whether a
  parsed PIF participates in modern launch admission.
- `nt_sec.c` crosses virtual memory, ROM mapping and VDM initialization. It is
  a monitor/machine composition prerequisite, not an `adapter-win32` utility.
- `nt_unix.c` wait/idle behavior can be session-local. Its system-path lookup
  cannot silently choose host/guest runtime inputs outside the immutable app
  manifest and source-policy boundary.

## Decision

No source is enabled. The ledger creates no generic compatibility component:
the original file remains in the `opennt-mvdm-host` mirror, while a later
package may bind a reached interface only through its one named adapter or
composition owner. Private NT4 APIs, CSRSS/BaseSrv and process-global VDM
state are not substituted by look-alike behavior without a source-derived,
same-shaped public binding decision.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S12Win32CapabilityInterfaces.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

Expected result: `T276 S12 Win32/NTDLL capability interface verification passed: 11 rows.`
