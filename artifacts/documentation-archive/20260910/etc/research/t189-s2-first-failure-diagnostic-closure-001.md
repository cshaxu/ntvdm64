# T189 S2 first-failure diagnostic closure 001

Date: 2026-08-12  
Packet: M0 T189 S2  
Disposition: source/static closed; no Bochs build or guest observation.

The adapter copied lifecycle snapshot now adds fixed-width
`boot_file_diagnostic`: `NONE`, `PROVIDER`, `PREPARE`, `QUEUE`, or `ACCEPTED`.
It is reset per v2 dispatch and is set only by the already-existing `54:0C/0D`
boot-file compound chain. It does not change provider, queue, result, resume,
session, or guest state.

BX-TRACE-061 was registered before `exception.cc` changed. Under the existing
default-off lifecycle trace, Bochs formats only the copied integer. It does
not inspect BOP bytes, selector/service, guest memory/state, payload, path or
session layout and has no new control-flow branch.

Passed: the adapter diagnostic-state static boundary, the Bochs trace static
boundary, and `bx-ntvdm-adapter-runtime-test --t181-v5-install`. The controlled
v5 fixture retains resume RIP `0x736` and its original multi-write at `0x91059`,
and now asserts `ACCEPTED`. This does not prove the real runtime category;
a separate derivative build and one-shot observation remain required.
