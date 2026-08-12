# T150 S1 INT 0Dh / BOP 02 return-frame audit 001

## Question

Can the original unexpected-interrupt path itself create the repeated
`0BC1:FFFF` prefetch fault, or is it only returning to an already-invalid
fault frame?

## Source and trace correlation

| Evidence | Established fact |
| --- | --- |
| `src/opennt/base/mvdm/dos/v86/doskrnl/bios/spckbd.asm:57,455-457,939-940` | `UNEXP_BOP` is `02h`; `unexp_int` emits the three-byte BOP and then jumps to `iret_com`, whose only return instruction is `FIRET`. |
| `spckbd.asm:2172-2201` | The original installer places `unexp_int` in the INT 0Dh IVT entry (along with other intentionally unexpected vectors). |
| `src/opennt/base/mvdm/softpc.new/base/bios/bios.c:120-124` | SoftPC's machine BOP table assigns BOP `02h` to `unexpected_int`, not to an `MS_bop_*` host-service dispatcher. |
| `src/opennt/base/mvdm/softpc.new/base/system/unexp_nt.c:50-91` | The original handler reads/masks/EOIs PIC state when applicable and stores the interrupt cause at BIOS data offset `0x6b`; it does not set CS, IP, SP, FLAGS or an IRET target. |
| T118/T119 evidence | The admitted machine-composition result resumes the BOP at copied fault RIP plus three, preserving CPU registers/flags; then the following *guest* jump reaches the unchanged `FIRET`. |
| T149 first and final cycles | Native Bochs reports the prefetch limit failure, then vector `0Dh` from `0BC1:FFFF`, then selector `02h` / handled outcome, and repeats that same sequence 309,782 times. |

## Result

The IRET return is not an independently generated bad transfer. The CPU first
detects the out-of-limit fetch and delivers INT 0Dh, creating the interrupt
frame. The original guest handler reaches BOP `02h`; its original machine
side-effect function does not alter the continuation; the handler then
executes `FIRET` and restores that already-created fault frame. Returning to
the saved `0BC1:FFFF` position explains the next identical prefetch fault.

This classifies the loop as an **original fault-frame replay**, not an adapter
BOP/provider result, a host-service dispatch error, a missing PIC semantic, or
a BOP `02h` return-ABI error. PIC/BDA behavior remains Bochs machine-owner
behavior through the existing contained machine component; adapter ownership
does not expand.

## Diagnostic limitation

`cpu.cc` assigns `prev_rip` at CPU-loop commit points; `BX-TRACE-056` records
that existing field plus the limit-check fields. It neither decodes guest code
nor provides a branch history. In particular, `prev-rip=FFFFh` cannot by
itself prove which instruction last executed or which transfer first selected
the `0BC1` application segment. The error line's `EIP=10000h` is the native
out-of-limit condition; the diagnostic's `eip=00000000` must not be treated as
a contradictory guest-state transition without a source-proven CPU-width
interpretation.

## Disposition

No source or runtime action is admitted. The next smallest useful inquiry is
the command-image/load provenance at application-relative offset `FEFFh`
(`FFFFh - 0100h`), including whether the retained source-built image can be
shown to occupy that address. That is a static image/provenance audit first;
it must not infer a source label merely from a matching byte or add another
runtime diagnostic.
