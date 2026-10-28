# DDEML route: witnessed initialization and finite S14 boundary

Original `wddetbl2.h` maps DDEML ordinals to the `WD32Dde*` host thunk family,
but the checked original WOW32 source manifest and master table do not select
that source or table; the current x86 graph also does not name the old source.
Selected classic `WM_DDE` conversion is a separate mechanism. Conversely,
the original guest DDEML source has its own 16-bit instance/message logic;
neither its presence nor the old thunk's absence proves which path an arbitrary
installed application actually takes.

The historical [initialization receipt](../shard-2/ddeml-init-evidence.json)
records isolated real Win16 tests of two unmodified, app-local DDEML images:
the Win3.1 and OpenNT debug copies. Both rejected a nonzero reserved parameter,
then completed initialization, same-instance reinitialization, uninitialize
and a second uninitialize with the recorded results. The callback was not
instrumented, and no DDE transaction was attempted. The test does not establish
guest source lineage, default search path, current package identity or
cross-task behavior. Its staged image files were removed; their hash values in
the receipt are historical observations, not newly rehashed bytes.

The [observer](ddeml-route-observe.py) independently checks the existing test
source, runner, both observer logs and both result files against receipt hashes,
and binds the checked current source and graph hashes. All six historical
identity checks passed in the [receipt](ddeml-route-evidence.json). Replay the
observer from the repository root. It does not launch the product or guest.

S14's finite decision and acceptance boundary is:

1. Identify the immutable DLL actually loaded by each admitted test and its
   module path, hash, exports and guest caller; retain that identity with the
   exact worker/WOW32 tested. Reuse already permitted probes without modifying
   or rebuilding shipped guest images.
2. Exercise the actual guest-owned route with same-task and cross-task/native
   peers: service registration, connect, request/response data and atom
   ownership, callback identity and ordering, normal uninitialize and task exit.
   Verify peer isolation, failure rollback, nested callback and sibling survival.
3. Compare those observations with original semantics. If a legitimate
   admitted guest requires the old host thunk, investigate its original
   `DdeGetCallbackInstance` dependency and public-host alternatives with
   exact instance/thread, reentry and retirement rules before integrating it.
   Do not activate a second DDE owner merely because source exists.

The prior initialization witness is positive but bounded; none of the
callback, conversation, teardown or old-host-thunk questions is marked
complete by this review. No product, guest, build or shared ledger changed.
