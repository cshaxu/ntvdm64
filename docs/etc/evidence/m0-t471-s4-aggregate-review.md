# M0 T471 S4 — aggregate review and acceptance readiness

## Question

Does the T471 whole-provider acceptance packet now have one unchanged,
hash-matched product package, complete selected S1--S3 evidence, and a finite,
truthful statement of its one owner-deferred physical-input limitation?

## Package identity

The reviewed package is still `build/t467-s3/package-r2/SYSTEM32`.  On
2026-10-26 its ten runtime images match the installed
`O:\winnt\system32` counterparts and the S1 manifest.  The key images are:

| Image | SHA-256 |
| --- | --- |
| `run16.exe` | `91811CEF9FC34564827AE530D700F46CF06BA7763035FC3DF206D1674AB2A30D` |
| `ntsrv.exe` | `6BB3A000B379C669B3EE4BDA5C8AB19FCDA05A5A9CBD236A9AEBA27CCF5D4058` |
| `ntcon.exe` | `9A192E04B56FBE8AD190FC207CF0D6755FF2D32DAAC0821399A6DAAAC46F5A8E` |
| `ntvdm.exe` | `7F2312AF2CC8480264B4935E2F5C9910D1A99D38C78C8F8DB57156FF1168611B` |
| `ntvwm.exe` | `A55E480E1CFC76508757501A20E62867A660CB95FED458040E4B2B3829410B46` |
| `WOW32.DLL` | `506586EFB2B4C953C7D10468A592570E470209A6224A3B3D244880B03F7FFC23` |

`ntmon.exe`, `VDMREDIR.DLL`, `nthook32.dll` and `nthook64.dll` also matched
their S1 rows.  T471 only added test/evidence code after this freeze; it made
no production source, release asset, installed-image or guest-media change.
No new publication is required for a byte-identical package.

## Reconciliation

1. [S1](m0-t471-s1-provider-artifact-freeze.md) reconciles W01--W19 to their
   named closure, including normal/failure/retirement ownership and each
   explicit capability boundary.  T471 adds no provider, private USER server,
   scheduler, guest rebuild or temporary substitute.
2. [S2](m0-t471-s2-application-integration-progress.md) records real
   startup/close/restart for WRITE, selected WINMINE gameplay, SOL
   geometry-limited gameplay and Options.  Its physical WRITE edit/save/dialog
   interaction remains owner-deferred **non-pass** because the test host could
   not inject on the active input desktop; it is not described as a product
   pass or defect.
3. [S3](m0-t471-s3-progress.md) passes shared-worker task retirement,
   independent-worker idle/retirement, selected OEM capacity/cleanup,
   COMMAND/native EDIT/DOS return, and identified WOW-worker loss followed by
   DOS `MEM`/`EXIT` recovery.

## Review result

The selected whole-provider acceptance scope is coherent and ready for owner
T-level acceptance.  The only open item is the already-approved, explicitly
non-passing visible physical WRITE interaction; it is not silently transferred
to another provider or claimed resolved.  This record does not close T471 by
itself and does not replace owner acceptance.
