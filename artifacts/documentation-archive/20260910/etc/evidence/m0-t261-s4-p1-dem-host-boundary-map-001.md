# M0 T261 S4 P1 — DEM host-support boundary map

## Question

For every remaining production `opennt-host/dem` source, which functions are
an OpenNT BOP/provider composition, a historical host capability, a legacy
Win32/NTDLL facade, or a SoftPC/CCPU guest-machine facade?  Can the next moves
preserve the original caller spelling without adding an `opennt-host` to
`opennt-bop` dependency?

## Inputs

- `src/opennt-host/dem/*` and their imported callers in
  `src/opennt-bop/dem/{demdasd,demdir,demerror,demfile,demgset,demhndl,demmisc,demsrch,demfcb}.c`.
- OpenNT comparison bodies retained under `docs/etc/legacy_code/opennt-bop/original/dem/`.
- T261/S2 binding recovery ledger and the shared T232 host-handle / guest
  pointer-manager contracts.

## Procedure

1. Enumerated each public and static function in the remaining nine DEM
   implementation seams and their public headers.
2. Traced their imported source callers and every current cross-component
   include.
3. Classified each function group at the first viable recovery rung, retaining
   original names, layouts, order and source failure branch where it remains
   directly reached.

## Observations and binding decisions

### `demhndl_shim.{c,h}`

- The active-call record, register accessors, `GetVDMAddr`/flush/free ordering,
  typed CPU result, service-specific bounded lease selection, and the
  `bx_ntvdm_demhndl_invoke*` entry points are BOP-provider composition. They
  belong beside the imported `demhndl.c` in `opennt-bop`, not in host.
- Its opaque native-handle lookup/publication is already delegated to the one
  `adapter-softpc` session handle-manager interface. No second host table may
  be introduced.
- Its `Vr*` calls must remain same-named calls from the imported DEM body, but
  target a lower-level Redirector host capability facade. They must not call a
  BOP 57 session dispatcher.

### `demfile_shim.{c,h}` and `demdir_shim.{c,h}`

- `bx_ntvdm_demfile_invoke` and `bx_ntvdm_demdir_invoke` are BOP composition
  and move with the corresponding original provider body.
- `CreateFileOem`, file attribute/delete/rename/volume helpers, and the three
  OEM directory/current-directory helpers are legacy public-Win32 shapes.
  They belong in `adapter-win32` under their original spellings, preserving
  OEM conversion, error order and the existing `CreateFileOem` retry rule.
- `LoadVdmRedir`/`VrConvertLocalNtPipeName`/`VrAddOpenNamedPipeInfo` retain
  their historical VDMREDIR API names.  Their implementation remains a
  host-side Redirector capability, while BOP 57 session decode stays separate.

### `demgset_shim.{c,h}`

- The service selection and guest CDS/DPB/DTA layouts are BOP composition and
  move beside `demgset.c`.
- `GetComputerNameOem`, `GetDiskFreeSpaceOem`, drive type and local-clock calls
  are public Win32 capability facades.  Their source-visible spellings belong
  in `adapter-win32`.
- The copied drive-admission snapshot remains the named
  `opennt-host` exception: it is project session policy with no independent
  historical host translation unit and retains excluded-drive failure before a
  host query.

### `demerror_lock_shim.{c,h}`

- Guest hard-error structure access and register restoration are BOP
  composition using a bounded `adapter-softpc` lease; they move with
  `demerror.c`/`demlock.c`.
- `RtlAppendUnicodeToString`, symbolic-link open/query and Unicode comparison
  are old NTDLL shapes and must move to `adapter-win32`, retaining the caller's
  NTSTATUS/error ordering.

### `demsrch_fcb_shim.{c,h}`

- The invoke entry and DTA/FCB memory layouts are BOP composition, moving with
  the imported search/FCB bodies.
- OEM FindFirst/FindNext and compatible Unicode/file-query shapes are
  `adapter-win32` facades.
- The historical VDD hook and `NtVdmControl(VdmQueryDir)` product services
  have no directly composable modern provider. They remain explicit
  source-derived unavailable paths until the VDD/Debugger owner package,
  rather than becoming a hidden directory-search fallback.

### `demmisc_shim.{c,h}`

- Its service invocation, copied loader span and terminal CPU result are BOP
  composition.  The `CreateFileOem`, `ReadFile` and full-path compatibility
  calls split to `adapter-win32`; checked guest span operations split to
  `adapter-softpc`.
- VDD hook/module notifications, debugger prompt, FDC reset and symbolic
  debugger operations are retained individually as source-derived unavailable
  exceptions. They cannot be claimed as host capability merely because their
  source caller is DEM.

### `demdasd_ioctl_shim.{c,h}`

- `bx_ntvdm_demdasd_ioctl_invoke`, copied register state, `sas_loadw` and
  `host_simulate` are BOP/SoftPC boundary composition.  `host_simulate` keeps
  the existing original failure branch because recursive SoftPC execution is
  not a Bochs capability.
- The `nt_fdisk_*` lifecycle is an OpenNT host capability derived from
  `softpc.new/host/src/nt_fdisk.c`; its public volume/geometry replacement is
  an adapter-win32 dependency.  It must be re-rooted under that original
  host path or registered as a statement-level source-derived exception before
  S4 closes.
- Floppy/FDC/DMA/CMOS remains a machine-owner transfer with explicit
  `ERROR_NOT_SUPPORTED`, never a host-storage substitute.

## Dependency order

1. Publish shared `adapter-win32` OEM/file/directory/NTDLL facade headers;
   do not alter imported callers yet.
2. Move DEM BOP invoke and guest-session composition to `opennt-bop`, leaving
   all original external spellings callable through lower components.
3. Re-root the independent `nt_fdisk` host capability and bind it only to the
   admitted drive-policy exception and adapter-win32 public calls.
4. Split Redirector's host API from BOP 57 service decode as one later joint
   operation; `demfile` and `demhndl` cannot be moved ahead of that API split.
5. Retire superseded `*_shim` filenames only after all import, fixture and
   formal-manifest consumers use the named final paths.

## Interpretation and confidence

The apparent current reverse edge is not an OpenNT source requirement: the
original DEM files use a host VDMREDIR API, while the present file combines
that API with BOP 57 decoding.  Preserving the original callers therefore
requires an extracted lower Redirector capability facade, not an upward call
from `opennt-host` to `opennt-bop`. Confidence is high for the ownership and
ordering map because every group was traced to a direct imported caller; the
exact Redirector API extraction remains S5 work because it owns the shared
state/lifetime boundary.

## Follow-up

S4 P2 may implement only the first independent group: adapter-win32 OEM
directory/file/NTDLL declaration-and-body facades plus corresponding BOP
composition rehomes. It must not change BOP service behavior or touch the
Redirector session until the joint S5 ledger is admitted.
