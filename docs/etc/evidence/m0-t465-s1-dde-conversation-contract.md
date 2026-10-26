# T465 S1 — WOW32 DDE conversation contract

## Scope and conclusion

This is the source-first admission ledger for T465.  It changes no runtime,
guest input, build selection or release image.  It separates the selected
classic `WM_DDE_*` protocol from the distinct DDEML provider instead of using
the presence of a DDE-named source file as evidence that either is complete.

The finite result is:

- `wdde.c`, `wmsg16.c`, `wmdisp32.c`, `wumsg.c` and `wmtbl32.c` are selected
  production original WOW32 bodies.  Together they own classic DDE packet
  conversion, the persistent guest/native alias relation, native message
  packing, ACK/free release and the call-local reentry selectors.
- `wddeml32.c` is retained original source but is intentionally absent from
  the 77-body production WOW32 selection.  Its entry table calls the full
  public-looking DDEML API family, but those calls require the original
  private USER DDEML instance, transaction, atom and conversation manager.
  `hdata.c::FreeDDEData` is the deliberately narrow, separately selected
  original cleanup owner; it is not a DDEML provider.
- Therefore S2 may complete only the already selected classic protocol
  families.  S3 owns classic cancellation/peer/task/module loss and release.
  DDEML remains an explicit stopping boundary, not an implementation gap to
  bridge with an autonomous scheduler or a synthetic provider.

This preserves the policy that original code owns retained algorithms, modern
USER owns actual HWND/message resources, and the adapter owns only explicit
identity/callback transport.  No modern object is interpreted as a private
NT4 USER DDE object.

## Build and dispatch reachability

`src/mvdm/wow32/sources` selects `wdde.c`, `wmsg16.c` and `wmdisp32.c`.
`New-T404S5Wow32ProviderNinja.ps1` obtains its bodies only from that selected
manifest, while adding the already admitted `hdata.c` original USER cleanup
body.  It does not select `wddeml32.c`.

`wmtbl32.c` maps all nine classic message entries to the selected forward
converters:

| Protocol family | Native-to-Win16 owner | Win16-to-native owner | Persistent authority | Completion/loss rule |
| --- | --- | --- | --- | --- |
| INITIATE | `WM32DDEInitiate` | `ThunkWMMsg16` | `wdde.c::DDEInitiateList` | Per-invocation published node distinguishes initiate ACK from packed ACK; matching unthunk removes only that node. |
| TERMINATE, REQUEST, UNADVISE | `WM32DDERequest` | scalar `ThunkWMMsg16` cases | None | Convert endpoints only; there is no invented packet or free owner. |
| ACK | `WM32DDEAck` | `ThunkWMMsg16` | `wdde.c::phDDEFirst` | Execute ACK resolves reversed peer pair and applies original `FREE_MEM`/`FREE_H16`; packed message storage is distinct from payload authority. |
| ADVISE | `WM32DDEAdvise` | `ThunkWMMsg16` post branch | `phDDEFirst` | Packet alias is retained after successful delivery; a failed construction/post unwinds only the acquisition recorded by that call. |
| DATA and POKE | `WM32DDEData`, `WM32DDEPoke` | `ThunkWMMsg16` post branches | `phDDEFirst` | Format conversion may create DIB/metafile inner aliases; source object remains borrowed until original protocol release. |
| EXECUTE | `WM32DDEExecute` | `ThunkWMMsg16` post branch | `phDDEFirst` | Duplicate-handle compatibility may create a new guest alias; ACK owns its normal release, and rejected post owns its no-ACK rollback. |

`wdde.c::phDDEFirst` is the sole long-lived payload/alias authority.  It keys
each relation by both endpoints and both handle representations; it must not
be replaced with an adapter cache.  `DDECONVERSION` is intentionally
call-local and bounded: it records only pairs newly acquired by the current
conversion so failure can undo those pairs in reverse.  Existing aliases and
source objects are borrowed.  `DDEInitiateList` is the separate, sole
initiation authority.

## ABI, reply and reentry rules

Classic DDE carries three independently owned things:

1. `PackDDElParam`/`UnpackDDElParam`/`FreeDDElParam` own the native packed
   message representation.  Queue removal, hook consumption and the
   message-specific INITIATE distinction determine its release.
