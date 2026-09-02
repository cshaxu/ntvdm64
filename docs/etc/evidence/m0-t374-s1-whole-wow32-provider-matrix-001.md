# M0 T374 S1 — Whole WOW32 provider matrix and admission cohort

## Question

What is the smallest source-shaped original provider cohort that can satisfy
the selected `MS_bop_1 → WOW32 → W32Init/W32Dispatch` ingress, and which
historical host edges must its recovery address?

## Inputs

* Original selected `src/mvdm-host/wow32/sources`, lines 39--115: 77 selected
  C provider bodies.
* Original `src/mvdm-host/wow32/wow32.c:460-764` (`W32Init`) and
  `:953+` (`W32Dispatch`), plus `wowtbl.c:40+` (`aw32WOW`).
* Original task path `src/mvdm-host/wow32/wkman.c:534+`
  (`WK32WOWInitTask`) and `:814+` (`W32Thread`).
* T276 final provider ledger and T277 divergence ledger.  Their file-level
  classification covers all 77 selected source bodies: 61
  `overlay-required` and 16 `adapter-backed`.
* T373's selected dynamic-ingress and formal-selection evidence.

## Provider matrix

The original manifest selects a single DLL product, not independently
selectable BOP services.  Its 77 bodies are represented by the following
source-owned matrix:

* **Dispatch/bootstrap:** `wow32.c`, `wowtbl.c`, `wcall16.c`, `wcall32.c`,
  `wcntl32.c`, `walias.c`, `wstruc.c`, `wparam.c`, `walloc16.c`,
  `mapembed.c`, `wthman.c`, `wthtbl.c` and their original tables.  These own
  `aw32WOW`, frame conversion and callback/task dispatch shape.
* **Kernel/task/DOS session:** `wkman.c`, `wkernel.c`, `wkbman.c`, `wkmem.c`,
  `wkfileio.c`, `wkgthunk.c`, `wdos.c`, `wktbl.c`, `wkbtbl.c` and `wreldc.c`.
  These own original TD lifetime, task/thread startup, guest stack and
  file/DOS interaction.
* **USER/message/window:** `wuser*.c`, `wu*.c`, `wmsg*.c`, `wmdisp32.c`,
  `wcuricon.c`, `wcurcash.c`, `wsubcls.c`, `waccel.c`, `wcommdlg.c`,
  `wowhooks.c`, `wutbl.c` and `wutbl2.h`/generated companion tables.  They
  require the original USER handler registration and message/window semantics.
* **GDI/resource/clipboard:** `wgdi*.c`, `wgfont.c`, `wgmeta.c`, `wgpal.c`,
  `wgprnset.c`, `wgtext.c`, `wdib.c`, `wres16.c`, `wres32.c`, `wuclip.c`,
  `wgtbl.c` and related tables.  These require GDI object identity plus the
  initialization performed by `InitCBFormats` and `InitVisRgn`.
* **Shell/OLE/printing, multimedia and network:** `wshell.c`, `wole2.c`,
  `wcommdlg.c`, `wspool.c`, `wow32fax.c`, `wsman.c`, `wmmedia*.c`,
  `wmm*.c`, `wsdata.c`, `wsext.c` and `wsraw.c`.  The T276 ledger identifies
  these 16 bodies as adapter-backed through public/same-shaped capability
  bindings; they still remain members of the original DLL product.

The exact per-file identity and disposition is retained in
`m0-t276-final-integration-disposition-ledger.tsv` rows whose `source_path`
starts with `wow32/` and `file_kind` is `source`; S1 does not duplicate or
silently collapse those 77 file identities.

## Required product boundary

`W32Init` executes the following unconditionally before `W32Dispatch` can be
treated as initialized:

1. fills a `PFNWOWHANDLERSIN` table with original allocator, resource,
   window-procedure, clipboard and dialog callbacks, then calls
   `UserRegisterWowHandlers`;
2. calls `RegisterWowBaseHandlers(W32DDEFreeGlobalMem32)`;
3. enters the original shared WOW product state through `LOCKSHAREWOW`, then
   cleans and registers the process shared list;
4. allocates and installs the initial `CURRENTPTD()` task record, creates the
   original task-creation event and initializes original registry/configuration
   state;
5. initializes known-DLL/setup-name/parameter-map/task-hung support, and calls
   `InitCBFormats` and `InitVisRgn`.

`WK32WOWInitTask` later depends on that initialized TD/product state while it
sets `RET_TASKSTARTED`, creates the original `W32Thread` and coordinates the
SoftPC timer.  The generated `aw32WOW` table then routes service calls through
the same product state.  Therefore a command-only, callback-only, shell-only
or public-API-only subset cannot satisfy the original ingress contract.

## Four-rung disposition

1. **Original source reuse:** selected.  The full 77-body original WOW32
   manifest is the minimum code cohort, preserving the original DLL, init,
   task, table and failure ordering.
2. **Smallest same-shaped binding:** required at four named edges: private USER
   handler registration, Base handler registration, shared WOW process/task
   state and TEB-backed current-task identity.  Public registry/event/process
   and selected shell/multimedia/network calls remain conventional Win32
   capability bindings.
3. **External-code intrusion:** not selected in S1.  Any mirror divergence
   must first retain the original provider body and use a named overlay only
   at a proven private transport or width-safe identity boundary.
4. **New behavior:** prohibited.  No static dispatch route, synthetic task,
   replacement NE loader or placeholder DLL is admissible.

## Result

The selected S2 cohort is the **whole original 77-body WOW32 DLL product plus
its four same-shaped host-boundary families**, not a per-BOP or per-export
slice.  S2 must first define these boundary facades and determine whether the
original init sequence can be formally composed.  It must not enable an
individual provider before that product-level question is closed.

## Confidence and follow-up

Confidence is high: the original manifest, `W32Init`, `W32Dispatch`,
`aw32WOW`, `WK32WOWInitTask` and the existing full file ledger agree on the
single-product boundary.  S2 owns only the selected original initialization
and dispatch product cohort.  A provider source change, media addition or
runtime observation remains outside S1.
