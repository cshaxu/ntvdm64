# T220 S4: direct DEM process-resource transaction map

## Scope

This map covers only the current direct namespace-handle family: DEM Open
`50:12`, Create `50:03`, and CreateNew `50:22`.  It deliberately excludes
readonly declared images, FCB state, pathname search state, Redirector and
device/VDD packages.  Those resources do not share the current token owner.

## Original ownership and failure facts

| Original fact | Required retained meaning |
| --- | --- |
| `demgset.c::demSetDTALocation` publishes `pusCurrentPDB` as a live guest word. | The owner is read from the guest at the transaction point; `50:36 DX` is not the owner source. |
| `demfile.c::VDDAllocateDosHandle` reads the current PDB before allocating JFT/SFT state; failure to find a free slot happens before the handle is returned. | A modern direct token must not become visible before the owner snapshot succeeds. |
| `demfile.c::demOpen` and `demCreateCommon` obtain the host handle, reject size/error cases and publish `AX:BP` only on success. | A post-open owner failure must close/rollback the host handle and return the existing source-derived BOP failure, never return an ownerless token. |
| `demfile.c::VDDReleaseDosHandle` validates the owner-selected DOS-handle state, then releases its entry. | Normal close remains token-specific and cannot release another PDB's resource. |
| `msctrlc.asm` calls `SVC_PDBTERMINATE` only for a genuine child removal, excluding parent/mismatch and TSR/keep-process paths. | PDB termination must select only resources whose copied owner equals `BX`; it is never a global session teardown. |
| `demsrch.c::demTerminatePDB` is `VOID` and orders VDD hook, host-device cleanup and per-PSP search-list release. | Direct local-file cleanup is a required source-derived compatibility seam because current tokens bypass the old DOS SFT host-handle representation; it must be recorded separately and preserve the no-delta lifecycle BOP result. |

## Current transaction gap

`bx_ntvdm_dem_namespace_partition_v1` calls
`bx_ntvdm_dem_local_file_backend_v1_open_ex`, which immediately calls
`bx_ntvdm_dem_file_session_v1_adopt`.  The resulting opaque token is then
published in the BOP result.  The token slot currently stores only handle,
generation and in-use.  Therefore there is no point at which current code can
prove the DOS process that owns the live host handle, and `50:3C` cannot
release it safely.

## Required transaction

```text
guest pathname copied and validated
  -> request exact CurrentPDB word via existing 50:1B registration
  -> absent/malformed PDB: source-derived failure, no host open
  -> profile decision + host open/create
  -> adopt token with copied PDB segment in the same session commit
  -> publish guest token/result

ordinary Close(token)
  -> validate token generation and close that token only

50:3C(BX=PDB), only after guest DOS's real-child guard
  -> release tokens whose copied PDB == BX
  -> retain VOID / RIP+4 contract
  -> leave foreign-PDB, parent and TSR resources untouched
```

The first `CurrentPDB` read must precede host open in the current rehost.
This is stricter than merely rolling back an already-opened handle and matches
the original `VDDAllocateDosHandle` allocation ordering as closely as the
modern token seam permits.

## Profile boundary

The owner read is profile-neutral.  Direct/readonly/overlay/virtual policy is
evaluated only by the existing profile consumer before the host operation.
An unsupported profile refuses the namespace operation without an owner
association, and no profile can fabricate a PDB value.

## Minimum implementation seam

The next S may modify only the direct file-session/local-file backend and the
direct namespace open/create transaction:

1. add a copied 16-bit owner field to a live token slot;
2. require a valid owner as an explicit `adopt` input;
3. add `release_owner(pdb)` that closes matching live slots and never touches
   foreign slots;
4. add a transaction adapter that invokes T220 S3's exact two-byte helper
   before `open_ex` for `03/12/22`.

It may not connect `release_owner` to `50:3C` yet.  First, a focused fixture
must demonstrate create/commit rollback, stale-token preservation, ordinary
close, matching-PDB release, foreign-PDB preservation and all four profile
dispositions.  A subsequent lifecycle S will be needed to bind the proven
transaction to the existing no-delta `50:3C` path.

## Decision

This is one whole direct namespace-handle package, not three BOP leaf tasks.
The source-first ladder remains at rung 2: the original source cannot compose
without CCPU/SAS/VDD, while the minimum rehost preserves its owner-before-
visibility and PDB-scoped release rules.  No Bochs change is indicated.
