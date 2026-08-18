# M0 T226 S5 — DPMI package code-complete formal result

## Scope

This result closes the local code-complete gate for the OpenNT DPMI
`53:00..18` owner package under the current Direct/Readonly CLI profile. It
does not claim a DPMI runtime, protected-mode guest execution, or a historical
NT host composition.

## Source and failure disposition

The retained [T199 DPMI source/ABI/failure matrix](../research/t199-s10-dpmi-source-abi-failure-matrix-001.md)
identifies the original 25-entry `DpmiDispatchTable` and groups the services by
protected-mode/LDT, interrupt/fault, DOS bridge, extended memory, session,
debug and VDD ownership. Those require the legacy protected-mode composition;
no source-derived adapter substitute is admitted. The current whole-package
result is therefore an explicit typed unavailable stop, preserving service
identity and never reporting success or returning a raw `#UD`.

## Formal build and local result

The pinned MSVC x64 `/MT` Ninja graph was generated in
`build/M0-T226-S5/dpmi-package-20260817a`. The formal
`t226-s5-dpmi-package-session-fixture.exe` built and returned zero. It invokes
every `53:00..18` service with a protected-mode copied CPU state and verifies
that each reaches the package session and returns `STOP`.

The older global composition fixture intentionally owns mocked mantle
capabilities and is retained as unit evidence; it is not a valid full-link
target. It was deliberately excluded rather than introducing test-only
mechanical stubs into product composition.

## Code-complete disposition

All 25 T226 S1 DPMI rows have source-proven owner/failure disposition and a
formal local negative assertion under Direct/Readonly. Overlay remains ABI-only
and native guest trace is not part of this result.