# T468 S1 — WOW32 printing and spool owner matrix

## Scope and conclusion

This is the source-first admission record for W15.  It selects four distinct
original call families and records their finite handoffs.  It adds no print
provider, device emulation, job manager, DC manager, callback carrier, guest
change or production binary.

The active x86 `wow32.dll` graph directly compiles and links the original
`wgdi31.c`, `wgprnset.c`, `wcommdlg.c` and `wspool.c`.  The selected source is
present in both pinned OpenNT editions for the raw-spool and setup bodies; the
external audit records the same GDI and COMMDLG dispatch slots in both editions.
This is therefore a recovery of an already-selected original host surface, not
a proposal to replace it.

## Selected entries and authority

| Family | Immutable guest entry / WOW dispatch | Original owner and modern endpoint | Authority and retirement | S2 disposition |
| --- | --- | --- | --- | --- |
| Raw spool job | GDI `OPENJOB` 240, `WRITESPOOL` 241, `CLOSEJOB` 243, `DELETEJOB` 244, `STARTSPOOLPAGE` 246, `ENDSPOOLPAGE` 247 | `wspool.c::{WG32OpenJob,WG32WriteSpool,WG32CloseJob,WG32DeleteJob,WG32StartSpoolPage,WG32EndSpoolPage}` → dynamically resolved `WINSPOOL.DRV` ANSI spool APIs | Native spooler owns `HANDLE`/job.  Original `GetPrn16` stores that handle in a worker-local 16-bit local allocation; the returned `WORD` is only the guest carrier.  `CloseJob` calls `EndDocPrinter`, then `ClosePrinter`, then frees that carrier only when close succeeds. | Exercise the retained body against one explicitly selected local provider and prove normal write/page/close plus endpoint failure.  Do not introduce a parallel job table or reinterpret the carrier as a global/native handle. |
| HDC document | GDI `STARTDOC` 377, `ENDDOC` 378, `ABORTDOC` 382, plus existing `STARTPAGE`/`ENDPAGE` | `wgdi31.c::{WG32StartDoc,WG32EndDoc,WG32AbortDoc,WG32StartPage,WG32EndPage}` → existing GDI/DC binding | W07/W08 own the `HDC16`↔host-DC identity and DC retirement.  `wgdi31.c` owns only document/page calls, original directory synchronization and documented compatibility form-feed/PostScript transitions. | Reuse W07/W08 unchanged.  S2 may prove document/page/abort outcomes, but may not make a second DC lifecycle or callback owner. |
| Device setup/capability | GDI `DEVICEMODE` 452, `EXTDEVICEMODE` 453, `DEVICECAPABILITIES` 454; immutable `WINSPOOL.EXE` forwards its own setup exports to these GDI slots | `wgprnset.c::{WG32DeviceMode,WG32ExtDeviceMode,WG32DeviceCapabilities}` and `wgdi.c::GetDefaultDevMode32` → dynamic `WINSPOOL.DRV` | The provider owns printer-driver state.  Original code copies guest strings and releases 16:16 flat views before a potentially re-entrant provider call; converted DEVMODE/DEVCAPS data remains caller-owned guest memory. | Preserve those copies/converters.  Test default/failure conversion only; no driver, DEVMODE cache or printer-selection policy is admitted. |
| Print common dialog | COMMDLG `PRINTDLG` 20 | `wcommdlg.c::WCD32PrintDlg` → modern `PrintDlg` | Existing W18 common-dialog state owns hook/template nesting.  Existing W07 HDC conversion owns only returned `PD_RETURNDC/PD_RETURNIC` DC identity.  The caller owns returned guest DEVMODE/DEVNAMES blocks. | Integrate only through those existing owners.  A cancelled dialog is a normal caller-visible result, not a spool job and not a new callback mechanism. |

The USER `WNET*JOB` local-only entries and `wfwnet` exports are intentionally
outside this candidate: they do not dispatch to the selected `wspool.c` path.
Likewise `wow32fax.c` uses three extra members of the same resolver table but
is a fax-installation surface, not a print/spool admission by association.

## Call and failure graph

```text
guest GDI/COMMDLG or immutable WINSPOOL.EXE
  → original WOW dispatch and pointer conversion
    → one of {raw spool, HDC document, device setup, PrintDlg}
      → existing W07/W08/W18 identity owner or WINSPOOL.DRV provider
      → original return/error conversion
      → guest caller owns its result buffers; provider owns real device/job
```

