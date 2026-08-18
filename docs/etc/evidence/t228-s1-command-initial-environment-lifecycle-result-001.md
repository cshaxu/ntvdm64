# T228 S1 P18 — COMMAND Initial-Environment Lifecycle Result

## Source Contract and Repair

OpenNT `src/opennt/base/mvdm/dos/command/cmdenv.c:179..330` keeps the
initial-environment allocation available while `IsFirstCall` remains true.
It can therefore satisfy repeated pre-`cmdGetNext` requests; after the first
command transition it reports zero rather than exposing the initial block.

The prior source-derived provider consumed its snapshot after the first
successful `54:0F` write.  P18 replaces that state with the explicit sequence
`READY -> DELIVERED -> CLOSED`: a repeated pre-`54:01` `54:0F` rewrites the
same immutable CLI-captured block, and only a successful `54:01`/`cmdGetNext`
commit closes it.  The adapter does not re-read the host environment, mutate
guest inputs, add a BOP, or alter bx-core/bx-mantle/NTDOS.

## Local and Formal Evidence

A fresh formal Ninja graph was generated in
`build/M0-T228-S1/p18-history-native-20260818b`.  During this build the
previous transient Ninja response-file rule was found not to materialize
`*.lib.rsp` under this Windows runner.  The generator now writes deterministic
per-library and per-target response files before graph execution; the fresh
graph compiled 314 required actions and linked
`bin/ntdos64-native.exe` with SHA-256
`5ea6169161e4cb494b0e77854475951f467238d99c55effe6d4f486f32388b02`.

The freshly linked focused fixtures passed:

- `t226-s3-command-comspec-bootstrap-fixture.exe` — capacity retry, repeated
  pre-close write, and closed zero-response contract;
- `t226-s3-command-bootstrap-provider-fixture.exe`;
- `t226-s3-command-bootstrap-session-fixture.exe`.

## Controlled Native Observation

One Direct run used the immutable P17 input root and the same deliberately
cleared child environment: `SystemRoot`, `ComSpec`, the two startup-source
variables, and `OPAQUE_A` through `OPAQUE_D` of 1,000 characters each.  The
run used a one-million-instruction budget plus BOP sequence, guest-exec ledger,
terminal position/history, and CS-transition observation.  Its durable local
record is `build/M0-T228-S1/p18-command-environment-reissue-20260818c`:
stdout SHA-256 `bc2b3acef2c0bf8f6385c6f72c547845352b3a29b00e35c5a7036b81ced17a2a`,
stderr SHA-256 `52de23b1a4fbd12fe3fd463888431e2b5a4b852502bad2dfff6a4b4b50025860`.

The native run reaches the two mapped `54:0F` sites at `0C41:03A6` and
`0C41:03CB`, then again reaches `0C41:03A6` with pre-dispatch `EBX=4444`.
It still has only the parent-start lifecycle record and ends through the
one-million budget terminal after the observed scalar transition
`0C41:1DC6 -> 4444:4444` (terminal `4444:33C0`).

## Result and Boundary

P18 closes the demonstrated provider divergence and its entire
initial-environment lifecycle contract.  It does **not** close COMMAND
transient placement, NTDOS MCB allocation, or native child execution: the
controlled native result preserves P17's deeper whole-owner limitation.
No trace-selected BOP or Bochs repair is admitted by this result.
