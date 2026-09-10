# T236 S3 DEM Hard-Error/Retry Source Recovery Result

## Question

Can the imported OpenNT `demSetHardErrorInfo` (`50:32`) and `demRetry`
(`50:33`) bodies retain their original saved-register and service-dispatch
semantics through the fixed bx-vdm boundary, without creating a second DEM
dispatcher or placing machine semantics in bx-vdm?

## Source And ABI Map

| Source owner | Retained behavior | bx-vdm seam/disposition |
| --- | --- | --- |
| `bop/opennt/dem/demerror.c::demSetHardErrorInfo` | Captures the DOS VHE and SYSDEV locations from `DS:DX` and `DS:BX`. | Direct imported body.  The shim owns two checked fixed-size guest-RAM snapshots because the historical SAS mapping cannot cross the typed boundary.  VHE write-back occurs before the scoped call returns. |
| `bop/opennt/dem/demerror.c::demRetry` | Restores `SAVEDEMWORLD`, reads `CurrentISVC`, and invokes `apfnSVC[iSvc]`. | Direct imported body.  The shim makes the copied GPR and DS/ES state mutable for the nested call, then emits the existing typed result deltas. |
| `bop/opennt/dem/demdisp.c::apfnSVC[]` | Owns the 73 DEM service handlers and the dispatcher selection. | The sole active table.  The former shim-owned all-unavailable table and wrapper were removed. |
| Original handlers using files | Consume `AX:BP` opaque IDs according to the imported DEM ABI. | Existing unified host-handle manager route; no host `HANDLE` enters guest registers. |

`VHE` and `SYSDEV` are not host handles.  They remain checked guest-memory
layouts; only the file token restored in `AX:BP` is resolved through the
unified handle manager.

## Recovery And Rejection

- `demRetry` now re-enters the original `demdisp.c` handler selected by the
  saved service number.  There is no adapter retry-provider table and no
  `ERROR_CALL_NOT_IMPLEMENTED` fallback that can shadow a real handler.
- The restoration seam supports the original 16-bit GPR values plus `DS` and
  `ES` needed by a nested service.  It changes neither CPU decode nor Bochs
  selector semantics.
- The imported `demClientError` is the active error owner; the superseded
  source-derived helper in `demhndl_shim.c` was removed.
- A caller that reaches a hard-error operation without first registering
  `50:32` violates the historical VHE prerequisite.  This S does not invent
  an adapter error-dialog or an INT 24/machine policy to conceal that missing
  guest initialization.
- DPMI/protected-mode continuation, guest error-dialog delivery, BIOS/device
  behavior, and guest-DOS normal-return continuity are outside this two-BOP
  package and retain their existing owners.

## Focused Formal Verification

Formal Ninja root: `build/M0-T236-S3/001`.

1. Rebuilt the source-current `bx-vdm` archive and the focused fixtures using
   MSVC x64 `/MT /W4 /WX`.
2. Ran `t230-s8-demerror-lock-direct-import-fixture.exe` successfully.  It
   registers VHE/SYSDEV with `50:32`, installs a `SAVEDEMWORLD` image for
   service `00`, then invokes `50:33`.  The retry re-enters original
   `demChgFilePtr`, resolves the saved `AX:BP` 32-bit opaque token through the
   unified manager, returns the original seek result, and publishes restored
   `DS`/`ES` deltas.
3. Ran `t230-s10-demdisp-direct-import-fixture.exe` successfully, verifying
   the sole 73-slot imported dispatcher table and its ordinary/no-op bodies.
4. An immediate Ninja dry-run for both targets reported `no work to do`.
5. `git diff --check` passed.

## Result

`50:32` and `50:33` are source-backed direct imports with only the bounded
SAS-to-typed-ABI seam required for checked guest memory and copied register
state.  No Bochs intrusion, second dispatcher, guest pointer escape, raw host
handle exposure, or adapter-owned DEM retry semantics remain in this path.
