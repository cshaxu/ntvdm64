# M0 T282 S14 — COMMAND COMSPEC bounded input binding

## Question

Can the original `cmdComSpec` preserve its source body and AL result while
reading the S9-proven at-most-64-byte `DS:DX` COMSPEC string without a naked
`GetVDMAddr` pointer?

## Admitted recovery ladder

1. Retain the original `cmdmisc.c` `cmdComSpec` control order and destination
   globals.
2. Use S12's bounded C-string copy over its numeric adapter-softpc descriptor.
3. Do not modify Bochs or create a new mapper/host policy.
4. If a malformed or unterminated 64-byte input has no source-defined error
   path, stop before inventing one and record the required owner decision.

## Scope

Only the `cmdComSpec` DS:DX input is admitted. This does not enable a BOP
route, change COMSPEC policy, or bind `cmdGetInitEnvironment`.

## Closure evidence

Record the source failure disposition before source modification, formally
compose original COMMAND on x86/x64, retain the S12 x86/x64 negative C-string
proof, and register any mirror divergence in the component README.
