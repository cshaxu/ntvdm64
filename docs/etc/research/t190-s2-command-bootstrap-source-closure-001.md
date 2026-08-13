# T190 S2 COMMAND bootstrap source closure 001

Date: 2026-08-12  
Packet: M0 T190 S2  
Disposition: source/test closed; no derivative build or runtime observation.

## Change

`bx_ntvdm_cmd_comspec_bootstrap_service` now mirrors the bounded portion of
original `cmdComSpec`: `54:02` requests one fixed 64-byte gather from `DS:DX`,
accepts the first NUL-terminated OEM byte sequence, caches `COMSPEC=` plus the
copied text, and returns the existing AL=1 first-profile result.  It rejects a
missing NUL in that bounded range and refuses a second completed first-call
state.  It no longer compares guest input with a profile-generated
`COMMAND.COM` pathname.

The existing `54:0F` transaction is retained: it reports required paragraphs
in BX when too small, otherwise writes only the cached COMSPEC value to `ES:0`.
It deliberately does not import ambient Win32 environment strings.  This is
the existing non-invasive CLI limitation, not a DOS-environment replacement.

`CMakeLists.txt` removes two unrelated objects from the focused test target.
They were not used by this test and introduced an undeclared host-drive link
dependency; the target now closes on its declared COMSPEC dependencies.

## Verification

- MinGW `bx-ntvdm-cmd-comspec-bootstrap-service-test` passes a non-profile
  `C:\ALT.COM` input, missing-NUL rejection, repeat rejection, and the
  existing `54:0F` insufficient/retry/write cases.
- MinGW `bx-ntvdm-adapter-runtime-test` builds successfully.
- `Test-AdapterRuntimeDiagnosticStateBoundary.ps1` and
  `Test-BochsAdapterStateTraceBoundary.ps1` pass.

No Bochs source, exception ingress, guest OpenNT source, host environment,
new BOP recognizer, device, archive, derivative build, or runtime trace was
changed or run.  The next S must audit the exact adapter-object closure before
any new manifest-bound derivative is admitted.
