# T95 S6 Post-`50:11` Continuation Semantics Audit 001

## Scope

This is a read-only correlation of the retained r10 observation with the
pinned OpenNT and historical SoftPC sources and the source-built NTIO image.
It changes neither Bochs nor the adapter.  It does not make a runtime claim.

## Established control flow

The r10 log records the sole accepted startup transaction at `0070:0475` and
the checked `resume=0479`.  The source-built image has at file offset `0475`:

```text
C4 C4 50 11 EA F9 02 0D 01
```

`msinit.asm:247-252` identifies this as `SVC SVC_DEMLOADDOS`, followed by
`jmp sysinit`.  `DOSSVC.INC:33` defines the service as `11h`; the current
adapter bridge has independently proven the one corresponding NTDOS write to
the live `DI:0000` address.  The r10 `address=8b00` is consistent with the
relocated image instruction immediately before the BOP, `mov di,08b0h`.

The word order in the following far jump is little-endian: `EA F9 02 0D 01`
targets `010d:02f9`, not `0d01:02f9`.  The source-built NTIO was made by the
original rule `reloc ntio.exe ntio.sys 70` (`bios/makefile:74-80`), and its
entry is placed at physical `0x700` with `CS=0070`.  Therefore the target is
physical `0x10d0 + 0x2f9 = 0x13c9`, file offset `0x13c9 - 0x700 = 0x0cc9`.
The retained image has a near jump at that offset to `goinit`, matching
`sysinit1.asm:637` and `:809`.

This rejects the claim that the first post-transaction failure is caused by
wrong `50:11` resume arithmetic, wrong NTDOS load segment, or an omitted
NTIO relocation.  It also establishes that the x87 record is later than a
valid source-defined continuation.

## First required operation after that continuation

`sysinit1.asm:821-831` begins `goinit` by deliberately replacing `INT 12h`
with `BOP 12h`:

```asm
; int 12h ; Get Memory in 1k
BOP 12h
mov cl,6
shl ax,cl
...
mov memory_size,cx
```

Its comment says that the BOP is used to ask the 32-bit side for memory size
instead of calling an unknown ROM BIOS.  The r10 relocated image contains the
same `C4 C4 12` at physical `0x15e6`, reached by the documented immediate
continuation above.

Historical SoftPC identifies selector `12h` as `memory_size`
(`softpc.new/base/bios/bios.c:154-163`).  Its implementation reads the BIOS
memory word and returns it in `AX` (`base/bios/mem_size.c:44-53`); reset had
previously seeded that word from the configured memory size
(`base/bios/reset.c:854-855`).  Thus this operation is a narrow conventional
memory query, not DOS filesystem/DEM search behavior, x87 handling, IRQ13,
or VGA mapping.

## Causal classification

| Candidate explanation | Classification | Basis |
| --- | --- | --- |
| Incorrect `50:11` fault/resume state | Rejected for this first transition. | r10 records exact `+4` resume; image and relocation arithmetic reach the source `sysinit` entry. |
| Missing/incorrect Bochs x87, CR0.NE, IRQ13, IVT `75h`, or PIC | Not established; do not repair. | The r10 x87 snapshot occurs later with `IF=0`; no IRQ13 acknowledgement occurs. |
| Missing BOP `12h` historical memory-size transition | Established as the first source-required capability after `50:11`. | OpenNT source, relocated image, and historical SoftPC selector/owner agree. |
| Adapter should implement IRQ13 or DOS semantics | Rejected. | Neither follows from the reached operation. |
| Adapter should become a general BOP dispatcher | Rejected. | This audit proves one next operation only, not a dispatcher closure. |
| `0xa1800` requires VGA mapping now | Not established. | It occurs only after the unclassified post-`BOP 12h` control loss. |

The current narrow startup bridge intentionally accepts only `50:11`; it has
no accepted transaction for `12h`.  With Bochs retaining generic `#UD`
behavior for an unmatched marker, the source-backed causal explanation was
that this first unserved BOP fell through to ordinary exception delivery.

## r11 position-only corroboration

The one r11 fresh-root observation preserved the unchanged source-built input
identities and the same sole `50:11` commit, then emitted:

```text
ntdos64 unmatched-ud cpu=0 cs=010d rip=00000516
```

This is the exact CS:IP form of the precomputed physical `0x15e6` BOP `12h`
instruction. It corroborates the source/image correlation without inspecting
the guest instruction bytes at runtime: `BX-TRACE-039` logged only the
existing `prev_rip` after the narrow `50:11` transaction declined the next
`#UD`.

The same retained log subsequently contains unmatched markers at `0000:0b75`
and `0000:0816`, then the pre-existing x87 compatibility diagnostic, the
original IRQ13 path, and finally the `0xa1800` prefetch veto. They are all
downstream of the now confirmed unserved `BOP 12h`; none is evidence to add a
PIC/IVT, FPU, VGA mapping, DOS filesystem, or adapter IRQ behavior. The
observer watchdog termination is retained as evidence only.

## Next bounded step

Before defining a `BOP 12h` result path, the default-off generic unmatched-#UD
position diagnostic was used once and confirmed physical `0x15e6`. The next
separate gate is a design-only owner/ABI decision for the historical
conventional-memory query: it must preserve Bochs as CPU/exception and
machine-mechanics owner, keep the result mapping fixed-width and fail-closed,
and forbid a general BOP dispatcher or adapter-owned DOS/IRQ semantics.
