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

P4 now constructs the source-shaped packed `CBVDMFRAME` through the bounded
WOW pointer scope: its original 16-byte `PARM16` extent, field order and
fresh write/read lease order are retained. The actual source
`SETVDMSTACK`/`host_simulate`/`VDMSTACK` interval remains P5 work; the current
session route is only a copied result handoff and does not claim guest callback
execution.
