# S10 shard 2: original guest DDEML selected owner group

## Inputs and source roles

The original guest `wow16/ddeml/makefile` selects `ddeml.obj`,
`dmgmon.obj`, `heapwach.obj`, `dmgwndp.obj`, `dmgdde.obj`, `register.obj`,
`dmgutil.obj` and other guest objects into DDEML.DLL. This is guest build
metadata only: it does not authorize compiling/replacing the immutable guest.
The debug makefile only adds `WATCHHEAPS` if `DBGHEAPS` is selected, so the
body of `heapwach.c`, which is behind `#ifdef WATCHHEAPS`, cannot be inferred
active merely because an object name is on the link list.

| Shard source | Original role and caller | Runtime status |
| --- | --- | --- |
| `dmgdde.c` | Original synchronous transaction helper `timeout()` runs a DDE modal loop and timer; `stdptcl.c` calls it when executing a transfer. | Single-instance init/uninit witness does not start a transaction or enter this helper. |
| `dmgmon.c` | `ddeml.c` installs `DdePostHookProc` and `DdeSendHookProc` on monitor mode and unhooks them at retirement; functions feed `MonitorBroadcast`. | Client-only probe does not enable monitoring, exercise hooks, or prove cross-task notification. |
| `register.c` | Original `ddeml.c::DdeNameService` calls `RegisterService`; it sends register/unregister to original DDEML16/32 listening classes, while `dmgwndp.c` calls `ProcessRegistrationMessage`. | Probe registers no service and proves neither peer discovery nor atom lifetime. |
| `heapwach.c` and `heapwach.h` | Optional `WATCHHEAPS` debugging, called from guarded `dmgmem.c` allocation/free paths. | Do not claim selected debug DLL has this optional behavior without examining its actual map and flags. |
| `ddemlp.h` | Original guest internal types, function declarations and shared constants included by manager bodies. | Header identity is not ABI execution, and source-to-prebuilt binary lineage is unproved. |
| `debug/ddeml.obj`, `debug/dmgwndp.obj`, `debug/dmgutil.obj` | Historical object inputs with same-path byte identity from permitted opennt-src-2 supplement; historical `debug/ddeml.map` names corresponding object/source line records. | Actual linking of these exact object bytes into current tested DLL is not established. |

## Prebuilt map correlation and bounded witness

The tested `src/mvdm/wow16/ddeml/debug/DDEML.DLL` NE entry table was compared
with the adjacent historical `ddeml.map`. The recorded segments/offsets match
for five named exports: `DdeInitialize`, `DdeUninitialize`,
`DdeClientTransaction`, `DdePostHookProc` and `DdeSendHookProc`.
`ddeml-debug-map-correspondence.json` records exact DLL/map hashes, ordinal
entries and line numbers. Matching exports supports structural correspondence;
it is not a full map/binary, object/binary or source/binary equivalence proof.

`ddeml-init-evidence.json` documents a different, separately bounded runtime
witness: both an original Win3.1 DLL and this prebuilt debug DLL loaded
app-locally, rejected nonzero reserved argument, initialized, reinitialized the
same instance and uninitialized, then rejected repeated uninitialize. No
service name, peer, actual DDE transaction, monitor class, cross-task callback
or failure injection was requested; callback invocation was not instrumented.
The Win3.1 binary has a different hash and is not identified as built from
the debug source/map. None of the above justifies marking individual guest
source files fully restored.

## Next verifiable boundaries

Retain all listed file rows as pending. An independent guest/client-peer
fixture would need service registration/atom delivery, normal and failed
transactions, monitor mode, nested callbacks, instance/task shutdown and
optional debug profile identification, paired with exact immutable guest
identity. Examine source lineage before using line-level guest source or map
to characterize an installed binary. Never patch or rebuild the guest.
