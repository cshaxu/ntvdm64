# M0 T95 S5: Bounded Guest-Read Transport Record

## Completed increment

The generic adapter guest-read record now admits one bounded request of up to
128 bytes. This is the minimum transport change required by the S3 search
contract: NTDOS stages a canonical pathname in its 128-byte `OpenBuf`. The
limit is generic, remains fixed in the versioned adapter record, and does not
create a search-specific reader.

The same-island host session records the admitted completion owner as an
adapter-local enum. The Bochs-facing take/complete ABI still exposes only the
generic read action and bytes. In particular, neither `DEM_DTA` nor `SEARCH`
is added to Bochs, the #UD record, or a guest-visible ABI.

| Owner | Current completion behavior |
| --- | --- |
| `DEM_DTA` | Existing DTA registration completion is selected and tested. |
| `SEARCH` | Reserved only for the paired search service. No runtime dispatch queues it yet; an attempted unimplemented completion fails closed and clears the pending read. |

The selector happens before completion and is bound to the copied exception
boundary and CPU snapshot. Mismatched boundaries clear the pending read, as
they did before this increment. A caller cannot choose a different consumer
after Bochs has supplied opaque bytes.

## Evidence

From repository root:

```powershell
cmake --build artifacts/build/cli-component-layout-check --target `
  bx-ntvdm-guest-read-action-v1-test bx-ntvdm-host-session-test `
  bx-ntvdm-adapter-runtime-test
& .\artifacts\build\cli-component-layout-check\bx-ntvdm-guest-read-action-v1-test.exe
& .\artifacts\build\cli-component-layout-check\bx-ntvdm-host-session-test.exe
& .\artifacts\build\cli-component-layout-check\bx-ntvdm-adapter-runtime-test.exe
```

The 2026-08-10 run built all three targets successfully. The first emitted
`single bounded read contract verified`; the runtime test emitted
`identity-to-pending-transaction path verified`; the session test returned
zero without diagnostic output.

## Explicitly not completed by this increment

This does not implement any DOS search operation, wildcard matching, host
namespace lookup, DTA/FCB multi-write, cookie table, or Bochs change. The S3
trace gates remain in force: `SEARCHBUF` byte 52, FCB-next empty-result
behavior, and volume-label behavior must not be invented. The next S5 unit
must introduce the paired search session and its atomic outputs only after
those unresolved observations are either traced or fail-closed for the first
profile.

## Follow-on session-state increment

`bx_ntvdm_search_sessions_v1` is the admitted adapter-local continuation
store. Its `begin` operation copies a sequence of already projected and sorted
namespace entries into private storage. `next` requires the exact cookie/ID
pair and originating PDB; it cannot cause host re-enumeration. The last result
releases the snapshot and returns a zero token. `cancel` and adapter teardown
also release it.

The token is produced with the Windows system RNG and is accepted only while
the matching private slot remains live. No host handle, host path, or adapter
address is contained in it. This is state plumbing only: wildcard filtering,
service dispatch, and DTA/FCB atomic output remain the next coupled unit.

Additional reproduction command:

```powershell
cmake --build artifacts/build/cli-component-layout-check --target bx-ntvdm-search-session-test
& .\artifacts\build\cli-component-layout-check\bx-ntvdm-search-session-test.exe
```

The 2026-08-10 run built the target and exited successfully. Its fixture
proves first/next ordering, final-release behavior, a forged ID rejection,
cross-PDB rejection, one-result behavior, and explicit cancellation.

## Four-service direct-buffer audit and correction

The source call sites were re-read after the session store was introduced.
Each member has one contiguous *direct service buffer*:

| Service | Single generic input range | Bytes |
| --- | --- | ---: |
| `50:09` pathname first | `DS:DX`, NTDOS canonical `OpenBuf` | 128 |
| `50:0A` FCB first | `ES:DI`, NTDOS canonical `OpenBuf` | 128 |
| `50:0B` pathname next | registered DTA | 43 |
| `50:0C` FCB next | `DS:SI`, `SEARCHBUF` | 53 |

FCB first writes its `SEARCHBUF`; it does not need to read it first. This
does not, however, settle the complete continuation-lifetime input set.

The original `AddFFindEntry` binds each retained `FFINDLIST` to the current
PDB/PSP value. The DTA registration supplies the physical *location* of that
word, but not the value. It is independent of the path, DTA, and `SEARCHBUF`
ranges above. Treating its address as a PDB identity would silently change
process isolation semantics. Therefore the earlier conclusion that one read is
sufficient was incomplete and is withdrawn.

Before any search selector is dispatched, S5 must define a generic mechanical
gather-read record that can request the service buffer plus the bounded PDB
word in one copied boundary transaction. It must carry only fixed physical
ranges and bytes, have no service/DEM/DOS fields, and be usable by any future
adapter consumer. The existing one-range v1 transport remains valid for DTA
registration and does not change. No Bochs or search-specific exception is
authorized by this correction.
