# M0 T303 S1 — first-slice mechanical-contract audit

## Decision

The first Base VDM to DEM/COMMAND vertical slice reaches four mechanical
contract families and one reusable future-WOW frame form.  It does **not**
require the historical SoftPC CPU executor, an A20 policy, a generic SAS map,
or a protected-mode provider to be enabled.  Bochs remains the sole CPU
executor through `adapter-bochs`.

The row-level source, width and disposition record is
[`m0-t303-s1-first-slice-mechanical-ledger.tsv`](../operations/m0-t303-s1-first-slice-mechanical-ledger.tsv).

## Reached original evidence

- `mvdm-support/inc/softpc.h:190` declares `host_simulate`; its
  `Sim32GetVDMPointer` and `Sim32FlushVDMPointer`/`Sim32FreeVDMPointer` forms
  are at lines 223, 471 and 474; `GetVDMAddr` has its numeric/process-alias
  variants at lines 522 and 531.
- Original COMMAND sources reach scalar register imports and `GetVDMAddr`.
  `dos/command/cmdmisc.c` includes a local lifetime divergence because a
  direct returned pointer cannot be retained across a modern x86/x64 boundary.
- Original DEM reaches the same register/pointer forms.  In particular,
  `dos/dem/demhndl.c` contains the historical Sim32 flush/free caller and
  `dos/dem/demfile.c` uses the SFT/JFT Sim32 paths.  `dos/dem/demdasd.c`
  reaches `host_simulate` at lines 513 and 532.

## Existing recovered bindings

- `adapter-mvdm-host-out/softpc/mvdm_command_registers.c` preserves reached
  fixed-width `get*`/`set*` spellings and delegates to selector-blind copied
  scalar operations in `adapter-bochs/machine_facade`.
- `mvdm_guest_location` owns no pointer identity: it captures 16:16 numeric
  locations and obtains a fresh bounded lease from the session's sole
  guest-memory mapping instance.  It is the only valid basis for replacing a
  source site that depends on a raw `GetVDMAddr` pointer.
- `mvdm_vdd_sft_shadow` gives the reached DEM SFT/JFT route explicit
  synchronous shadow/commit/discard behavior.  It is deliberately not a
  generic `Sim32GetVDMPointer` emulation.
- `mvdm_vdm_stack` already proves the source-shaped SS:SP copied frame path,
  but its `host_simulate` call currently reduces every CPU-loop return to
  `void`; it therefore cannot prove the historical stop/return contract.

## Explicit exclusions

`mvdm_a20`, `mvdm_sas`, `mvdm_xms_memory`, protected-span and protected-frame
helpers are retained evidence for XMS or DPMI/DPMI32.  This audit does not
enable them, claim them as DEM/COMMAND behavior, or introduce a second CPU
executor.  WOW stack/callback behavior remains P11-owned even though its
atomic 16:16 stack primitive is reusable mechanical evidence.

## Result

S1 is closed: every reached mechanical form has one owner and a width/lifetime
rule.  The only incomplete shared mechanical requirement is a selector-blind,
typed finite-resume result for `host_simulate`; its bounded recovery is the
next S and intentionally stops before MVDM ingress/event routing.
