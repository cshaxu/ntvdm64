# T202 S3 closure: DEM file/handle/FCB/search admission

## Question

Can the next T202 subtask replace the present endpoint and fixture routes with
one source-derived DEM file/handle/FCB/search owner package without silently
adopting a host pointer, a synthetic C: drive, a trace-selected service, or a
Redirector implementation?

## Inputs

- The canonical 73-slot DEM dispatcher in `demdisp.c` and the 29 identities
  selected from it for this owner subpackage.
- `demfile.c`, `demhndl.c`, `demfcb.c`, `demsrch.c`, `demdir.c`, `dosdef.h`,
  and `demerror.c`.
- T201 mutation profile ABI, T202 S2 host namespace/CWD/volume closure, and
  the existing bx-vdm boot, readonly, FCB and search routes.
- The source/ABI map
  [t202-s3-dem-file-handle-fcb-search-source-abi-map-001.md](t202-s3-dem-file-handle-fcb-search-source-abi-map-001.md).

## Procedure

1. Enumerated the selected identities from the dispatcher: `00..0C`, `12`,
   `16`, `17`, `1E`, `20`, `22`, `27`, `2C..31`, `44`, `47`, and `48`.
2. Ran `Export-T200S31BopSourceInventory.ps1` and filtered selector `50`; the
   S3 list has 29 unique identities and no duplicate service number.
3. Mapped every identity to its historical module, guest ranges/registers,
   host API, direct/readonly/overlay/virtual disposition and current bx-vdm
   route action.
4. Reconciled packed search DTA/SRCHBUF layouts and the `demClientErrorEx`
   ordinary versus INT-24 hard-error branches.

## Observations

- Local filesystem operations depend only on available ordinary user-mode
  Win32 APIs: file, directory, attribute, time and enumeration APIs.  There
  is no absent public Win32 API that blocks this local-file owner package.
- `GetVDMAddr`, `Sim32*`, raw `HANDLE` transport, historical CCPU/SAS state,
  and VDMREDIR named-pipe ownership are not reusable modern APIs.  The first
  three become checked bx-vdm transactions and session-owned opaque tokens;
  VDMREDIR stays with the future Redirector package.
- Current boot/readonly/FCB/search routes cover fixture and failure evidence
  only.  The map assigns all 29 identities exactly one migrate, replace, or
  Redirector-defer action.
- A hard device error is a distinct original result path: it requires the
  DEM hard-error owner, rather than a readonly access-denied substitute.

## Interpretation and confidence

S3 is closed as admission, not as provider recovery.  It supplies a complete
and non-overlapping implementation contract for the next whole owner package:
the package must own local file tokens, bounded pathname/buffer/DTA/SRCHBUF
transactions, direct/readonly/overlay resolution, and source-shaped results;
it must explicitly defer only the `47/48` pipe ownership to Redirector and
keep virtual boot volume default-off.  No trace has been used to select an
implementation endpoint.

Confidence is high for dispatch ownership, ABI shape and ordinary Win32
availability because they are read directly from the cited OpenNT modules.
The historical hard-error delivery mechanics remain a linked DEM owner
dependency and require an explicit typed integration point during provider
implementation; this is a scoped implementation dependency, not a reason to
revive CCPU or to add Bochs semantics.

## Follow-up

T202 S4 implements one source-derived DEM file/handle/FCB/search provider and
its package regression.  It replaces the listed fixture/failure leaves rather
than accumulating a new recognizer.  A native trace remains prohibited until
the full owner package's direct/readonly/overlay and deferred-pipe regressions
are complete.
