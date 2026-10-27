# S10 shard 2: DDEML provider and guest route

## Frozen scope and current observation

`wow32/wddeml32.c` is source-identical in the frozen manifest but not selected
by the x86 WOW32 product graph. `wow32/sources` does not select this file.
`wddetbl2.h` carries 16-bit ordinal entries to `WD32Dde*` operations, but a
repository-wide text search of the selected WOW32 tree found no include of
that header. This is source/build evidence, not a direct observation of an
actual guest module calling the original table.

The historical T465 review is
`docs/etc/evidence/m0-t465-s1-dde-conversation-contract.md`: it excludes the
NT4 private USER manager/CSRSS closure and correctly does not equate classic
`WM_DDE_*` acceptance to DDEML. Its blanket assertion that public modern
DDEML cannot replace the internal manager does not by itself prove that the
source-shaped thunk cannot use documented public DDEML handles with bounded
identity/lifecycle conversions. That narrower possibility requires evidence.

## Original source routes, kept distinct

1. The unselected thunk route maps DDEML ordinals via `wddetbl2.h` to
   `WD32DdeInitialize`, which calls native `DdeInitialize` and associates
   the returned instance with the 16-bit callback in `aCallBack` via
   `WOWDdemlBind`. Subsequent client transaction/data/string operations call
   public DDE APIs and use original alias helpers, `aAccessData`, and
   `WOWDdemlUnBind`. `W32DdemlCallBack` obtains a current instance from the
   private `DdeGetCallbackInstance`, converts callback parameters, allocates
   a temporary 16-bit CONVCONTEXT when needed, calls `CallBack16`, then frees
   that temporary. Each of the two original binding arrays has `MAX_CONVS`
   3200 slots; overflow and absent lookup have original weak error behavior.
2. The separately mirrored `wow16/ddeml/ddeml.def` exports `DDEINITIALIZE`
   at ordinal 2, and its own `wow16/ddeml/ddeml.c::DdeInitialize` calls its
   Win16 `Register` implementation. That source includes original guest
   instance, window/message, atom and queue logic; its Makefile builds a
   16-bit DDEML.DLL from many guest objects. It is not proved to be the
   particular unchanged guest DLL available at runtime. Its existence shows
   that rejecting the host thunk cannot by itself establish guest DDEML
   absence. Do not modify or rebuild guest to resolve this question.
3. Current selected `wdde.c` / `wmsg16.c` / `wmdisp32.c` / `wumsg.c` /
   `wmtbl32.c` implement the classic `WM_DDE_*` conversion and ownership
   chain, not the native DDEML instance API. T465 S4 proves its selected
   classic cases and explicitly excludes general DDEML coverage.

## Concrete host boundary observation

On this machine `SysWOW64/user32.dll`, version `10.0.26100.8117`, SHA-256
`56F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E`,
`dumpbin /exports` lists `DdeInitializeA` and `DdeUninitialize` but not
`DdeGetCallbackInstance`. The native private callback-instance lookup is
therefore unavailable through this x86 DLL's named exports. The same host
check is not proof about every supported Windows release, and a missing
private export does not establish that documented public DDEML APIs are
semantically inadequate.

## Finite candidates to investigate, not approvals or passing claims

- First identify actual immutable guest `DDEML.DLL` identity, source lineage
  and its imports/dispatch. If it owns DDEML in 16-bit code, trace each of its
  USER/message/atom and callback dependencies through current selected WOW
  paths and test both same- and cross-process instances. Avoid introducing a
  redundant second owner merely to compile an unreachable thunk.
- If a legitimate guest/source profile reaches the thunk table, evaluate
  retaining `WD32Dde*` against documented public DDEML APIs with a bounded
  callback-instance association. Public callback parameters do not directly
  provide the instance, and the old thunk assumes the missing private query.
  Any slot-specific callback or another association must be assessed for the
  original 3200-slot capacity, reentrant initialization, initialization-time
  callbacks, exact instance/thread identity, duplicate IDs, uninitialization,
  task death, alias transfer and failure rollback. This is only a design
  direction; feasibility, size, and semantic equivalence remain unproven.
- Identify the original guest-table source and selected task callback ABI,
  then use a source-shaped fixture and immutable real 16-bit DDEML client/peer
  for callback/init, transaction, access/unaccess and retirement. Compare
  against a canonical original-compatible baseline and preserve negative and
  nested behavior. Existing classic DDE probes are supplemental, not an
  acceptance substitute.

Until route and dependency evidence resolve those questions, keep the file
`pending` in the tri-state reconciliation and assign DDEML investigation
to the relevant later complete functional owner group. Do not resurrect
private USER/CSRSS or introduce a synthetic DDE manager solely for a link.
