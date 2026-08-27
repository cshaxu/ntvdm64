# M0 T282 S9 — COMMAND environment guest-memory seam audit

## Question

Can the selected original COMMAND environment/current-directory group use the
existing session guest-memory mapping manager without reviving the broad
historical `GetVDMAddr` pointer model?

## Inputs and procedure

- Exact original `dos/command/cmdmisc.c` and `cmdenv.c` pointer sites.
- Existing `adapter-softpc/mvdm_guest_location.{h,c}` bounded descriptor and
  lease facade, which resolves `segment << 4 + offset` through the
  thread-bound session's sole `guest_memory` mapping-manager instance.
- T280 S20 design/evidence establishing that a native pointer cannot outlive a
  synchronous exact lease.
- The [four-record seam ledger](../operations/m0-t282-s9-command-environment-guest-seam-ledger-001.tsv), checked by
  `Test-T282CommandEnvironmentGuestSeam.ps1`.

## Observations

- `cmdComSpec` has a bounded destination but its original `strcpy` reads an
  unbounded guest string. Its exact safe input extent is 64 bytes, including
  the terminating NUL.
- `cmdGetCurrentDir` uses its guest `DS:SI` address only as an output buffer;
  the source caps the OEM result at 67 bytes. A host temporary buffer can
  preserve source ordering without passing guest memory to Win32/OEM APIs.
- `cmdGetInitEnvironment` knows the exact required multisz byte count before
  writing it, but historically resolves ES:0 before checking the BX paragraph
  capacity. The modern lease must be acquired only after that source check.
- The helper functions that synchronize `VDMINFO` current directories and
  transform environment state use host-local data at their own call sites;
  they need no guest mapper.

## Interpretation

The existing `mvdm_guest_location` bounded lease primitive is the correct
single session-owned guest-memory mapping path. It must be reused; a second
COMMAND mapper or a naked `GetVDMAddr` compatibility macro is prohibited.

The later behavior slice needs small registered mirror divergences only at the
three pointer operations: acquire/copy/release around the known exact spans.
The original service ordering, layout, capacity responses and register/error
branches remain the source owner. No adapter may infer a string length or map
an arbitrary guest range before the original logic has supplied its bound.

## Follow-up

The next S may implement the complete environment/current-directory group
only after the required source-shaped adapter-softpc copy helpers and original
register facade are audited together. It must not enable `54:02`, `54:04` or
`54:0F` independently.
