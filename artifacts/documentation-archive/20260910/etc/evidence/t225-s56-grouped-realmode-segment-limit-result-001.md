# M0 T225 S56 — grouped real/V86 segment-limit compatibility result 001

## Scope

This is one group-completion record, not a trace-selected repair.  It covers the
registered default-off, selector-blind CPU compatibility profile derived from
OpenNT SoftPC CCPU's default real/V86 segment-limit behavior.

## Implemented mechanical groups

| Group | Native handling under enabled real/V86 profile | Regression evidence |
| --- | --- | --- |
| Scalar | Existing virtual access checks retain descriptor, type, paging and linear-access behavior while omitting only the historical real/V86 segment-span rejection. | Word and dword reads/writes crossing `FFFFh`. |
| RMW/string | RMW access uses the same checked virtual path; repeat speedups decline so the native per-element access path remains authoritative. | `INC word [FFFFh]` and `REP MOVSW` crossing the boundary. |
| Stack | Stack prefetch does not cache a segment-limited host window; existing virtual stack accesses remain owner of access behavior. | `PUSH`/`POP` at `SS:FFFFh`. |
| Fetch/control | Real/V86 control-transfer checks and prefetch-window limiting suppress only the historical segment-limit rejection; protected mode remains gated off. | Operand-size 32 far jump to `CS:00010000h`, ordinary target fetch, RAM write and controlled `#UD` stop. |

The fixture first proves disabled native Bochs rejection for both a scalar
word read and the far jump.  With the profile enabled it reaches the
fixture-only controlled stop and verifies all resulting ordinary RAM bytes.
It also proves the predicate is inactive after the CPU mode is set to
protected mode.  Existing S30's default-off first-fault regression still
passes.

## Formal build and test

A fresh disposable CPU5/P-MMX MSVC x64 `/MT` Ninja root was generated at
`build/M0-T225-S56/compat-groups-20260817b`.  The generator wrapper was
corrected to restore Ninja's caller directory after `VsDevCmd.bat`, so MSVC
library/link response files resolve under the build root.

Passed:

- `t225-s56-realmode-segment-limit-compatibility-fixture.exe`
- `t225-s30-segment-access-observation-fixture.exe`

## One paired native observation

The locked original-toolchain source-built NTIO, NTDOS, COMMAND and target
inputs were staged once.  The CLI's existing startup-configuration contract
also requires the staged `fixture-config.nt` and `fixture-autoexec.nt` to be
passed through `NTDOS64_STARTUP_CONFIG_SOURCE` and
`NTDOS64_STARTUP_AUTOEXEC_SOURCE`.  The native CLI was invoked once in each
supported mode with the same inputs and a 1,000,000-tick budget; no BOP,
first-fault or instruction-history observation option was enabled.

| Mode | Exit | stdout SHA-256 | stderr SHA-256 | stderr bytes | Terminal |
| --- | ---: | --- | --- | ---: | --- |
| Direct | 4 | `6442c836088d95943d52e506e84895fecddfad88647e8d324da1979f592637f8` | `39bbea117986b48b8eca12a26bf767b704c5ad67a1861e289e78f68049271607` | 383 | `terminal=4 detail=0 lifecycle=5 presentation=5` |
| Readonly | 4 | `6442c836088d95943d52e506e84895fecddfad88647e8d324da1979f592637f8` | `39bbea117986b48b8eca12a26bf767b704c5ad67a1861e289e78f68049271607` | 383 | `terminal=4 detail=0 lifecycle=5 presentation=5` |

Both runs installed composition, entered the native machine stage, reached the
finite execution budget, and emitted the same original Bochs marker:
`BOUND_GdMa: fails bounds test`.  They are byte-identical across Direct and
Readonly.

An initial unconfigured invocation returned `terminal=2 detail=53` in both
modes.  The stage enum identifies 53 as
`STARTUP_CONFIGURATION_BIND`; the audit found that the invocation had staged
but not supplied the two required startup-configuration environment variables.
The configured rerun above supersedes that incomplete procedure.  It is not a
CPU, BOP, guest, or composition implementation defect.

## Closure limitation and transfer

The formal fixture establishes the full admitted mechanical profile and the
paired native run proves it can enter the current machine composition in both
supported modes.  The later `BOUND` marker is a single native observation, not
a new leaf implementation admission.  S56 therefore closes its CPU boundary;
the next S must globally reconcile the current BOP owner/package disposition
before any further provider work, using trace only after a selected package
has been completed.