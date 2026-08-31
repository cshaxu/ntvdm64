# M0 T325 S2 — Public Workstation-Query Lease Recovery Plan

## Selected Bodies

Only these original calls are admitted:

1. `VrGetComputerName`: `GetComputerName` result copied to `ES:DI` after the
   original `LM20_CNLEN` gate.
2. `VrGetUserName(BX=1)`: `NetWkstaUserGetInfo` result copied to `ES:DI` only
   after the original supplied-capacity gate.
3. `VrRemoteApi` local-computer fallback: `NetWkstaGetInfo` host buffer is
   copied into a local temporary then freed before original remote branching.

## Binding Rule

`mvdm_guest_location` is the only guest-span path. For an admitted `ES:DI`
write, derive its real-mode location from the original registers, acquire an
exact writable lease, copy the already-validated bounded bytes, commit/release
before return, and map lease failure to the original selected error branch.
No native pointer is retained or supplied to asynchronous work.

## Mirror Rule

Retain the original function definitions, public calls, control order,
capacity rules and register outcomes. A direct `LPSTR_FROM_WORDS` write has no
source release point, so replace only that write with a named, source-shaped
adapter call or isolated overlay hook. Register every such line as a mirror
divergence; no generic `LPSTR_FROM_WORDS` rewrite is permitted.

## Verification

Build selected original bodies and the minimal binding on both formal x86 and
x64 graphs. Test successful bounded copy, insufficient capacity before guest
write, host API failure, missing/stale session lease and paired
`NetApiBufferFree`. Do not claim remote/RAP guest networking.
