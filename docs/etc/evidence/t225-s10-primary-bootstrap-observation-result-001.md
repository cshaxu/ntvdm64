# T225 S10 Primary-Image Native Bootstrap Observation

## Question

Can the freshly Ninja-linked x64 `/MT` native CLI execute the admitted
source-built primary guest input far enough to observe the existing NTIO entry,
the mechanical `50:11 demLoadDos` commit/resume, and the first
NTDOS-owned continuation in both Direct and Readonly profiles?

## Inputs and build identity

The native executable was freshly linked in the disposable root
`build/M0-T225-S10/ninja-native-preflight-r1`. Its declared Ninja link
closure is:

- `obj/targets/src_cli_ntdos64_native_cli_c.obj`;
- `lib/bx-vdm.lib`;
- `lib/cli.lib`;
- `lib/bx-mantle.lib`;
- `lib/bx-core.lib`.

The staged primary input root is
`build/M0-T225-S10/primary-bootstrap-inputs-r2`. It is produced only from
the S8 original-toolchain source-built NTIO, NTDOS, COMMAND and SHARE stages.
The selected profile aliases byte-identical source-built `SHARE.EXE` to
`TARGET.EXE` solely because the existing CLI admission contract selects
`TARGET.COM` or `TARGET.EXE`; it does not alter its bytes or claim a new
guest artifact identity.

The observation record binds the native executable SHA-256
`84debf586ae191a9c4a1d926f7dcc4169d9e6b36b2de3595bfc4dc6cdfe70c91`
and primary input manifest SHA-256
`2bd00ab78eae08cf301d95e3cab9962bcd2655fbfcdf085ce1abce313035516a`.

## Procedure

`Invoke-T225S10PrimaryBootstrapObservation.ps1` launched the same native
executable once per admitted profile:

- Direct;
- Readonly.

For each launch, it supplied the staged profile/root, source-built configuration
and autoexec inputs, redirected stdout/stderr to the disposable run root, and
applied an external ten-second process watchdog. The watchdog terminates only
the PID it started; a watchdog termination is recorded with a null exit code.
No guest memory was read, no BOP selector was decoded by the observer, and no
Bochs, adapter or provider behavior was changed.

## Observations

The reproducible record is
`build/M0-T225-S10/primary-bootstrap-observation-r6/observation.json`.

| Mode | Elapsed | Exited naturally | Watchdog | Exit code |
| --- | ---: | --- | --- | --- |
| Direct | 10062.6264 ms | no | terminated | null |
| Readonly | 10023.9992 ms | no | terminated | null |

Both stderr logs show the same early native machine facts: CPU hardware reset,
APIC allocation, CPUID reporting, and the first RAM block allocation. Both
then report the same repeated CPU diagnostics, including unsupported LOCK
prefixes and SLDT in real/virtual-8086 mode. Neither stdout log contains a
terminal result.

The current generic output does **not** emit an NTIO-entry marker, a
`50:11` fixed-width action/continuation marker, or an NTDOS-entry marker.
Therefore this run does not establish that any of those stages was reached.

## Interpretation and boundary

This is a bounded lifecycle/observability result, not a BOP-provider result.
Direct and Readonly reach the same early native reset behavior and neither
returns within the external budget. The identical evidence does not prove a
Readonly fallback, an image mismatch, a `50:11` failure, an NTDOS failure,
or a required new Bochs device.

The first unresolved owner boundary is selector-blind native machine-stage
observation and run control: it must expose existing fixed-width lifecycle/
mechanical facts without interpreting BOP or guest DOS semantics. No
trace-led BOP implementation is admitted by this result.

## Next gate

S10 remains open. A follow-on admission must first map the existing
machine-stage budget and generic observation hooks, then choose the smallest
selector-blind evidence path capable of distinguishing reset, NTIO entry,
mechanical `50:11` commit/resume, and first NTDOS continuation. Only after
that package is implemented and re-run may this fixture identify a divergent
owner.
