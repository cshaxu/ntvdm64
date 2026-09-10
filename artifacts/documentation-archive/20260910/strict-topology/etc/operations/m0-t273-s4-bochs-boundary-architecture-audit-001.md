# M0 T273 S4: Bochs foundation boundary audit

## Mirror and exception reconciliation

The admitted `bochs-core` set was SHA-256 compared path-for-path with the
pinned local Bochs 2.6 baseline.  Of 207 non-README files, 178 are byte
identical, 29 differ, and none lacks its baseline-relative counterpart.
`bochs-core/README.md` now indexes each difference family, its original
purpose, minimal disposition and files; executable divergence bodies remain
in the private overlay.

## Directed-boundary results

- `bochs-core` has no include of `adapter-*`, `app`, `session` or `opennt-*`.
- After excluding the mirror and its private overlay, the only `bochs-core`
  includes are five `adapter-bochs` assembly files:
  `machine_facade.cc`, `minimal_machine.cc`, `minimal_pic.cc`,
  `minimal_product_shell.cc` and `minimal_sim.cc`.
- No source outside `bochs-core`/`bochs-core-overlay` references the private
  overlay. The overlay is therefore not a public component dependency.
- `adapter-bochs` has no OpenNT, DOS, VDM, WOW, BOP, SoftPC/CCPU or Win32
  service token in production source. The lone negative explanatory comment
  was made neutral during this audit.

These results prove the intended dependency direction. They do not claim the
private overlay has zero implementation: it contains registered, mirror-owned
mechanical bodies required by the minimal Bochs closure.

## Host-width revalidation

Using only disposable `build/M0-T273-S2/r005*` outputs, both focused fixtures
were recompiled, linked and executed under MSVC `/MT` on each host width:

- x64: retained-machine two-budget stop/resume and app/session teardown both
  exited `0`.
- Win32/x86: the same two fixtures both exited `0`.

Each run emitted only native reset/CPU5 CPUID/RAM-allocation diagnostics. No
guest image, provider, BOP, firmware product boot or full device/plugin shell
was loaded.

## Disposition

T273 is closed with a source-audited minimal mechanical machine foundation.
It supplies opaque create/load/run/stop/resume/destroy mechanics and an app
teardown shell, not a runnable OpenNT virtual machine. Canonical OpenNT source
supply is the next dependency package.
