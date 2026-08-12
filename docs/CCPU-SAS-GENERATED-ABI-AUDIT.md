# CCPU SAS Generated ABI Audit

Status: source-input admission and object-closure evidence, 2026-08-06.

## Question

Can the fixed OpenNT CCPU source set be compiled with one of the generated
`sas4gen.h` files retained by the source snapshot?

Answer: the exact generated output is absent from the snapshot. The owner has
authorized the separately manifested OpenNT-input-plus-NTVDMx64-patch recipe
described below for compile/link-dependency analysis only. It is not a
replacement SAS interface or runtime-admission evidence.

## Direct Evidence

The archived CCPU `sources` file defines the historical profile:

```text
C_DEFINES= -DCPU_30_STYLE -DCPU_40_STYLE -DNEW_CPU -DCCPU -DSPC386 -DANSI -DNTVDM
INCLUDES=...\\host\\genPg\\inc
```

The referenced `host/genPg/inc` directory is absent from the fixed upstream
tree. The following retained generated inputs are therefore only nearby
variants, not evidence of the required profile:

| Input | `SasVector` fields | Backward move fields | `Sas_touch` and `SasVirtualiseInstruction` | `IsPageInstanceData` |
| --- | ---: | ---: | ---: | ---: |
| `base/cvidc/sas4gen.h` | 55 | 3 | absent | absent |
| `host/inc/mips/prod/sas4gen.h` | 55 | 0 | present | present |
| archived probe-v2 `base/inc/sas4gen.h` | 55 | 0 | present | present |
| CCPU `sasCdef.c` initializer | 57 | 3 | present | absent |

`ccpu386/sasCdef.c` is itself marked generated and initializes the three
backward move entries plus `c_sas_touch` and `c_VirtualiseInstruction`; it
does not initialize `IsPageInstanceData`. Thus neither retained 55-field
header can represent its ABI. The old probe-v2 header is byte-identical to
the retained platform variant and is diagnostic staging material, not an
independent historical input.

## Consequences

Using the `cvidc` variant makes the final two CCPU initializer fields
incompatible with its function table. Using the platform variant shifts every
field after the forward-move block and adds an instance-data slot CCPU does
not initialize. A permissive compiler may merely warn about this; that is not
valid object-closure evidence and must not be linked or executed.

Before the owner decision, the CCPU CMake gate was deliberately blocked at
`sasCdef.c` until one of the following was obtained with provenance:

1. the original `host/genPg/inc/sas4gen.h` generated for the recorded CCPU
   profile; or
2. the original generator plus its profile inputs, capable of reproducing the
   57-field output byte-for-byte or with a documented semantically equivalent
   result.

No unreviewed structural merge or compatibility shim is acceptable. The
owner-authorized overlay is the sole exception: it is mechanically derived
from the recorded OpenNT input and fixed NTVdmx64 patch recipe, lives under
the original x86 relative path, and is constrained to the default-off CCPU
object gate. This does not affect the independently successful BIOS/BOP/DEM
host-stack object gate.

## Upstream Object Database Check

The fixed OpenNT clone retained 61 unreachable historical commits in its
read-only Git object database on 2026-08-06. Every one was traversed with
`git ls-tree -r` for `host/genPg`, `sas4gen.h`, `sasCdef.c`, and `MkCpuInt`.
No `host/genPg` directory or generator source was present. The three relevant
available blobs were also identical in all 61 commits:

| Path | Git blob ID |
| --- | --- |
| `base/mvdm/softpc.new/base/ccpu386/sasCdef.c` | `4d6959da16e7cb1edac51ecb8ef2ffdcc52275bb` |
| `base/mvdm/softpc.new/base/cvidc/sas4gen.h` | `1273a85466dce6013d86d09ddde620edecd06165` |
| `base/mvdm/softpc.new/host/inc/mips/prod/sas4gen.h` | `4f3803b41f8553abb6868e95aa5dfe2a53eec6d3` |

This rules out an omitted commit in the locally fixed upstream history. It
does not establish the missing CCPU generator output; a separate provenance
source remains necessary before runtime admission.

## NTVDMx64 Patch Corroboration

The independently imported NTVDMx64 source tree contributes a stronger
description of the missing x86 layout, but not the missing generated file
itself. Its tracked
`ntvdmpatch/patches/common/sas4gen.patch` has the original input path:

