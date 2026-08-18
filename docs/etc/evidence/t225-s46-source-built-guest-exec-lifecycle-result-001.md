# T225 S46 — Source-Built Guest EXEC Lifecycle Result 001

## Scope

S46 implements only the declared default-off, copied lifecycle observer and a
fresh four-image input stager.  It does not alter a BOP provider, guest image,
OpenNT guest/host source, Bochs device, profile capability, or host mutation
behavior.

## Inputs and build

A fresh input root was produced by
`tools/build/Stage-T225S46GuestExecFixtureInputs.ps1` at
`build/M0-T225-S46/guest-exec-inputs-r1`.  Its manifest schema is
`ntdos64.t225.s46.guest-exec-fixture-inputs.v1`; its only allowed use is this
bounded Direct/Readonly guest-EXEC observation.  The four source-built inputs
are exact:

| Role | Guest name | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| NTIO | `NTIO.SYS` | 33792 | `cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937` |
| NTDOS | `NTDOS.SYS` | 27858 | `957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84` |
| COMMAND | `COMMAND.COM` | 50384 | `908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43` |
| SHARE alias | `TARGET.EXE` | 882 | `69dabbdb754b358ac4fe4b22de04c0e4c93076816f14bb0730caa9fd223996fc` |

The formal manifest-bound Ninja graph was generated in
`build/M0-T225-S46/lifecycle-r2` with MSVC x64 `/MT`, the pinned CPU5/P-MMX
configuration, and the existing five owned libraries.  It built the native
CLI, `t225-s46-guest-exec-lifecycle-observation-fixture`, and the repaired
S6 four-image fixture.  The focused observer fixture passed.  The S6 fixture
also passed when supplied its explicitly required process-local configuration
sources; its expected Overlay/Virtual rejection was retained.

## Observer contract

`bx_ntvdm_guest_exec_lifecycle_observation_v1` is disabled by default.  When
enabled, after the existing generic #UD bridge has accepted a checked resume,
it copies only these already-routed BOP identities in their source-defined
lifecycle order:

1. `50:36` — NTDOS child-entry notification;
2. `50:3C` — NTDOS child termination notification;
3. `54:0B` — COMMAND parent-return result.

It neither reads nor writes guest memory, parses a guest service payload,
changes the generic outcome, or participates in routing.  Focused regression
covers disabled behavior, out-of-order termination, the ordered three-record
sequence, exact `fault_rip + 4` resume, and rejection of a non-resume outcome.

## Paired native result

Both runs used the same fresh root, source-built `TARGET.EXE`, process-local
`fixture-config.nt` / `fixture-autoexec.nt`, tick budget `1000000`, and
`--observe-guest-exec-lifecycle`:

| Mode | Exit | Observer result | Terminal |
| --- | ---: | --- | --- |
| Direct | 4 | `entry=1 terminate=0 parent-return=0 out-of-order=0` | existing execution-budget terminal |
| Readonly | 4 | `entry=1 terminate=0 parent-return=0 out-of-order=0` | existing execution-budget terminal |

The complete raw outputs are byte-identical with SHA-256
`d46e2352dfb84fbadef3b73658e8c30f49f198ce6dd7e09d28b387336a9e0161`.
The Direct run re-hashed every staged input after execution and found no
change.  Raw logs remain only in the disposable S46 build root.

## Interpretation and limitation

This proves that the manifest-bound source-built guest path reaches the
existing no-VDD `50:36` resume once in both supported mutation modes.  It does
not prove the original `$Exec` `retf` reached SHARE, SHARE's `INT 21h/4Ch`,
NTDOS `$Exit/$Abort/reset_environment`, `50:3C`, COMMAND `54:0B`, parent
state restoration, or ordinary CLI completion.  The unchanged bounded result
is evidence for the existing guest/machine continuity owner identified by S44;
it does not authorize a trace-selected BOP, device, provider, or Bochs repair.