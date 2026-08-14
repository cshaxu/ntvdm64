# T199 S37 DEM Package Family Regression 001

## Question

Can the current source-built DEM package turn every canonical OpenNT
`50:00..48` service identity into a checked typed result without using the
bound COMMAND composition or a native execution trace?

## Inputs

- OpenNT canonical dispatch source:
  `src/opennt/base/mvdm/dos/dem/demdisp.c` (`apfnSVC`, 73 callable entries
  followed by its sentinel).
- DEM package session and planes under `src/bx-vdm/`.
- The MSVC x64 `/MT` closure selected by
  `tools/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 -Fixture dem-package`.
- The dedicated fixture
  `tests/bx-vdm/bx_ntvdm_dem_package_family_v1_test.c`.

## Procedure

1. Initialize an unbound DEM session, its boot namespace plane, and a
   contained C: drive snapshot for each service independently.
2. Construct only the copied four-byte BOP window `C4 C4 50 service`, then
   classify it through common ingress and the provider registry.
3. Dispatch each of the 73 canonical services in a fresh DEM session and
   require a valid non-pass-through typed result.
4. Exercise two input-sensitive endpoints separately: `50:02` with token
   zero, and `50:1B` with the fixture's zeroed real-mode registration record.
5. Exercise the checked FCB BOP path over guest RAM: `50:0A` first,
   `50:0C` continuation, and exhausted `50:0C`.

## Observations

`artifacts/build/t199-s37-dem-package-r35/` records a successful MSVC x64
`/MT` compile, link, PE-header audit, and process exit code zero. The fixture
does not bind global composition, invoke COMMAND, or run the native Bochs
execution loop.

During construction, the all-family fixture exposed two implementation
errors: `50:31` had no DEM component classification, and an invalid completed
`50:1B` DTA registration leaked as a session failure. `50:31` is now
classified with its actual FCB owner (`demfcb.c`); an invalid completed copied
read now goes through the DEM package's controlled source-derived failure
provider. A mantle action execution failure remains a hard transport failure.

`50:02` follows its existing provider's original token-zero success contract
(resume, CF clear, no fabricated AX), rather than the generic failure route.
`50:1B` reaches a checked controlled failure for the deliberately invalid
zero-address registration input.

The readonly namespace mutation provider now owns `50:01`, `03-06`, `08`,
`17-18`, and `22`. These are the `demfile.c`/`demdir.c` mutation operations
for which the declared contained filesystem is immutable. They return the
source-shaped DOS access-denied result (AX=5, CF=1), independently of the
general DEM fallback; the fixture explicitly checks `50:03`.

The FCB provider now owns the `demfcb.c` family boundary. It retains
`50:2E`'s null-handle close success and `50:30 demDate16`'s always-successful
DOS packed date/time result. The remaining FCB file/search operations require
an open-capable FCB backend absent from the immutable profile and therefore
return a contained access-denied failure. The fixture explicitly verifies both
special contracts.

The error/lock session plane now owns all three of its original dispatch
members. `50:32` retains copied hard-error registration; absent a saved
retryable operation, `50:33` returns AX=FF with CF set; and `50:3F` returns
the contained access-denied lock failure. The fixture explicitly verifies the
last two results. No Win32 file lock, hard-error popup, or Bochs behaviour is
introduced.

The DEM miscellaneous plane now returns the original non-debug/non-WOW branch
for `50:1D`, `23`, `34-3A`, and `3E`: V86-kernel address, no-device disk
reset, DOS-app and symbol notifications, console debugger calls, and WOW file
adjustment all resume without fabricating host debugger, VDD, floppy, or WOW
state. The fixture explicitly verifies `50:23` has no register delta.

The same fixture proves the FCB BOP attachment over the real mechanical
guest-RAM path: with `C:\\*.COM`, `50:0A` emits a continuation-bearing FCB
form, the first `50:0C` consumes it and resumes successfully, and the second
`50:0C` returns the source-derived no-more-files result (AX=12, CF=1). A
separate `C:\\*.TXT` `50:0A` call proves the legal no-match contract with the
same result and cleared continuation bytes. The fixture neither invokes the
native Bochs execution loop nor claims malformed-token, single-entry, or
complete FCB-family closure. A lower transaction regression also presents a
valid FCB token from a different PDB and proves the source-derived
no-more-files result rather than cross-PDB session reuse.

The no-device raw-media component is separately asserted through its three
canonical DEM entrypoints. `50:21` with a non-admitted IOCTL subfunction
returns AX=1/CF, mirroring `demIoctlInvalid`; `50:29` and `50:2A` return
AX=21/CF, mirroring `demAbsReadWrite` when no BDS exists. The bounded
metadata `50:21` 08/09 capability remains ordered before this terminal
provider. No raw host drive, BIOS INT13, or device model is introduced.

