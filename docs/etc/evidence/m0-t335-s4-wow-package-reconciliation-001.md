# M0 T335 S4 — WOW32 package reconciliation

## Selected product boundary

The declared T335 product is a non-GUI WOW32/Win16 host/guest vertical
profile.  `mvdm-host/wow32` retains the original host source ownership;
`mvdm-guest/win16` remains load-only media.  No guest object or binary becomes
a host-link input.

The selected profile has these proven elements:

- original `wcall16.c` `CallBack16` body, with its original CCPU40 stack and
  return ordering, is compiled and exercised by S3/P12;
- original BaseVDM command-record flow is bound through the existing
  same-shaped local BaseVDM boundary (S3/P10);
- the complete temporary-pointer family uses bounded session leases rather
  than raw aliases (S3/P1--P7);
- app forwards original `-f/-w/-a` activation and the existing guest media is
  consumed by original COMMAND/DOS logic (S3/P11).

The formal process product deliberately does not statically link a general
WOW provider or guest trampoline: BOP `51h` retains the original dynamic
provider boundary, and `WOW16_From_CallBack16` is guest media.  This is an
explicit `exact-unavailable/not-yet-reached` product disposition, not a
missing loader or an app-owned replacement.

## Fresh formal build evidence

Two new disposable graph roots were generated from the current worktree with
`tools/build/New-T310OriginalSoftpcNinja.ps1`, using CCPU40 and MSVC `/MT`:

```text
build/M0-T335/S4/formal-x86
build/M0-T335/S4/formal-x64
```

Each executed the complete 419-action `original-softpc-process.exe` graph
under its matching `VsDevCmd` architecture environment.  A subsequent
`ninja -n original-softpc-process.exe` reported `no work to do`, and each
root contains its final product EXE.  The prior direct Git-Bash invocation
failed before compilation because `cl.exe` was not on its environment PATH;
the rerun under `VsDevCmd` is the only build result used here.

The source tree retains pre-existing historical warnings.  They remain
visible and are not suppressed or treated as a new passing x64 semantics
claim.  Both selected product links succeed without `/FORCE`.

## Fixed-container observation

The fresh x86 product was staged with the unchanged DOS/ROM media by
`Stage-OriginalSoftpcRuntime.mjs`, then launched once by the non-debug,
console-owning observer with:

```text
-f -w -a build/output/wow16/kernel31/retail3/krnl386.exe --ordinary-child
```

The observer reports:

```text
container=console-owning-nondebug
result=exited
exit=0xc0000005
timeout-ms=8000
```

This repeats S3/P11's observed post-`54:05` guest/CCPU continuity boundary.
It neither selects a BOP repair nor indicates a missing DOS/Win16 loader.
No x64 execution claim is made: x64 is the paired compile/link verification
architecture for this current CCPU40 profile.

## Final dispositions

- GUI/USER/GDI, printing, OLE, multimedia and FAX remain outside the declared
  non-GUI profile.
- WOWEXEC/CSRSS/BaseSrv transport, kernel VDM and cross-process WOW broker
  remain explicit historical product-shell boundaries.
- The actual Win16 callback trampoline and BOP `51h` provider load remain
  source-owned but unreached after the recorded CCPU continuity fault.
- No new guest loader, generic callback scheduler, raw pointer mapping,
  Bochs route or BOP leaf workaround was introduced.

## T335 closure

T335 is closed as a source-backed, declared non-GUI WOW32/Win16 host/guest
profile.  It does not claim generic Win16 applications, GUI/WOWEXEC, or
successful arbitrary guest execution.  The next queue package owns the
separate CPU40/guest-continuity recovery needed before later WOW ingress can
be evaluated.
