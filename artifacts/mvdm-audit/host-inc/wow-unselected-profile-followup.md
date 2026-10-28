# Unselected WOW declarations and the active dispatcher

This bounded review adds to `wow-fax-dde-debug-declaration-review.md`; it does not classify the underlying DDE, dispatcher or USER capability as absent and does not mutate the shared ledger or guest.

## `inc/wowexp.h`: excluded candidate for the observed CCPU40 build only

The byte-identical original/current header contains one prototype, `extern VOID FAR PASCAL W32Dispatch(VOID)`. A source-wide literal include search found no current consumer, and the recorded WOW32/worker dependency census has no exact `wowexp.h` edge. In the selected chain the WOW32 implementation is declared separately by `wow32/wow32.h` and exported by `wow32.def`; `softpc.new/host/src/nt_bop.c` resolves `W32Dispatch` by `GetProcAddress` and holds a function pointer. This explains why the active dispatcher can exist without this separate declaration header. Earlier callback and Win16 tests concern the dispatcher/function, not evidence that this duplicate header was consumed.

Recommend excluding only the unused declaration carrier from the observed CCPU40 build, subject to coordinator review; this is **not** an exclusion of `W32Dispatch`, WOW support, or external/unbuilt/debug profiles. Source search and cached graph absence do not rule out macro-generated consumers; preserve the original file and leave broader profile obligations explicit.

## `inc/wowddeml.h`: pending

`wddeml32.c` includes this header and `wddetbl2.h` consumes its packed argument types, but original/current `wow32/sources` selects classic `wdde.c` and does not list `wddeml32.c`; original/current `wowtbl.c` does not include `wddetbl2.h`. The captured selected graph consequently does not select that historical thunk provider. `shard-2/ddeml-owner-boundary.md` and `ddeml-implementation-options.md` identify a separate real 16-bit DDEML source route, an alternative unselected native thunk route and the missing native private callback-instance export. Until the actual immutable guest DLL/entry route and the feasible provider are established, no overall exclusion or restoration of DDEML or this header is justified. Existing selected classic WM_DDE conversion is a distinct layer.

## `inc/wowint.h`: pending

It defines `FUN_SETTABLE` and a packed `WOWSETTABLE16` argument. Searching current host and guest C/headers/assembler found those two identifiers only in that header; no selected exact dependency was recorded. Those are limited negative observations, not evidence of behavior, absent macro-generated consumers, or a justified alternative implementation. Keep pending pending owner/profile reconciliation.
