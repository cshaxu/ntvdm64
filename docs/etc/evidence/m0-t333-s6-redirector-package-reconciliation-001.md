# M0 T333 S6 — Redirector package reconciliation

## Final package result

All 32 original `57:00..31` dispatch rows now have exactly one disposition:

- local named-pipe, mailslot, identity and async completion paths remain
  original provider code with the admitted Redirector adapter lifetime forms;
- local public information paths remain original code with bounded copied-span
  adapters;
- the original explicit `ERROR_NOT_SUPPORTED` bodies remain unchanged; and
- BaseSrv/RAP, NetBIOS, DLC and VDD rows have an exact, named queue successor.

The S1 table ledger plus the S5 network/assignment ledger are the complete
row-by-row record.

## DLL boundary decision

The original `vdmredir/sources` requests `DLLENTRY=VrDllInitialize`.  That
entry is an NT4 three-argument cdecl loader callback rather than a modern PE
`DllMain` ABI.  More importantly, its original `DLL_PROCESS_ATTACH` path
immediately invokes `VrInitialize`, which invokes `VrNetbios5cInitialize` and
`VrDlcInitialize`; detach invokes `TerminateDlcEmulation`.

Therefore an adapter loader wrapper would not be a neutral packaging change at
this point: loading it would claim success for still-unresolved NetBIOS/DLC/VDD
product facilities.  No wrapper, `/FORCE`, loader stub, or substitute
Redirector policy is admitted.  The original DEM/COMMAND deferred load branch
continues to observe `LoadVdmRedir` failure (`CF` with
`ERROR_INVALID_FUNCTION`) when no complete sidecar exists.

## Build and route checks

- Every original `vdmredir/sources` translation unit and the selected
  Redirector bindings compile into separate static libraries on both MSVC x86
  and x64 formal Ninja graphs.
- The S3 non-`/FORCE` direct-link probe remains the proof of the DLL's
  unresolved cross-package symbols; S4/S5 did not introduce a false resolver.
- A repository scan finds no live
  `mvdm-host-overlay/vdmredir` source/build/test input.  The only retained
  mentions are historical evidence records.
- The product executable remains independently linkable because Redirector is
  still an original deferred DLL dependency, not a statically forced host
  library.

## T333 closure

T333 closes the Redirector owner package boundary.  The next network work is
the queued NetAPI/RAP Redirector network-boundary package; it must start from
the retained original source and S5 ledger, not a trace-selected `57:xx`
service or a new network provider.
