# M0 T276 S16: W3 `sim32` package disposition plan

## Objective

Audit every selected `sim32` source path as one original monitor/guest-memory
translation package. Record its original build role, SIM32 pointer/read/write/
flush API form, and its dependencies on the unique session mapping manager,
`adapter-softpc`, `adapter-vdm-monitor` and `adapter-bochs` before enabling a
SIM32 body.

## Non-goals

No source move/edit, SIM32 implementation, pointer lease implementation,
Bochs change, adapter body, formal build edge, guest execution or trace-led
consumer recovery.

## Acceptance

Every selected path has one non-enabled disposition. The audit must explicitly
preserve the historical API shape while prohibiting raw guest/host pointer
exposure and a second mapping manager.
