# S10 shard 2: DDEML registration and transaction trace

This is a source trace, not proof that either preserved guest binary is built
from this exact source. The initialization evidence is the previously recorded
`ddeml-init-*.result.json`: both real app-local guest DLLs initialized and
uninitialized as client-only; neither callback nor transactions were tested.
Client-only initialization is unsuitable for registration: `ddeml.c:2285`
rejects registration with `DMLERR_DLL_USAGE`.

## Original Win16 owner path

- `ddeml.c:92,133,218` binds the caller callback and task to an instance;
  server initialization also establishes DDE management/listening windows.
- `ddeml.c:2257-2340` validates the instance and service name, stores the
  registration with a retained string reference, and calls `RegisterService`.
  Null registration is rejected; null unregister removes all registrations.
- `register.c:70-121` synchronously sends `UM_REGISTER` or `UM_UNREGISTER`
  to the local instance list, including the registering instance, then to
  other DDEML16 and DDEML32 listening window classes. Local-send source
  semantics and external delivery must be checked separately.
- `register.c:124-140` routes the notification to `DoCallback` and deletes
  the receiver's atom reference. `dmgwndp.c:1272-1330` applies instance
  callback controls and invokes the guest callback. `ddeml.c:384` unregisters
  outstanding names during teardown. The source-level send is not itself a
  proof of an observed callback: instance flags may filter callbacks.
- `ddeml.c:784` and `stdinit.c:104-260` negotiate `WM_DDE_INITIATE`, run
  the server connect callback, and create a conversation after ACK.
  `ddeml.c:1213-1378` validates `DdeClientTransaction`; `stdptcl.c:54-254`
  maps request/poke/execute/advise to messages. Server receipt at
  `dmgwndp.c:920-967` flows through `stdptcl.c:328-449` and `MakeCallback`;
  `dmgwndp.c:615-630` executes callbacks and `QReply`; `stdptcl.c:537-692`
  sends DATA/ACK and manages associated objects. This requires message,
  atom, data-handle and callback lifetime fidelity, not just initialization.

## Existing guest test materials and finite next validation

`wow16/ddeml/tests/src/ddemo/ddemo.c:152,205,427,440` includes register,
connect, request and server callback examples. `tests/src/client/dde.c:390`
contains a request and `tests/src/server/dde.c:100,193` server callbacks.
These are original source examples, not presently verified runnable binaries;
no modification or rebuilding of guest is authorized.

A minimal independently authored isolated Win16 probe can import the unmodified
app-local guest DLL, initialize server mode with a proper MakeProcInstance
callback, create a service string, register, log synchronous callback type,
unregister, free the string and uninitialize. Receipt must include loaded
DLL path/hash, all return codes, callback count/type, task/instance identity,
and teardown results, with the existing Win3.1 and OpenNT debug binaries as
separate profiles. It must not assume their binary-source lineage. Negative
controls must include client-only registration rejection. Registration alone
cannot prove transaction semantics: an additional client/peer and message pump
must test connect, request/data payload, disconnect and object lifetime.
Original DDEML instance notification for a self-registration is a testable
source-derived expectation, not a claim until the actual guest confirms it.

No new guest probe was run in this group; no existing runtime result has been
expanded into a transaction or callback success claim. The DDEML selected
thunk and guest-source rows remain pending as recorded in `file-audit.csv`.
