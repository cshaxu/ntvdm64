# r12 Adapter Runtime Fixture

## Claim

The first source-derived adapter transaction executes through a real Bochs
process: reset → observed marker → profile-validated NTDOS payload commit to
ordinary RAM → resume → controlled stop.

## Reproduction

The r12 fixture executable is a direct, explicit link of the r9-seeded
Bochs libraries with the listed adapter, CPU exception and memory objects. It
does not call the recursive root Makefile target. Run:

```powershell
tools\Invoke-Bochs26NativeResetFixture.ps1 `
  -BuildRoot artifacts\build\bochs-2.6-native-nogui-ucrt-r12 `
  -AdapterRuntimeFixture -BenchmarkMillions 1
```

The fixture creates a temporary, strict BYOB profile with NTIO.SYS,
NTDOS.SYS and COMMAND.COM all SHA-256 admitted before Bochs starts. Its reset
ROM is exactly `mov di,08b0h; C4 C4 50 11; hlt`; it contains no DOS kernel or
filesystem behavior.

For the adapter-runtime variant, the generated Bochs configuration also uses
the stock declarative gate:

```text
plugin_ctrl: unmapped=0, biosdev=0, speaker=0, extfpuirq=0, parallel=0, serial=0
```

That keeps optional default plugins out of the reset closure. It does not
alter Bochs source or claim that the remaining hard-wired device set is
minimal.

## Observed Evidence

`artifacts/build/bochs-2.6-native-nogui-ucrt-r12/ntdos64-reset-fixture/bochs.log`
contains, in order:

1. `Bochs benchmark mode is ON (~1 millions of ticks)`
2. `ntdos64 adapter transaction committed`
3. `cpu loop quit, shutting down simulator`

The second line is emitted only after the adapter session has accepted the
identity-checked resource set, prepared DEM `50:11`, and Bochs has accepted
the all-or-nothing ordinary-RAM copy. The fixture predicate requires all three
lines. It is therefore execution evidence for the first commit path, not a
claim that NTDOS has booted or that COMMAND is runnable.

The current r12 explicit link additionally verifies the same-boundary pending
operation rule: Bochs returns the copied exception event and CPU snapshot to
the adapter before it may take the prepared write. A stale or mismatched
boundary cancels the pending transaction; this tightening was separately
tested in the adapter session unit target before the fixture was rerun.

## Limits

The r12 executable still links the stock Bochs hard-wired device libraries and
is a temporary execution baseline, not the final adapter-owned machine
composition target. It establishes the CPU/memory/adapter boundary before the
composition extraction. No full recursive Make build was used for this link.
