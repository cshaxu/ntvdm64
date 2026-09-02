# M0 T386 S1 — original COMMAND HMA/arena startup-chain audit

## Question

Does the selected CPU40/SoftPC and immutable original DOS image provide the
source-owned HMA/A20/arena path required before permanent `COMMAND.COM` can
perform its initial environment sizing and command-loop handoff; and is the
current `MVDM-HOST-DIV-200` `BX=0` fallback an admissible result?

## Inputs

- Selected immutable `O:\\ntvdm64\\COMMAND.COM`, SHA-256
  `908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`.
- Matching original OpenNT `base/mvdm/dos/v86/cmd/command/COMMAND.COM`, with
  the same SHA-256.
- Original guest sources: `doskrnl/bios/msint13.asm`, `msbio1.asm`,
  `sysinit1.asm`, and COMMAND `rdata.asm`/`init.asm`.
- Current selected CPU40 sources: `softpc.new/host/src/nt_mem.c`,
  `sim32.c`, `base/ccpu386/ccpusas4.c`, and current mirrored
  `dos/command/cmdenv.c`.

## Procedure

1. Compared the selected `COMMAND.COM` hash with the OpenNT original and
   checked the guest mirror for an uncommitted diff.
2. Traced `INT 2Fh` functions `4A01`/`4A02`, `HMAPtr`,
   `MoveDOSIntoHMA`, `FTryToMovDOSHi`, `SaveFreeHMAPtr`, and A20 ownership.
3. Traced the COMMAND initial-environment retry and its order relative to
   `EndInit`, resident sizing, MCB allocation and transient relocation.
4. Compared current `cmdGetInitEnvironment` with the original body.

## Observations

- The guest image is unchanged: both selected artifacts hash to the value
  above; the guest-mirror diff is empty. No guest byte was written in S1.
- `msint13.asm` routes `INT 2Fh/4A01` and `4A02` through `HMAPtr`.
  When `FreeHMAPtr` is `FFFFh` and `SysinitPresent` is set, `HMAPtr` calls
  `MoveDOSIntoHMA`; `sysinit1.asm` binds that pointer to
  `FTryToMovDOSHi`. On success `SaveFreeHMAPtr` records the normalized
  `FFFF:offset` free area and sets `inHMA`.
- `nt_mem.c::InitIntelMemory` commits the first 1 MiB plus the `0xFFF0` A20
  wrap interval. Under `CPU_40_STYLE`, `sim32.c::host_sas_init` selects that
  initializer, and `ccpusas4.c` retains the original A20 contract:
  `c_sas_enable_20_bit_wrapping` masks at `0xFFFFF`; disable uses
  `0xFFFFFFFF`. The machine has a source-defined HMA backing route; S1 found
  no evidence that it is replaced by a synthetic command allocator.
- COMMAND's `Setup_res_end` chooses resident size based on DOS-in-HMA state.
  Its environment sequence in `rdata.asm` first allocates an environment,
  calls `SVC_GETINITENVIRONMENT`, and, if returned `BX > EnvSiz`, explicitly
  deallocates that block, allocates `BX` paragraphs, and calls the service
  again. Only afterward does it enter the `EndInit`/transient relocation
  sequence.
- The original `cmdenv.c::cmdGetInitEnvironment` reports the required
  paragraph count when the supplied `BX` capacity is too small. The current
  mirror instead has `MVDM-HOST-DIV-200`, returning `BX=0` for exactly this
  normal retry path.
- **Correction after the direct runtime observation:** `EndInit` is not
  safely outside the first COMMAND environment allocation. The original
  source deliberately overlays `EndInit` on `Pipe1` (`rdata.asm`), and the
  map places that area in the still-live transient image. With the current
  modern host environment, the original required-size response is `BX=0270`;
  freeing the initial `0010`-paragraph block and reallocating at `ES=049F`
  yields `[0x49f0, 0x70f0)`, which covers the BOP return continuation. Thus
  the earlier inference that the retry protocol was independently safe was
  incorrect. The original retry protocol exists, but its safety depends on
  an arena/layout precondition that this product has not yet restored.

## Interpretation and confidence

High confidence: `MVDM-HOST-DIV-200` is not source-shaped. The original guest
explicitly owns the required-size retry protocol, so suppressing it cannot be
the final product behavior. The direct observation additionally proves that
the retry alone is insufficient under the current arena/layout: it overwrites
the still-live transient continuation before the later relocation sequence.
This audit does **not** claim that HMA, A20, or the MCB arena is already
runtime-proven. The next recovery must establish the original arena/layout
precondition without changing guest bytes or silently shrinking the host
environment.

## Four-rung disposition

1. **Original source reuse — selected.** Restore the original insufficient-
   capacity result in the imported `cmdGetInitEnvironment` control flow.
2. **Same-shaped facade — retained only for guest-memory access.** The
   existing mapping-manager lease replaces historical `GetVDMAddr` only while
   preserving ES:0, span and write failure behavior.
3. **External intrusion — not yet decided.** The required-size return itself
   needs no machine change, but the direct retry has now proven an unresolved
   original DOS-arena/layout precondition. That owner must be audited before
   selecting a minimal recovery.
4. **New behavior — rejected.** No synthetic environment, command loader, or
   guest modification is admissible.

## Follow-up

S2 restored the original required-size return and its direct runtime
observation disproved the assumed arena safety. The next admitted work must
audit and recover the original DOS arena/layout precondition, then repeat the
formal link and immutable COMMAND built-in execution proof.
