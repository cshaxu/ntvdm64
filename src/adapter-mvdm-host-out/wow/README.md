# wow family

Owns the historical WOW32/WOWEXEC host-product boundary. No guest provider or
broker behavior is admitted.

## T291 S3 callback transaction

`mvdm_wow_callback_transaction` is a copied, one-session handoff for the
callback-reached numeric values. It copies the input bytes before dispatch,
uses the monitor-owned task-frame projection, and returns only the copied
AX:DX result. The app/session dispatch route is synchronous and remains the
only future machine-resume owner. It does not expose a raw guest/native pointer
or add a callback worker, CCPU executor, CSRSS broker or WOW provider.

P4/P5 construct the source-shaped packed `CBVDMFRAME` through the bounded
WOW pointer scope, then retain the original non-fast
`SETVDMSTACK`/`host_simulate`/`VDMSTACK` interval through the same-shaped
SoftPC facade.  Its original 16-byte `PARM16` extent, field order and fresh
write/read lease order are retained.  This synchronous guest re-entry never
uses the session command/control route; that route remains for
`GetNextVDMCommand`-style broker requests.
