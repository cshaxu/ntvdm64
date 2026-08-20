# M0 T234 S1 — 32-Bit Opaque Host-Handle ABI Result

## Question

Can the adapter keep OpenNT DEM's `AX:BP` and COMMAND's `BX:CX` token
layouts fully 32-bit while preventing native x86/x64 host `HANDLE` values
from entering guest state?

## Inputs

- Original OpenNT DEM and COMMAND mirror call sites.
- T232 manager and `BX-VDM-001` exception registration.
- `bx_ntvdm_host_handle_manager`, DEM direct-session/shim, COMMAND misc shim,
  and their formal focused fixtures.

## Source-Recovery Disposition

1. Direct raw OpenNT composition is unavailable: it assumes an NT4-sized
   `HANDLE` is safe to place directly in the 32-bit guest-facing pair.
2. The smallest compatible seam is a same-width opaque-ID manager. Original
   provider translation units, dispatch order, register-pair positions, and
   error owners remain unchanged.
3. No Bochs or OpenNT source intrusion is used.
4. No new BOP or DOS behavior is authored.

## Procedure

1. Changed manager IDs and both indexes from `uint16_t` to `uint32_t` and
   bumped its ABI version.
2. Reserved `0` as invalid and `UINT32_MAX` as the existing COMMAND
   no-redirection/default-standard-handle sentinel.
3. Made DEM decode and encode the full `AX:BP` ID; made COMMAND encode and
   decode the full `BX:CX` ID and its 32-bit stack records.
4. Updated all non-v1 product fixtures to use the same manager surface.

## Observations

- DEM's direct imported `demhndl` fixture accepts the live opaque token
  `0x00010001` across `AX:BP`; a distinct unknown high-word token follows its
  ordinary invalid-handle path.
- DEM FCB/search fixture opens at `0x00010000` and carries both halves through
  the original call shape.
- COMMAND console fixture publishes `BX:CX = 0001:0000`; lifecycle fixture
  uses that token successfully for inherited standard output and rejects an
  unknown 32-bit token using the pre-existing COMMAND result convention.
- The manager fixture verifies zero and `UINT32_MAX` rejection, allocation at
  `0x00010000`, and exhaustion at the reserved sentinel without requiring
  kernel-handle exhaustion.
- Seven focused formal targets source-built and exited zero under MSVC x64
  `/MT` in `build/M0-T232-S1/formal-r1`.
- The complete formal Ninja graph then rebuilt and linked 34 affected actions
  successfully from the same verified build root.

## Interpretation And Confidence

The active adapter now differs from OpenNT only in replacing a raw 32-bit
host-handle bit pattern with a same-width opaque 32-bit ID. This removes the
unnecessary prior 16-bit ABI narrowing while retaining native-width safety on
both x86 and x64 hosts. `BX-VDM-001` remains necessary because OpenNT itself
did not own this map.

## Follow-up

Close T234 only after the full formal graph and documentation inventory gate
are recorded. A future combined DEM/COMMAND runtime must inject one manager
instance; it must not create another token table.
