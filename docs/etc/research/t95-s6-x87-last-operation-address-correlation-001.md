# T95 S6 x87 Last-Operation Address Correlation 001

## Address facts

The narrow startup-session source (`src/bx-ntvdm-adapter/bx_ntvdm_startup_session.c:59-64`) sets the real-mode entry to `CS=0x70`, writes the full NTIO image at physical `0x700`, and preserves only the four bytes at `0x714`. The source-built image is 33,792 bytes (`0x8400`), so its only admitted payload interval is:

```text
[0x700, 0x700 + 0x8400) = [0x700, 0x8b00)
```

The image map places its initial `init` at offset `0x370`, consistent with the entry `jmp` from offset zero. This confirms that its normal initial code lies inside the same payload interval after the `0x700` placement.

## First captured x87 record

The first r6 diagnostic record is:

```text
CS:RIP = 0000:0590
FCS:FIP = 0000:00fa
FDS:FDP = 0000:0d41
```

Real-mode segment:offset physical arithmetic gives respectively `0x590`, `0x0fa`, and `0x0d41`. Each is below `0x700`, hence outside the admitted NTIO payload interval.

This is not an inference from a disassembler. `FPU_update_last_instruction` in `src/bochs/fpu/fpu.cc:48-56` records the current CS and committed prior RIP as the FPU last-instruction location, and records the selected data-segment address for a memory operand. The source therefore establishes that the first recorded last x87 operation and its operand are both outside the copied NTIO image. The current snapshot also has `CS=0`, not the admitted entry segment `0x70`.

## Classification

| Claim | Classification |
| --- | --- |
| The adapter copied NTIO at `0x700` and entered `0x70:0`. | Established by the fixed startup-plan source and accepted plan log. |
| The first x87 compatibility check is executing within the copied NTIO payload interval. | Rejected: both current `0000:0590` and last-x87 `0000:00fa` physical locations are outside it. |
| The initial x87 summary comes from uninitialized Bochs reset state. | Rejected by the non-reset x87 status/provenance record. |
| A particular IVT entry, IRQ13 handler, BIOS owner, BOP/DEM service or host service caused the transfer outside the payload. | Unresolved. The records prove the address-domain mismatch, not its transition cause. |

## Consequence

The next question is no longer an FPU initialization question. It is a first-prefix **control-transfer provenance** question: identify, from the already admitted initial vector/state inputs and source path, how execution can reach below `0x700` before the first compatibility check. No vector or state injection, firmware replacement, interrupt shim, host service, or additional runtime attempt is authorized by this record.