```text
NT4\private\mvdm\softpc.new\host\inc\x86\prod\sas4gen.h
```

and records these additions to that x86 `prod` header:

1. `Sas_touch` immediately after `Sas_PigCmpPage`;
2. `SasVirtualiseInstruction` immediately after `IOVirtualised`; and
3. a trailing `IsPageInstanceData` entry.

This is meaningful because the retained CCPU initializer has exactly the
first two additions while retaining the three backward-move entries found in
the base `cvidc` header. It has no initializer for `IsPageInstanceData`; under
the historical C aggregate rules a trailing field could be zero-initialized,
but that observation is *not* evidence that the resulting object is valid to
run.

The retained MIPS header is not a suitable substitute: it contains the patch
additions but replaces the three backward-move slots, so its field positions
from the forward-move block onward are incompatible with CCPU. The former
table's ``0`` values therefore mean "no backward move fields in that variant",
not that the source snapshot lacks their declarations altogether.

This patch supplies provenance for the required *shape* of the approved x86
header overlay. It still does not recover the exact `host/genPg/inc` output or
the generator/profile inputs. Therefore its successful compile result remains
strictly object-closure evidence; no runtime target may consume it without a
separate link and runtime verification record.

## NTVDMx64 Patch Recipe (Further Evidence)

The fixed NTVDMx64 input carries more than a field-level diff. Its tracked
`ntvdmpatch/patch.cmd` records the construction sequence used for its x86
historical build input:

1. create `host/inc/x86/prod`;
2. copy `base/cvidc/sas4gen.h` into that directory; and
3. apply `patches/common/sas4gen.patch` to the copied x86 header.

This is a reproducible *input-plus-patch* recipe, not a claim that the
resulting file is the missing OpenNT `host/genPg/inc` generated output. The
patch adds all three x86-specific slots: `Sas_touch`,
`SasVirtualiseInstruction`, and trailing `IsPageInstanceData`. Therefore the
result has 58 table fields. `sascdef.c` supplies the first 57 values; ordinary
C aggregate initialization would zero-initialize the trailing instance-data
field. That explains the observed source-level shape, but does not establish
that a zero implementation is safe for execution.

The owner authorized this recipe on 2026-08-06. It is materialized as
`overlay/base/mvdm/softpc.new/host/inc/x86/prod/sas4gen.h`, listed in
`SOURCE-MANIFEST.json` with SHA-256
`B227A02ECD0A36FE422758502BA27786A441970691F96F0D36EF53D9E10D8A28`.
Its first and only currently allowed use is CCPU **object and link-dependency
analysis**. It remains excluded from the default runner and cannot be used as
runtime-admission evidence until the unresolved CCPU/BIOS/DEM dependencies and
the `IsPageInstanceData` behavior have their own verification records.

## Reproduction

On 2026-08-06, the standalone OpenNT gate was configured with clang-cl 22.1.8
under `vcvarsall.bat x86` and the recorded CCPU profile. Building
`ntdos64-opennt-ccpu-objects` reached the generated `sascdef.c` unit and
failed only with:

```text
sascdef.c:77: error: unknown type name 'TYPE_sas_touch'
sascdef.c:79: error: unknown type name 'TYPE_VirtualiseInstruction'
```

The full diagnostic is build evidence at
`artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/ccpu-gate.log`. The target has no link
action, so the attempt neither produced nor executed a historical runtime.

## Authorized Overlay Object Closure

With the authorized x86 overlay selected only for
`ntdos64-opennt-ccpu-objects`, clang-cl/i686 compiled the 130-unit CCPU source
set successfully. The final record is
`artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/ccpu-overlay-gate.log`; it ends with
`Built target ntdos64-opennt-ccpu-objects` and 365 legacy warnings, with no
compiler errors. The first post-SAS compiler error was a separate duplicate
declaration in `zfrsrvd.c`: `Length_of_M_area` used `PHY_ADDR` while the
retained OpenNT `sas.h` declares `IHPE`. The owner-authorized local overlay
changes only that declaration to `IHPE`, matching `sas.h` and the existing
`ccpusas4.c` definition. It is separately manifested with SHA-256
`62EE006AF8AC54C3E3DC48A1C53BE98626C6999BCBC308B3EDFE1DB2404A5DA7`.

This proves source/object closure, not binary linkage, interpreter execution,
the behavior of a zero-initialized `IsPageInstanceData` slot, or DOS startup.