2. `HDDE` pairs own the cross-width data relation and the original
   `DDE_PACKET`, `DDE_METAFILE`, `DDE_EXECUTE_FREE_H16` and
   `DDE_EXECUTE_FREE_MEM` rules.  Payload lifetime can outlive conversion and
   callback return.
3. `fThunkDDEmsg` and `fWhoCalled` are thread-local conversion selectors,
   not protocol ownership.  Their saved/restored scopes in selected message
   and hook callers preserve nested same-thread conversion without creating a
   second queue or scheduler.

The native-to-Win16 window path calls the original forward converter, then
the original `CallBack16`, then its matching unthunk/free phase.  The
Win16-to-native post path keeps the `DDECONVERSION` record through native
`PostMessage`: success transfers it to the existing receiver/ACK owner;
rejection frees the packed parameter and rolls back only that record before
the original private-message fallback.  The synchronous AmiPro-compatible
`WU32SendMessage` route remains original and is not retrofitted with a post
failure rule.

`wow32.c` registers original `W32DDEFreeGlobalMem32` with the observed native
`RegisterWowBaseHandlers`/`GlobalFree` callback route.  Guest global-free
notifications enter original `W32WowDdeFreeHandle`/`W32DdeFreeHandle16`.
`hdata.c::FreeDDEData` intentionally excludes EXECUTE data, whose lifetime is
the ACK path.  These are protocol release owners, not generic `finally`
cleanup helpers.

## DDEML stopping boundary

`wddetbl2.h` and `wddeml32.c` show the original DDEML ABI: initialization and
per-instance callback binding, connection/list operations, transactions and
abandon, advise, data/string handles, callback enablement, naming service and
reconnect.  `W32DdemlCallBack` requires an instance-to-guest callback binding
and invokes `CallBack16`; the conversion helpers additionally register
`-1`-endpoint aliases.  Those are not classic packet entries and cannot be
folded into `phDDEFirst` without changing their owner and lifetime.

The required original provider is the excluded private USER DDEML manager.
It owns conversation/transaction IDs, atom/string identity, queued
transactions, callback scheduling, cancellation and instance/task teardown.
Public modern DDEML functions cannot be treated as its structure-compatible
replacement, and importing the USER manager would recursively require its
private USER/CSRSS state.  S1 therefore rejects all of the following:

- compiling `wddeml32.c` alone;
- adding a DLL-local conversation table or callback scheduler;
- deriving private USER state from modern HWNDs, pointers or handles; and
- treating `FreeDDEData` or the classic `EXECUTE` witness as DDEML evidence.

That is an architecture boundary, not a compatibility claim: DDEML calls are
not accepted by T465 unless a later owner-approved packet proves a finite
original-provider closure or approves a new observable semantic boundary.

## Evidence and S handoff

Earlier retained evidence already proves one real same-worker classic route:
`verify-win16-message-probe.ps1 -ClassicDde` exercised native
`WM_DDE_EXECUTE`, original guest alias reading, reverse `WM_DDE_ACK`, guest
early-free request and native alias retirement.  The exact-body alias fixture
also covers source-owned pairing/rollback.  Those witnesses do **not** cover
INITIATE atoms, DATA/POKE/ADVISE, external native peers, cross-worker routing,
or peer/task/module loss.

S2 owns the first four missing classic conversation/data families and must
prove actual original message/reply ordering through production callers.  It
may reuse the existing HDDE and `DDECONVERSION` authorities only.  S3 owns
rejected transfer, TERMINATE/UNADVISE, peer window destruction, normal module
or task retirement, and exactly-once release of any remaining classic aliases
and packed parameters.  Neither S may add a DDEML provider or change guest
media.  S4 will reconcile the real classic matrix and retain the DDEML
stopping boundary explicitly.

## S1 verification

The audit checked the selected original manifest, all nine classic dispatch
entries, both conversion directions, alias registration/rollback/free owners,
TLS selector declarations and DDEML dispatch/callback dependencies.  It also
checked the existing `hdata.c` divergence declaration and the retained real
classic `EXECUTE`/ACK witness.  No source build is claimed: S1 admits no
runtime change.
