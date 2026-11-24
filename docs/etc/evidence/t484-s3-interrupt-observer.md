# T484 S3 P186: native interrupt observer build

The diagnostic driver remains relevant: Win31 identity keyboard/blocked-input
cases and Win31/Win95 editor runners accept its image through InterruptObserver.
The image provides reflected IRQ observation and explicit diagnostic stimuli;
it is not evidence that every Full run executes those scenarios.

The standalone `build-interrupt-observer.ps1` producer is deleted. The existing
`tools/build-fixtures.cmd x86 interrupt-observer` graph owns NASM object assembly,
WLINK response generation and linking. README names the new cached output.
The ASM source, segment attributes, observe_ddb ordinal and command flags remain
unchanged. WLINK plus its colocated DLL/default configuration are dependency
inputs, following the existing addon toolchain discovery convention. Missing
WLINK leaves other fixtures available but explicitly fails this requested target.
No original guest or production source changed. The old fresh-root build.json
is no longer produced; the native graph retains source/tool dependencies and
this delivery records equivalence. Runtime callers consume the driver image.

## Verification

Owned background PID6684 completed with exit0 in
`build/t484-s3-interrupt-observer/result.json`:

- Object and linked .386 exactly match the previous producer recipe.
- A repeated native build preserves the image modification time.
- A dedicated missing-linker configuration successfully configures, then fails
  the requested target with the required explicit diagnostic.

The initial oracle wrapper used forward slashes in its repo prefix and failed
its own Windows path check before assembly. Initial result/failure records are
retained; correcting that test wrapper produced the final passing comparison.
The old recipe is only a comparison oracle, not a remaining project producer.

Cached full and11 raw/22 published hash verification are recorded by
`delivery-result.json` in the same directory. Unchanged products retain their
existing publication. No fresh guest runtime or Full pass is claimed.

The fixed inventory remains316/355 disposed,39 pending. P183's additional18
NASM scripts now have four handled producers and14 pending reconciliation.
Five supplemental callers still retain shared preparation/staging work; this
standalone producer is fully retired rather than counted among those callers.
