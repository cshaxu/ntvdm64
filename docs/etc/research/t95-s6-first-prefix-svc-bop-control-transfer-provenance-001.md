# T95 S6 First-Prefix SVC/BOP Control-Transfer Provenance 001

## Scope and method

This is a source-and-preserved-artifact correlation only. It neither reruns
the r6 companion nor changes Bochs, OpenNT, guest inputs, configuration, or
the adapter. The inputs are the accepted source-built NTIO first-prefix
evidence, the pinned OpenNT and Bochs sources, and the copied r6 build input.

## Reached NTIO transition

The first-prefix source path in
`src/opennt/base/mvdm/dos/v86/doskrnl/bios/msinit.asm:247-252` ends its setup
with:

```asm
mov di,dos_load_seg
SVC SVC_DEMLOADDOS        ; di is segment to load DOS
                            ; If it fails it never comes back
jmp sysinit
```

`src/opennt/base/mvdm/inc/DOSSVC.INC:33` assigns
`SVC_DEMLOADDOS = 11h`. `src/opennt/base/mvdm/inc/BOP.INC:1-20` assigns
`BOP_DOS = 50h`, defines a BOP as `C4 C4 <number>`, and makes the `SVC` macro
use that BOP followed by its service byte. Thus this call site encodes the
four-byte host transition:

```text
C4 C4 50 11     BOP_DOS / SVC_DEMLOADDOS
```

The preserved source-built NTIO image contains that exact sequence at file
offset `0x475`; with the admitted physical placement at `0x700`, it is at
physical `0xb75`. The following bytes are `EA F9 02 0D 01`, the far jump to
`0d01:02f9` (`sysinit`) that is reached only if the service returns. This
matches the source comment: loading NTDOS is not an optional later DOS
feature; it is a required first-prefix transition before `sysinit`.

## Historical owner and current Bochs disposition

The original SoftPC selector table at
`src/opennt/base/mvdm/softpc.new/base/bios/bios.c:293` assigns BOP `50h` to
`MS_bop_0`. This is historical owner-selection evidence only. It is not a
runtime dependency and does not authorize importing SoftPC or reproducing its
general BOP dispatcher.

The pinned Bochs decoder has no OpenNT meaning for these bytes. Its C4 entry
is the LES/VEX decoder path (`refs/bochs/cpu/fetchdecode.cc:376`); with a second
byte whose high bits are set it recognizes a candidate VEX form
(`fetchdecode.cc:1369-1408`), explicitly marks that form invalid outside
protected mode, and its error executor raises `BX_UD_EXCEPTION`
(`fetchdecode.cc:2012-2020`). This is compatible with the historical use of
an otherwise invalid `LES` register encoding as a BOP marker. Bochs is only
the generic #UD producer here; it must not learn BOP number `50h`, service
`11h`, DOS loading, or continuation policy.

The immutable r6 copied `cpu/exception.cc` defines
`BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT` as `0` when not otherwise provided. Its
only root-local added compiler definition is
`/DBX_NTVDM_ENABLE_X87_COMPAT_DIAGNOSTIC=1` in the copied FPU Makefile.
Accordingly r6 did not compile an adapter #UD consumer for this transition.

## Classification

| Claim | Classification |
| --- | --- |
| `50:11` is a source-defined transition required to load NTDOS before `sysinit`. | Established. |
| The source-built NTIO image contains the corresponding bytes at its admitted placement. | Established from preserved image evidence. |
| Historical OpenNT selected `MS_bop_0` for BOP `50h`. | Established historical ownership evidence. |
| r6's generic #UD adapter intercept consumes this BOP/SVC. | Rejected: the copied r6 input leaves the intercept disabled. |
| This static transition is a direct instruction-by-instruction trace explanation of the first x87 record. | Unresolved: no such trace was collected. |
| A direct special-case `50:11` handler in Bochs or a partial DOS loader is now authorized. | Rejected. |

## Consequence

This is the first source-backed missing boundary, and it strongly explains why
the simple NTIO placement/entry companion cannot continue into normal NTDOS
startup. It is not evidence that Bochs needs DOS semantics. The next allowed
question is design-only: define the smallest **adapter-owned startup
transaction** that can identify one admitted transition, preserve its
post-BOP/service continuation and failure behavior, and use a generic,
default-off Bochs #UD mechanics seam. That design must first inventory the
existing adapter service plane and prove that it does not become a general
BOP/DEM dispatcher.
