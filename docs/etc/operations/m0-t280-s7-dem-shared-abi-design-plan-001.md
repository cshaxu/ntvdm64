# M0 T280 S7 — DEM shared ABI recovery design

T280/S6 proved that the next recovery unit is not a service handler.  It is
the shared compilation and ABI form used by all sixteen original DEM bodies.
S7 creates the source-first decision record required before any matching
adapter or platform declaration is implemented.

## Required source-first questions

For each shared family, S7 records the exact OpenNT source/header definition,
the historical compile condition and build ordering, each DEM consumer, the
current x86/x64 collision or absence, and one future owner:

1. `DEVL` diagnostic declarations (`demexp.h`, `demDebugBuffer` visibility)
   and their exact historical non-debug form;
2. `GetDiskSpaceInformationA` and the modern x86 SDK same-name collision;
3. `MAXIMUM_FILENAME_LENGTH`, `PFILE_BOTH_DIR_INFORMATION`, file-system
   device/object records and NTSTATUS values;
4. `MEDIA_TYPE`, `DISK_GEOMETRY` and the reached storage IOCTL declarations;
5. each reached SoftPC/CCPU `GetVDMAddr` or related guest-memory operation.

## Mandatory mapping rule

Only family 5 receives a mapping-manager disposition.  The original numeric
guest address remains a 16/32-bit guest value, resolved by the session
`guest_memory` mapping-manager instance into a bounded synchronous lease in
`adapter-softpc`, then a typed `adapter-bochs` operation.  Host-local
temporary pointers and records in families 1--4 remain outside all mapping
manager instances.

## Non-goals

No original body edit, no include guessing, no adapter implementation, no
private kernel VDM call, no runtime provider, no BOP ingress and no guest
trace.  A missing declaration must be classified from original headers before
the next S may add it.

## Exit criteria

One source/consumer/shape/failure ledger covers all five families, including
the x86 collision and each original header path.  It names the smallest
permitted follow-on action—exact mirror import, same-shaped adapter binding,
or adapter-softpc mechanical seam—without creating a generic compatibility
layer.
