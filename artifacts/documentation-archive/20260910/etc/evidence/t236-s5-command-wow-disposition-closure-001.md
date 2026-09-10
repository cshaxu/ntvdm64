# M0 T236 S5 P1 — COMMAND WOW Source-Disposition Closure

## Question

Can the imported OpenNT COMMAND `54:01` path compose its historical WOW
helpers in the one-session CLI, without inventing a WOW bootstrap or a private
NT4 broker substitute?

## Source Map

The retained source owner is
`src/opennt/base/mvdm/dos/command/cmdmisc.c`:

| Original item | Historical role | Current disposition |
| --- | --- | --- |
| `cmdGetNextCmd` | Retrieves the ordinary VDM command and, when `VDMForWOW` is set, first requests the `krnl386` startup command line. | Direct imported normal COMMAND path in `src/bx-vdm/bop/opennt/command/cmdmisc.c`. |
| `GetWowKernelCmdLine` | Publishes the separate WOW VDM's `krnl386` startup command/environment into guest COMMAND structures. | Source-shaped unavailable outcome: current branch calls a documented shim which performs the original non-returning terminal convention as a typed controlled stop. |
| `GetWOWShortCutInfo` | Copies `VDMInfo.Reserved` metadata for the first WOW task. | Not compiled or exposed by the admitted COMMAND slice: there is no reached current caller and no admitted WOW startup producer/consumer for `VDMInfo.Reserved`. |
| `GetNextVDMCommand` | NT4 client request to BaseSrv/CSR, including WOW command/broker behavior. | The already-closed local session seam serves only normal one-session input/environment and re-entry count. It is not claimed as BaseSrv or a WOW broker. |

The original `GetWowKernelCmdLine` depends on the separate WOW product
composition: a WOW root, kernel command publication, WOWEXEC/WOW32 consumer
and NE loading. Re-enabling the source body without those owners would write a
startup image that this product cannot execute. `GetWOWShortCutInfo` depends
on the same BaseSrv-provided startup-reserved input. Neither has a public
Win32-only replacement that is semantically equivalent on its own.

## Smallest Compatibility Seam

`src/bx-vdm/bop/shim/command_misc_shim.c` retains the `VDMForWOW` branch's
terminal shape by calling `TerminateVDM()`. It produces a typed `STOP`, never
continues after the BOP, writes no fabricated kernel command line, exposes no
host pointer or handle, and does not introduce WOW policy into Bochs.

This is a source-derived unavailable disposition, not a successful WOW
implementation. The direct imported ordinary `cmdGetNextCmd` route remains
unchanged.

## Procedure And Observation

The focused direct-import fixture initializes an ordinary COMMAND source,
then invokes `54:01` with `vdm_for_wow = 1`. The fixture requires:

1. successful dispatch of the imported `cmdGetNextCmd` branch;
2. a typed `STOP` result with no resume RIP; and
3. no reset of already-delivered ordinary-session command state.

The formal MSVC x64 `/MT` Ninja target
`t231-s7-command-get-next-direct-fixture.exe` passed in a fresh S5 build
root and its immediate Ninja dry-run reported no work. Its marker is:

```text
T231 S7 direct OpenNT cmdGetNextCmd local handoff/retry plus explicit VDMForWOW unavailable stop verified
```

## Result And Transfer

S5 is closed. The normal `54:01` local handoff remains direct OpenNT source;
the reached `VDMForWOW` branch is explicitly terminal rather than falsely
successful. `GetWOWShortCutInfo` is intentionally not exposed until an owner
can provide both the original-style reserved startup data and its WOW consumer.

The following work transfers to the queued **OpenNT WOW16 owner-package
completion**: WOW root selection, `krnl386` command publication, shortcut
metadata producer/consumer, WOWEXEC/WOW32 brokerage, NE/DPMI loading and a
declared non-GUI WOW execution profile. Full BaseSrv/CSR command brokerage and
cross-owner COMMAND execution continuity remain separately assigned to the
queued **OpenNT COMMAND cmdExec32 full-capability closure** package.

