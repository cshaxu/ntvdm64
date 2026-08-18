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
inputs were staged once.  The current native CLI was then invoked once in
Direct and once in Readonly mode, with identical inputs and a 1,000,000-tick
budget and no BOP, first-fault or instruction-history observation option.

| Mode | Exit | Terminal output SHA-256 | stderr SHA-256 | Result |
| --- | ---: | --- | --- | --- |
| Direct | 3 | `cffc211f1f8953e7bbcbb925385adc735269681f38a38e3cd07839e8317bd9c2` | `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` | `terminal=2 detail=53 lifecycle=3 presentation=3` |
| Readonly | 3 | `cffc211f1f8953e7bbcbb925385adc735269681f38a38e3cd07839e8317bd9c2` | `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` | `terminal=2 detail=53 lifecycle=3 presentation=3` |

The engine maps a failed composition install to terminal kind 2 before machine
stage creation.  Therefore this paired result is byte-identical pre-machine
composition rejection; it neither exercises nor disproves the newly completed
CPU profile, and it does not authorize a follow-on BOP, guest or CPU patch.
S56 remains active only because its required native machine-stage evidence has
not yet been admitted by a successfully installed current composition.