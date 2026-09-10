# M0 T335 S2 — WOW `CallBack16` frame-contract closure

## Result

The original non-fast `CallBack16` transaction has a finite, source-shaped
binding plan. It is **not enabled** by this record. The original body in
`mvdm-host/wow32/wcall16.c` remains the algorithm and ordering authority.

The only non-original mechanism required by the selected transaction is a
small `adapter-mvdm-host-out/wow` scoped-frame bridge. It will use the
already-existing session guest-memory mapping instance and its bounded lease
API; it must not introduce a second mapping manager, raw host pointer,
callback scheduler or BOP service.

## Original flow established

1. Read the current PTD and its numeric `vpStack`.
2. Temporarily read the caller `VDMFRAME` and validate the original task
   relation.
3. Allocate the word-aligned `CBVDMFRAME` below the original stack according
   to `TDF_INITCALLBACKSTACK`; write and flush it.
4. Release both temporary VDM mappings before the recursive CPU invocation.
5. Set the numeric SS:SP and invoke the original CPU40 `host_simulate`
   function vector, then restore IP.
6. After its original return, reacquire the callback frame, copy AX:DX,
   restore `vpStack`, and release it.

`nt_cprgs.c` proves that the selected CPU40 CCPU branch assigns
`host_simulate_func = c_cpu_simulate`. Thus the non-fast callback calls the
same original recursive CCPU mechanism as the source; the top-level
`mvdm_softpc_execution_run_until_return` composition helper is deliberately
not substituted for it.

## Required difference and why it is bounded

The original `GETFRAMEPTR`/`FLUSHVDMPTR` macros expose `FlatAddress` and an
NT4-process pointer. That cannot be retained across a modern session or an
x64 build. The original source itself releases its frame pointers before
`host_simulate` and reacquires the result afterward, so it naturally maps to
two short guest-memory leases:

- a read lease for `VDMFRAME`;
- a write-and-commit lease for `CBVDMFRAME` before recursive execution;
- a new read lease for `CBVDMFRAME` after return.

The bridge must preserve these boundaries and original failure result. It may
not retain a lease over `host_simulate`, expose the bounce buffer outside the
call, or convert a guest address into a persistent host pointer.

## Explicit exclusions

- `FASTBOPPING`, `CurrentMonitorTeb`, `FastWOWCallbackCall`;
- BaseSrv/CSR `GetNextVDMCommand` and WOWEXEC notification;
- GUI/USER/GDI, generic Win16 API, arbitrary guest load and WOW process
  broker behavior;
- any Bochs route, machine recreation or generic adapter abstraction.

Those items retain their named package boundaries. The row-by-row source,
ABI, lifetime and successor decision is in
[`m0-t335-s2-wow-callback-frame-contract-ledger.tsv`](../operations/m0-t335-s2-wow-callback-frame-contract-ledger.tsv).

## S2 closure decision

Every transaction element now has a direct, binding-only,
overlay-required, exact-unavailable or named-successor-owner disposition.
The next legal step is T335/S3's declared non-GUI vertical profile: implement
the small scoped-frame bridge, bind it to the original CPU40 nested-return
contract, and verify it locally before selecting any WOW provider body.