The contained volume capability is injected into the same DEM session only as
an immutable snapshot. `50:0E` is asserted with an admitted A: record and
returns AL=0, BX=100, CX=512, DX=200, SI=4, CF clear. This proves the
OpenNT-shaped free-space result without a BOP-time host query; media-ID buffer
write behaviour is not included in this milestone.

`50:10` is now also asserted through the same checked mechanical write path:
the fixture verifies the `VOLINFO` serial, label, and filesystem offsets in
guest RAM. Its AL!=0 set form is independently asserted to retain OpenNT's
CF-only unsupported contract with no fabricated register or guest-memory
write. The formal boot-namespace composition also exposes a pre-bind
volume-snapshot copy setter; r35 compiles that interface in the same closure.
The mixed boot-namespace regression is presently not acceptance evidence for
this addition: it stops at its pre-existing `50:11` RAM assertion (process
exit 14), before any volume path. No native trace was run.

The r36 source-built DEM fixture initially corrected `50:46 demGetDPBList`'s
packed `DPB.Next` location from offset 25 to offset 27. The subsequent layout
audit also found that the old 33-byte stride omitted `FreeClusters`: OpenNT's
packed `DPB` is 35 bytes. The provider therefore uses the source `sizeof(DPB)`
stride, while continuing to write only the three fields that `demGetDPBList`
writes historically: `DriveNum`, `Unit`, and `Next`.

Subsequent source-built regressions extend this same package baseline:

- r69 closes the FCB continuation boundary set: single-entry, exhausted,
  empty-token, altered-token, foreign-PDB and checked-aperture paths;
- r71 corrects `50:1D` from a false V86-bridge classification to its actual
  `demdisp.c` ordinary return, and regresses the entire no-debug/no-VDD/no-WOW
  MISC family, including `demIsDebug`'s AL-only result;
- r73 completes the admitted `50:21` 08/09 metadata family for removable,
  remote and invalid-drive (AX=15/CF) outcomes, while retaining raw IOCTL and
  absolute I/O as no-device terminals; and
- r74 separately proves `50:32` copied real-mode locator registration rejects
  an aperture-crossing locator rather than retaining a host pointer.
- r77 rebuilds and runs the full 73-member fixture from the current worktree,
  after the r71 MISC owner correction and r73 DASD/IOCTL correction.  Its
  fresh artifact is `artifacts/build/t199-s37-dem-package-r77-current/`.
- r78 corrects `50:27 demCommit`: OpenNT `demhndl.c` ignores its
  `FlushFileBuffers` result and clears CF, so the contained provider now
  preserves that outcome rather than using the unrelated AX=5 unavailable
  terminal.  The full fixture asserts no GPR delta and CF clear.
- r79 gives the `50:47/48` pipe-status pair one explicit no-projected-pipe
  disposition.  Their original COMMAND helpers require a pipe table,
  asynchronous state and a projected NT handle, none of which exist in the
  CLI profile; both now return `AX=6`/`CF=1` instead of falling through to
  the generic unavailable result.  This does not implement pipes.
- r81 restores the self-contained `50:44 demCheckPath` `\\DEV\\` branch:
  it reads a checked DS:SI path, writes DX=0 and clears CF for that DOS device
  directory.  Other paths retain the contained AX=5/CF terminal because the
  original `CreateFileOem(...NUL)` host-directory probe is not admitted.
- r82/r84 restore `50:08 demFileTimes` for an opened immutable namespace
  entry: get returns copied profile DOS time/date in CX/DX, while set returns
  AX=5/CF.  r84 is the focused source-built fixture; it does not emulate a
  writable host handle.

The latest complete package artifact is
`artifacts/build/t199-s37-dem-package-r82-file-times/`; r84 is the focused
`artifacts/build/t199-s37-readonly-file-r84/` fixture; r74 remains the focused
`artifacts/build/t199-s37-hard-error-r74/` ABI fixture.  Neither is a native
Bochs trace.

## Interpretation and Limits

This is package-routing and result-containment evidence, not a claim that all
73 historical DEM business operations are feature-complete. It proves that no
canonical DEM identity remains a generic deferred/raw-#UD escape in the
source-built package. Per-component source/ABI/failure work remains S37 work,
and the packet's one native trace remains deferred until that work and its
full family regression are ready.

## Follow-up

Continue the existing source/ABI/failure map by DEM component, replacing the
shared controlled fallback only where an individual original failure contract
requires a more specific provider. Do not create work from the next native
trace hit.