`WG32OpenJob` resolves a device name from the guest profile, obtains a default
DEVMODE when available, opens the provider and starts a document.  A failed
load/open/start returns the original zero/error convention.  `WriteSpool` maps
the provider's success to the requested 16-bit byte count; the provider error
is otherwise returned.  Raw cancellation is the original `DeleteJob` call and
must be observed as such, not substituted with a host-side cancellation policy.

For HDC printing, `StartDoc` first synchronizes the DOS current directory for
`FILE:` output, then calls the existing native DC.  `AbortDoc` removes the
original optional form-feed buffer before invoking the native abort.  There is
no independent job record in this route.  `PrintDlg` has its own template and
hook cleanup in `wcommdlg.c`; it only borrows W07's established returned-HDC
conversion.

## Endpoint and immutable-input evidence

`tests/observation/verify-wow32-dynamic-host-surface.ps1` derives the complete
15-name table from the original source and checks the x86 system-directory
`WINSPOOL.DRV`; its retained T422 result is **15 required, 0 missing**.  This
machine's inspected module has SHA-256
`BD6F7CCC755D5EB12214C47E94A068E99CA9B375AE865EE1895B485F66C77A84`.
That is a static import fact, not proof of an installed printer, provider
policy or successful physical/file job.  Current non-elevated service/CIM
queries are denied, so S1 does not make a false local-provider availability
claim.  S2 must select and record an available provider or leave that real
endpoint an explicit non-pass.

The original 16-bit companion is already installed at
`O:\winnt\system32\WINSPOOL.EXE` and matches the selected immutable source
`src/mvdm/wow16/winspool/WINSPOOL.EXE` at SHA-256
`E4E787340F124476090ED0F22837D7D7ACAB37FD7BBE49DB46CAE9AC4944D535`.
It is not one of the ten host release images in `release-manifest.json` and
S1 neither changes nor republishes it.  Earlier isolated evidence establishes
its KERNEL-only NE dependency and forwarding role; that evidence does not
claim WRITE acceptance or authorize guest modification.

## Source-established limits and bounded S2 decision

1. The original generic `LoadLibraryAndGetProcAddresses` fills the shared
   `spoolerapis` table one member at a time, asserts missing exports only in
   debug builds, and returns success even if an entry is null.  A static
   all-15 export witness avoids the failure on this host but does not make this
   inherited failure path safe.  No matching SoftPC correction was found in
   the reviewed sibling source.  Under source policy, S2 will not invent an
   atomic resolver rewrite; an unavailable/partial-provider result remains an
   explicit non-pass unless a matching correction is admitted separately.
2. `wspool.c` retains original raw-job semantics, including its provider error
   conventions and `DeletePrinter` call shape.  S1 has not established a
   source-proven defect or matching correction, so S2 must test it rather than
   “modernize” it.
3. `GetPrn16` is a local carrier, not a cross-process identity; native values
   do not escape the WOW worker.  Task-abrupt retirement needs a source trace
   through the original local-heap/task teardown before any cleanup change is
   admissible.  S2 owns that trace and a focused normal/failed close check;
   S3 alone may repair a proven job-retirement leak.

S2 is consequently finite: stage an immutable original guest print probe and
the existing companion only in an isolated test package; select one observed
provider; exercise raw open/write/page/end/close and HDC start/page/end/abort;
exercise no-printer/provider failure and PrintDlg cancellation; trace the
local-carrier retirement edge.  It must reuse W07/W08/W18 and must not add a
provider, device, print queue, callback scheduler, guest edit or release claim
until those real endpoint tests succeed.

## Evidence inputs

- `src/mvdm/wow32/{wspool.c,wspool.h,wgprnset.c,wgdi.c,wgdi31.c,wcommdlg.c,wgtbl2.h,wcmdgtbl.h}`
- `build/.ninja/msvc-x86-release/wow32/build.ninja`
- the read-only WOW32 audit's `active-dispatch-slots.csv` and `guest-exports.csv`
- `docs/etc/evidence/m0-t422-s1-dynamic-host-surface-audit.md`
- `docs/etc/evidence/m0-t447-s2-message-boundary-audit.md`
- W07/W08/W18 closure records cited by the active packet.
