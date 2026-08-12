# T95 S7 r36 INT10 Pseudo-ROM Source Correlation

## Question

What original owner precedes the first r36 TF-bearing return frame, and what
single fact remains missing before an execution diagnosis can be made?

## Inputs

- r36 immutable log:
  `artifacts/analysis/t95-s7-runtime-trace-029-20260811-001/bochs.log`.
- Source-built NTIO map:
  `artifacts/toolchain-runs/ntio-tools16-opennt-v1/base/mvdm/dos/v86/doskrnl/bios/ntio.map`.
- OpenNT guest sources:
  `doskrnl/bios/msbio1.asm`, `mschar.asm`, `msinit.asm`, and `spckbd.asm`.

## Procedure

Correlated the final pre-TF vector records with map symbols, then followed the
original source path that installs and invokes the observed INT10 handler.
No executable, source change, guest-memory inspection, or adapter operation
was performed.

## Observations

1. Immediately before the first TF frame, r36 records:

   ```text
   INT 29: 0032:379B -> 0070:0254
   INT 10: 0070:0260 -> 073B:089D
   INT  1: 0000:0043 -> 0070:018B
   ```

   The NTIO map names `0070:0254` `outchr`; `0070:018B` is `intret`.
2. `msbio1.asm` documents `outchr` as the INT29 display handler and calls
   `int 10h` for TTY output.
3. During initialization `sysinit1.asm` copies the original `SpcKbdBeg..End`
   resident guest bytes to DOS free memory, records that segment in
   `spc_mse_int10`, and invokes `InstSpcKbd`.  This is original OpenNT guest
   code—not adapter behavior.
4. `InstSpcKbd` first copies the existing IVT INT10 vector to the resident
   guest's `host_int10`, then installs `int10h_vector` into IVT entry 10h.
   The map locates the source offset of `int10h_vector` at `009D:402D`.
5. `spckbd.asm` initializes `use_host_int10` to `FULLSCREEN` (`1`).  In that
   branch `int10h_vector` reaches `jmp_native`, an original `EA` far jump via
   the saved `host_int10` pointer.  Other branches use historical EGA/mouse
   BOPs, but r36 contains no BOP observation between this INT10 delivery and
   the first TF frame.
6. The optimized `con_writ` path in `mschar.asm` does create an IRET-shaped
   frame, but it explicitly pushes FLAGS `0046`, `CS`, and `ret_adr` before
   its far return.  r36 instead observes saved FLAGS `0346` and return
   `0000:0043`; it is not that source-shaped acceleration frame.

## Interpretation and Confidence

High confidence: the immediately relevant composition is original OpenNT
guest pseudo-ROM code plus the pre-existing machine INT10 vector.  It is not
a DEM/BOP/filesystem service and must not be reproduced in the adapter.
Bochs remains the owner of the target's PC/firmware execution.

The unresolved fact is the actual far target loaded from the copied
`host_int10` field at `jmp_native`.  Existing r36 logs identify the installed
pseudo-ROM entry but not that original vector value.  Without it, the evidence
cannot distinguish a malformed preserved vector from a correct machine BIOS
target whose later return frame is altered elsewhere.

## Follow-up

If a further runtime observation is admitted, it may use only a default-off,
real-mode, selector-independent Bochs log after the original far-jump target
has been decoded.  It records pre-jump CS:IP and decoded target CS:IP, does
not read guest memory beyond the original operand fetch, and does not inspect
or name OpenNT/BOP/DOS/adapter state.  It must not alter a CPU result, IRET,
INT1, IVT, ROM, device, guest byte, or adapter contract.
