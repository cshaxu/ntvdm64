# T198 S20 Native DEM Load-DOS Witness 001

## Question

Can real-mode `C4 C4 50 11` publish an immutable NTDOS image at live
`DI:0000` through the single current `bx-vdm` composition and resume into the
published bytes without giving Bochs BOP meaning?

## Source Basis And Contract

`msinit.asm:247-252` sets `DI` from `dos_load_seg`, calls
`SVC_DEMLOADDOS`, then transfers to `sysinit`; `dossvc.h:109` names service
`0x11`.  `demmisc.c:152-207` identifies `DI:0000` as the NTDOS destination
and makes failure non-returning.  S19 records why its historical
`DemDispatch`/CCPU/SAS/Win32 composition is not a runtime-link input.

The new adapter-local provider accepts only copied real-mode `50:11`, a
non-empty same-island `byob_image`, and a matching profile descriptor length.
It derives `low16(EDI) << 4`, preflights the whole payload against the
one-MiB aperture, and emits one mantle-owned checked write action.  Success
resumes at `fault_rip + 4`; every exact-request validation failure is typed
controlled stop with no action.  It has no host path, HANDLE, file read,
CCPU/SAS pointer, original dispatcher, or Bochs API.

`dem_plane_v1` was also corrected to classify `0x11` with the original NTDOS
namespace owner rather than MISC.  That classification caused the initial
pass-through; no ingress special case was introduced.

## Verification

The fresh MSVC x64 `/MT` C closure at
`artifacts/build/t198-s20-bx-vdm-x64-r4` exits zero with only `bcrypt.lib`.
Its extended session fixture verifies a valid `DI=0800h` publication at
`0x8000` and typed resume; a descriptor/image-length mismatch yields STOP and
leaves the independently addressed `0x9000..0x9002` unchanged.  The finite
runner, generic bridge and mantle-action boundary checks pass.

The CPU5 native fixture uses a deterministic `0x6cd2`-byte test image whose
first byte is `HLT` (it is an execution witness, not a production artifact).
It executes `mov DI,08B0h; C4 C4 50 11; jmp far 08B0:0000`.  The full CPU5
source closure rebuilt at `artifacts/build/t198-s20-native-demload-r1`; the
same objects were linked through `native-demload.rsp`, excluding only the
adapter test object that otherwise duplicates `main` during the generic
directory-expansion probe.  `t198-s20-native-demload.exe` exits zero, proving
`#UD -> provider -> checked RAM write -> resume -> guest fetch`.

## Limits

This does not load a production BYOB artifact from disk in the fixture, start
NTIO, perform `sysinit`, answer selector `12`, or prove DOS/COMMAND
continuity.  Production binding still requires `byob_image_load_exact` before
composition binding.
