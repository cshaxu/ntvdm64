# T225 S8 Primary NTIO Source-Build Result 001

## Question

Can the primary NTIO input be recreated from the managed OpenNT source closure
and pinned original tools16 toolchain without using a prebuilt NTIO image?

## Procedure

`tools/build/Build-T225S8PrimaryNtio.ps1` created a fresh manifest-bearing
stage below `build/M0-T225-S8/`.  It copied only the managed NTIO, V86 include,
MVDM include, and message source inputs; removed copied generated message
classes; then ran the original tool sequence:

1. OpenNT `BUILDIDX` and `NOSRVBLD` under DOSBox;
2. OpenNT MASM over the twelve original `msbio_4.lnk` module members;
3. OpenNT LINK16 with the unchanged response file; and
4. OpenNT `RELOC NTIO.EXE NTIO.SYS 70` under DOSBox.

The tool verifies all regenerated class hashes before assembly and refuses an
output identity mismatch.  No prebuilt `.OBJ`, `.EXE`, or `.SYS` is copied into
the stage.

## Observation

The fresh scripted stage produced:

| Property | Value |
| --- | --- |
| Artifact | `base/mvdm/dos/v86/doskrnl/bios/NTIO.SYS` |
| Bytes | 33,792 |
| SHA-256 | `cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937` |
| Result | exact locked primary identity |

Its `ntio-build-manifest.json` records the OpenNT revision, empty guest-source
transform list, all five tool hashes, the twelve-module order, and output
identity.

## Interpretation

NTIO is again available as a physically fresh, manifest-bearing,
primary-original-toolchain source-built input.  It remains distinct from the
approved reference-binary sequence even though the bytes are equal.

## Boundary

This is source-build provenance only.  It does not run the four-image fixture,
guest code, BOP, Bochs, or CLI runtime.  NTDOS, COMMAND, and SHARE still need
their own fresh primary stages before S8 cross-validation is admitted.
