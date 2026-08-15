# T220 S2: bounded DEM process-resource read design

## Decision

The existing source-derived `50:1B` (`demSetDTALocation`) registration is the
only admissible bootstrap for the first process-owner read.  It already
records copied guest physical locations for `CurrentPDB` and the SFT chain
head, without retaining guest or host pointers.  T220 must reuse that
registration; it must not add an independent PSP scanner, infer ownership
from `50:36`, or add a Bochs API.

## Original layout evidence

`src/opennt/base/mvdm/inc/doswow.h` is packed and defines:

| Structure / field | Offset | Bytes | First use |
| --- | ---: | ---: | --- |
| `CurrentPDB` value published by `demSetDTALocation` | registered guest address | 2 | `*pusCurrentPDB` in `demfile.c` |
| `DOSPDB::PDB_JFN_Length` | 50 | 2 | validate a requested DOS handle |
| `DOSPDB::PDB_JFN_Pointer` | 52 | 4 | locate exactly one JFT entry |
| JFT entry | requested handle index | 1 | reject `FFh`; derive SFT number |
| `DOSSF::SFLink` / `SFCount` | 0 / 4 | 4 / 2 | bounded SFT-chain walk |
| `DOSSFT::SFT_Ref_Count` | 0 | 2 | reject a free SFT |

The first implementation does **not** need to reproduce the handle lookup.
It needs only the first row: an exact two-byte copied `CurrentPDB` snapshot
to associate an already accepted direct DEM resource with the current DOS
process.  This is sufficient to correct the current session's missing
process-owner field.  PDB/JFT/SFT lookup is reserved as a separately admitted
follow-up only if a reached original caller requires guest-handle validation.

## Narrow ABI

`bx_ntvdm_dem_process_owner_v1` will be a DEM-private, fixed-width helper.
It accepts an existing valid `bx_ntvdm_dem_dta_registration_v1`, produces one
checked read request of exactly two bytes at `registration.current_pdb`, and
accepts only an exact copied completion.  Its output is:

```text
status: absent | read-required | valid-owner | malformed
pdb_segment: uint16
```

It has no selector, service, CPU state, BOP result, host handle, guest
pointer, callback, or policy decision.  The invoking DEM file/search provider
owns when the read is requested and how a malformed/absent result becomes the
original source-shaped failure.  The helper never reads PDB/JFT/SFT fields
itself in its first form.

## Validation and failure rules

1. A registration is absent when `current_pdb == 0`; the helper returns an
   explicit absent status and emits no read.
2. The two-byte range must be wholly below the real-mode 1 MiB aperture.
3. Completion must match the exact address, byte count and request identity;
   any mismatch is malformed, with no owner value published.
4. `PDB == 0` is malformed for an active resource association.  A caller may
   convert this to its existing source-derived failure but may not attach the
   resource to a global/default owner.
5. The helper copies the little-endian segment value only.  No raw guest
   pointer, guest buffer or guest-memory capability escapes its result.

This retains the earliest original rule—`pusCurrentPDB` is a guest-published
word—and defers the later original PDB/JFT/SFT rules until they are actually
needed.  It is not a simplification of those later rules.

## Shared-profile relation

The owner segment is process identity only.  Direct/readonly/overlay/virtual
decisions remain in the shared mutation-profile consumer at the actual file,
search or FCB operation.  No profile can manufacture or rewrite a PDB owner.

## Focused fixture

The fixture must cover: valid little-endian owner; absent registration; zero
owner; range overflow; completion with wrong range; completion with wrong
byte count; and no raw pointer/host-handle field in the public record.  It is
purely source-built and does not bind a BOP, run a native trace or access
Bochs.

## Result

The design proves a separately bounded implementation is possible without
generic guest inspection.  T220 S3 may implement exactly this two-byte helper
and fixture.  It may not attach it to DEM operations or alter `50:3C` until a
following lifecycle-integration S is admitted.
