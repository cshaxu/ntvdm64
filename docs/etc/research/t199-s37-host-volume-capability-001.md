# T199 S37 Host Volume Capability 001

## Purpose

OpenNT `demgset.c` obtains free-space and media metadata through the NT host.
The CLI composition must not query those APIs from a BOP request, expose a
host path or handle to the guest, or fabricate volume data from a drive type.

## Boundary

`bx_ntvdm_host_volume_snapshot_v1` is an adapter-local immutable value:

- input: the already policy-filtered `bx_ntvdm_host_drive_snapshot_v1`;
- setup-only capture: volume serial, label, filesystem, and cluster geometry;
- BOP-time use: reads the immutable value only;
- output: no root path, Win32 handle, or API object crosses into guest memory
  or bx-core/bx-mantle.

An unavailable admitted volume is represented explicitly as an unavailable
record. A non-admitted record is discarded by `apply`, preventing accidental
ambient drive capability.

## Source and acceptance

- Original semantics: `src/opennt/base/mvdm/dos/dem/demgset.c`, especially
  `demGetDriveFreeSpace` and `demGSetMediaID`.
- Capability implementation:
  `src/bx-vdm/bx_ntvdm_host_volume_snapshot_v1.*`.
- r31 source-built MSVC x64 `/MT` regression constructs a C: record, proves
  its retained metadata, and proves a D: record is discarded when D: is not
  admitted. It does not invoke a native trace.

## Follow-on

The r32 DEM package regression now consumes an admitted volume record through
`50:0E demGetDriveFreeSpace`, returning the source-shaped AL/BX/CX/DX/SI
contract after 16-bit DOS cluster reduction. r34 also consumes it through
`50:10 demGSetMediaID`: checked writes fill the serial, eleven-byte volume
label, and eight-byte filesystem fields of `VOLINFO`; the set branch retains
OpenNT's CF-only unsupported result. r35 adds an explicit set-form regression
(AL nonzero, no GPR write, CF set) and declares the corresponding pre-bind
copy setter on the formal `bx_ntvdm_boot_namespace_composition_v1` boundary.
That setter is compiled into the source-built closure; runtime attachment
remains a separate CLI startup-composition task, not an ambient BOP-time
capture. Machine-name policy and single-DPB layout remain separate source/ABI
decisions.
