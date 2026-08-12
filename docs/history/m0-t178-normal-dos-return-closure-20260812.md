# M0 T178 normal DOS-return attribution closure - 2026-08-12

T178 closes without implementation. It proves that `50:36` is the original
pre-target DEM notification and not a return signal; a normal source-built
SHARE path proceeds through INT 21h/AH=4Ch and an accepted `50:3C` PDB
termination BOP. The next owner is original DOS teardown and parent-state
restoration. COMMAND `54:11` is not observed before the existing stack/control
failure.

Accordingly, CLI result transport is rejected as premature. Bochs process exit,
watchdog state and the historical COMMAND host-process status do not represent
the guest exit code. The transfer is to an unnumbered queue candidate for
guest teardown/control-state attribution; it is not a request to expand BOP
providers or adapter semantics.

The retained `history/m0-t178-closure-20260812.md` is a pre-admission
historical locator and remains unchanged.
