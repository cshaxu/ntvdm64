# M0 T226 S4 — XMS package code-complete formal result

## Scope

This result closes the local code-complete gate for the OpenNT XMS
`52:00..0B` owner package. It does not claim native guest execution, a
complete historical NT host composition, or implementation of future Overlay
behavior.

## Formal build and local result

The pinned MSVC x64 `/MT` Ninja graph was generated in
`build/M0-T226-S4/xms-package-20260817a`. The following targets built and
returned zero:

- `t226-s4-xms-package-session-fixture.exe`
- `t226-s4-xms-plane-fixture.exe`
- `t226-s4-a20-mantle-fixture.exe`
- `t226-s4-extended-memory-mantle-fixture.exe`
- `t226-s4-xms-native-mantle-fixture.exe`

The native mantle fixture supplies the fixed 12-byte XMS move descriptor at
`SS:BP`, verifies `52:01`'s controlled resume, and observes the copied bytes
through ordinary guest RAM. It is a local mechanical test, not a guest trace.

## Endpoint disposition

`52:00` is A20; `52:01` is move; `52:02..05`, `0A`, and `0B` use the retained
XMS session and mantle extended-memory contracts for allocation, free,
lock/query, free-memory query, and reallocation. `52:06..08` (UMB) and
`52:09` (INT15 hook) remain explicitly typed deferred stops because their
separate mantle contracts have not been admitted. The complete 12-row plane
fixture proves selector, service identity, provider identity, and disposition;
the package-session fixture exercises the admitted and deferred outcomes.

## Code-complete disposition

T226 S1 binds all twelve XMS identities. This result provides one local
source-recovery disposition and assertion per row under Direct/Readonly.
Neither trace reach nor a native guest result selected this work; all host and
machine semantics remain in their respective provider and mantle owners.