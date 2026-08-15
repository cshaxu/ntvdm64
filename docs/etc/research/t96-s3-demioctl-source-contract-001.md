# M0 T96 S3 DEMIOCTL Source Contract 001

## Question

Can the existing adapter-local `50:21`, `AX=4408` responder become the DEM
IOCTL provider, and which portions of original `demioctl.c` are actually
reachable from the DOS kernel and composable in the non-invasive CLI profile?

## Inputs

The primary historical source is the pinned OpenNT tree. Its relevant input
files and SHA-256 values recorded on 2026-08-11 are:

| File | SHA-256 | Role |
| --- | --- | --- |
| `refs/opennt/base/mvdm/dos/dem/demioctl.c` | `6F480BF758D071B1FD2F76A137F892B77D1EA8D2D8FFB4690D395AEDF5F733FA` | Host DEM selector table and handlers. |
| `refs/opennt/base/mvdm/dos/dem/dosdef.h` | `82522742F0111CAC59FEADABA4C1216C39D72C9AC77473FB0061AD29E5B8A8A0` | AL=08/09 definitions. |
| `refs/opennt/base/mvdm/dos/dem/demdasd.h` | `DE5BD5E437C17A34AF71999E6E6E7C0E749D9D9DA61BC5CC8420C0406E1A2C35` | Generic block-IOCTL codes and structures. |
| `refs/opennt/base/mvdm/dos/v86/doskrnl/dos/ioctl.asm` | `8DC2168299012C24A663035CA655AC14169DBEE494858CBDC895E60DF7B8D8FA` | Guest DOS caller and BOP reachability. |

The old trace-only conclusion is retained in
`t95-s7-demioctl-4408-trace-audit.md`; this record supersedes it for provider
composition. The frozen legacy responder is
`src/bx-ntvdm-adapter/bx_ntvdm_dem_ioctl_changeable_service.c`; it is audited
evidence, not the S3 provider implementation.

## Procedure

1. Read the `apfnSVCIoctl` table and each of `demIoctlChangeable`,
   `demIoctlDiskGeneric`, `demIoctlDiskQuery`, and `demIoctlInvalid`.
2. Read the guest DOS `IOCTLJMPTABLE` and its `HRDSVC SVC_DEMIOCTL` call
   sites, rather than inferring BOP reachability from the host table alone.
3. Trace every original branch's register and guest-memory/host dependency to
   an adapter owner and a current profile disposition.

## Observations

`demIOCTL` selects solely on AL. Its host-side table contains the following
complete range (AL beyond 11 is debug-checked invalid in this source):

| AL | Original handler | Guest DOS BOP reachability | Contract and S3 disposition |
| ---: | --- | --- | --- |
| 00--07 | `demIoctlInvalid` | No; guest DOS handles these locally. | Original CF=1, AX=`ERROR_INVALID_FUNCTION`; do not expose a direct-BOP compatibility shortcut. |
| 08 | `demIoctlChangeable` | Yes. | BL is zero-based drive. Returns AX=0 for removable and AX=1 otherwise; CF clear. It first uses `demGetPhysicalDriveType`, then `GetDriveTypeOem` when unknown. Candidate only with an immutable projected-drive type snapshot. |
| 09 | `demIoctlChangeable` | Yes. | Same BL lookup. AL becomes 0; DX=1000h for remote/CD-ROM, otherwise 0800h; CF clear. It is not the old record's "always local" simplification. Candidate only alongside AL=08 in one provider slice. |
| 0A--0C | `demIoctlInvalid` | No; guest DOS handles the corresponding handle/device operations. | Original CF=1, AX=`ERROR_INVALID_FUNCTION`; no adapter implementation. |
| 0D | `demIoctlDiskGeneric` | Yes. | BL/CL plus SI:DX parameter blocks. Branches perform BDS state changes, track reads/writes/format, guest-memory parameter mutation, floppy/fdisk access, `CreateFileA`/`DeviceIoControl`, and host error translation. Explicitly deferred: raw devices and mutable block media are outside the profile. |
| 0E--10 | `demIoctlInvalid` | No; guest DOS handles or rejects these locally. | Original invalid contract; no adapter implementation. |
| 11 | `demIoctlDiskQuery` | Yes. | BL/CL query of BDS-backed generic-block support. It reports only a subset of the mutable/raw block operations as supported. Deferred with AL=0D; returning success without the matching block provider would be false capability. |

For AL=0D, the generic-code table is also whole rather than a safe read-only
subset: 40h set parameters, 41h write track, 42h format, 46h set media, 47h
set access, 60h get parameters, 61h read track, 62h verify, 66h get media,
67h get access, and other values fail `DOS_INVALID_FUNCTION`. Even its
apparently read-oriented cases require BDS and checked guest structures;
there is no admitted `SI:DX` copy ABI for them yet.

The guest `ioctl.asm` sends only AL=08, 09, 0D, and 11 through
`SVC_DEMIOCTL`. The historical host table's invalid entries therefore must
not be mistaken for a request to move guest-DOS IOCTL handling into adapter.

## Interpretation

The source supports a narrow, coherent future provider subplane, not a
single `4408` exception:

- one **projected-drive metadata** slice owns AL=08 and AL=09 together, using
  only a frozen drive-type snapshot and exact register/CF results;
- one **raw block-media** slice owns AL=0D and AL=11 together, but remains
  deferred until BDS, guest multi-range copying, error translation, and a
  deliberately admitted block-media capability exist;
- all other AL values remain guest-DOS owned or source-invalid, never an
  adapter success path.

The old 4408 responder has the right broad observation (C: fixed produces
AX=1), but it lacks AL=09 and is a legacy runtime recognizer. Under T96 it
must remain frozen until S8 migrates it through common ingress/selection into
the metadata slice. No new `adapter_runtime_v2_dispatch` condition is
permitted.

Confidence is high for selector, reachability, registers, and the raw-media
blocker because all come directly from the four pinned files. It is medium
for eventual CLI semantics: the immutable drive snapshot exists, but the
complete provider migration and its negative tests are not yet admitted.

## Follow-up

Treat `demioctl.c` as a closed source-contract group for S3. The next DEM
source review may choose another component group. S8 is the only place to
migrate the legacy AL=08 responder; it must add paired AL=09 behavior and
negative tests for AL=0D/11 before claiming an IOCTL provider.
