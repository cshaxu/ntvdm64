# M0 T373 S2 P2 — WOW32 dynamic-ingress source boundary

## Question

Can the selected original WOW command bodies be reached in the current
CPU40/x86 product by directly binding a small static subset, or does the
earlier original SoftPC dynamic-provider boundary select a larger owner
contract that S2 cannot safely replace?

## Inputs

* `src/mvdm-host/softpc.new/host/src/nt_bop.c:MS_bop_1`.
* `src/mvdm-host/wow32/wow32.def`, `sources`, `wow32.c`, `wowtbl.c`,
  `wkman.c`, `wucomm.c` and their original declarations.
* S1 topology ledger and S2 P1 command-binding dependency closure.
* Current CPU40/x86 formal generator
  `tools/build/New-T310OriginalSoftpcNinja.ps1`.

## Procedure

1. Read the selected original `MS_bop_1` path before any WOW command thunk.
2. Compare each dynamically required export with its original definition and
   all initialization/dispatch prerequisites.
3. Compare that required source contract with the current formal graph and
   the S2 non-goals.
4. Reject any candidate that calls a WOW command body directly or adds a
   second static dispatcher, because either would bypass the selected SoftPC
   provider lifecycle.

## Observations

* `MS_bop_1` dynamically loads `WOW32`, resolves five names
  (`W32Init`, `W32Dispatch`, `GetCommHandle`, `GetCommShadowMSR`, and
  `W32HungAppNotifyThread`), calls `W32Init` once, then calls `W32Dispatch`.
  In the CPU40 branch it preserves/restores `GLOBAL_SimulateContext` and
  detects a stack switch after dispatch.
* A missing DLL, a missing export, or a false `W32Init` follows the original
  terminal direction: `MS_bop_1` frees the DLL when appropriate and invokes
  `TerminateVDM`. The current formal graph intentionally stages no WOW32 DLL.
* Original `W32Init` is not a command-only initializer. Before returning it
  calls private `UserRegisterWowHandlers`/`RegisterWowBaseHandlers`, acquires
  shared WOW task-list state through `LOCKSHAREWOW`, cleans/adds a shared
  process record, allocates current-task state, initializes clipboard and GDI
  visibility support, and initializes hung-app support. These are the
  historical USER/GDI/shared-product shells excluded from S2.
* Original `W32Dispatch` is likewise not a command-only dispatch point. It
  acquires the current VDM frame, requires `CURRENTPTD`, dispatches by the
  generated global `aw32WOW` thunk table, writes AX:DX, restores VDMSTACK and
  applies original WOW exception handling. The selected command entry
  `WK32WowGetNextVdmCommand` and `WK32DosWowInit` are two KERNEL rows inside
  that table, not an independent public dispatcher.
* `GetCommHandle`, `GetCommShadowMSR` and `W32HungAppNotifyThread` pull in
  the original communication and hung-task product families. A placeholder
  export that merely satisfies `GetProcAddress` would turn later use into a
  false success.

## Disposition

The earliest selected unavailable boundary is **the original WOW32
`W32Init` product-shell contract**, before either selected command thunk can
be correctly reached. S2 therefore does not add a static command route, a
replacement `W32Init`, placeholder DLL exports, or a second WOW dispatcher.
The existing original `MS_bop_1` dynamic-load failure remains the correct
current CPU40/x86 behavior.

This is a source-shaped unavailable result, not a claim that WOW command,
task or NE lifecycle is complete. The selected `wkman.c` command bodies and
their mapping-safe dependencies remain recorded for later composition only
after an admitted original WOW32 product-shell cohort has a finite owner
boundary. That later cohort must retain the dynamic DLL export contract and
must not substitute app/adapter dispatch.

## Verification

* Source-order audit confirms every `MS_bop_1` export lookup precedes the
  initializer/dispatcher calls and their CPU40 context handling.
* Definition audit confirms the first required `W32Init` private USER/shared
  product edges occur before command-thunk dispatch.
* The current formal generator selects only
  `adapter-mvdm-host-out/win32/source/wow_private_unavailable.c` and no
  WOW32 provider DLL; this matches the source-shaped unavailable profile.
* No production source, build selection, guest media or runtime behavior was
  changed by this boundary audit.

## Follow-up

S3 retains the already-selected non-fast callback/CPU40 work and must not
assume that a WOW command record can yet enter `W32Dispatch`. Any future
admission of WOW command or NE launch begins with a whole-package
`W32Init`/dispatcher product-shell ownership audit, including its public
modern bindings and every remaining explicit unavailable edge.
