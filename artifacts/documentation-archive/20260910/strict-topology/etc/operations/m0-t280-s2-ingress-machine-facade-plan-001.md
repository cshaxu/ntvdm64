# M0 T280 S2 — ingress and machine-façade declaration closure

## Scope

S2 is the first bounded implementation prerequisite from the T280/S1 workset.
It maps the original declaration and import surface of:

- `softpc.new/host/src/nt_bop.c`;
- `softpc.new/host/src/nt_aorc.c`;
- `softpc.new/host/src/nt_sas.c`; and
- `softpc.new/host/src/sim32.c`.

`nt_bop.c` and `nt_sas.c` are original adapter-backed source bodies.
`nt_aorc.c` is an original `not-host-runtime` CPU accessor wrapper: its
function-table shape is binding evidence only and it must not enter the host
runtime. `sim32.c` is an original overlay-required body: S2 records its
same-shaped transport surface, but does not compile it or create a replacement
body.

## Required routing

- The fixed-width event arrives at `adapter-bop`; only that adapter may enter
  the original `nt_bop.c` dispatcher.
- Historical `Cpu`, `Sas`, register, flag, A20 and checked-memory operations
  retain their OpenNT call shape and bind through `adapter-softpc`, then
  `adapter-bochs`. Neither original source nor any adapter starts a second
  CCPU executor.
- `TerminateVDM` and host-resource notifications resolve to `session` and
  `adapter-win32` according to the original source form. A controlled stop
  ends the session, never the surrounding app.
- Numeric DOS/VDM addresses resolve only via the guest-memory mapping-manager
  instance. Host-local temporary pointers, CRT buffers, and public Win32
  structures remain native pointers and never consume a mapping token.
- Debug/VDD/monitor branches retain the original declaration and have their
  explicit `adapter-vdm-monitor`, `adapter-debugger`, or `adapter-vdd`
  successor; S2 does not invent a broker.

## Evidence method

The ledger records a source range, original symbol group, declared import
shape, one exclusive owner chain, mapping rule, and status. Its verifier
confirms every required group appears exactly once and no row names
`bochs-core` as an OpenNT caller target.

S3 may import an adapter-backed body only after its S2 row states the adapter
declaration that will satisfy its original call form and records every
unavailable successor. `nt_aorc.c` remains excluded even though its historical
CPU4 declaration shape is retained.
