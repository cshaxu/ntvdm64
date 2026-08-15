# M0 T199 S7 COMMAND Package Facade Regression

`bx_ntvdm_command_package_facade_v1` is the sole package admission point for all seventeen callable COMMAND services.

- `03` routes to the original compile-disabled no-op provider.
- `08` and `0A` now yield typed controlled stop as S6 required.
- Existing bounded candidates route to retained providers only after facade admission.
- Deferred members produce no CPU or RAM result.
- `54:11` remains rejected before the facade.

The composition fixture now enumerates `54:00..10`, verifies every package route, and verifies direct no-op resume and explicit-unavailable stop. Existing candidate positive/negative checks remain in the same source-built fixture. This is a package implementation boundary, not a claim that any historical COMMAND component is closed.

`tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1` source-built, linked, and ran under MSVC x64 `/MT` with `artifacts/build/t199-s7-command-facade-r2`; its JSON record reports zero link and run exits. No bx-core, bx-mantle, Bochs product, OpenNT runtime, or guest trace entered the fixture.

The rejected r1 build found only a `/WX` signed/unsigned fixture comparison; r2 corrects that test-only type mismatch. Next T199 work is the whole XMS source/ABI/failure map.
