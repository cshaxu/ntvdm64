# T194 S4 single-target COMMAND lifecycle implementation 001

Date: 2026-08-12  
Packet: M0 T194 S4  
Disposition: source/test/build closure complete; no guest runtime observation.

## Question

Can the contained COMMAND host-service plane express the T194 S3 lifecycle
without a per-BOP exception, a host command queue, or a terminal `QUIT.COM`
command?

## Inputs

- Original OpenNT COMMAND references: `src/opennt/base/mvdm/dos/command/`
  `cmddisp.c`, `cmdmisc.c`, and `cmdexec.c`.
- T194 S3 source contract.
- Existing fixed-width `CMDINFO` gather/multi-write and CPU-result ABI.
- Existing immutable BYOB profile, launch-plan, read-only namespace and
  COMMAND provider tests.

## Implementation

`byob_profile` now admits only the explicit v6 profile identity
`ntdos64-byob-profile-v6` / `nt4-en-us-command-normal-return-v6`. It requires
exactly `NTIO.SYS`, `NTDOS.SYS`, `COMMAND.COM`, and one `TARGET.COM` or
`TARGET.EXE` declaration, one declared non-terminal target placement, and
metadata for the four files visible to the guest. The older v5 profile still
requires its two declared targets and `QUIT.COM`.

The retained v2 launch-plan wire format now carries a validated slot count of
one or two. It has no new host-side queue or result field. The adapter passes
that immutable count to the existing `54:01` prepare path. A one-slot plan can
complete only the first copied `CMDINFO` transaction; a second `54:01` is
declined before guest-memory gathering. The existing `54:11` handler accepts
only after that first delivery, copies the low 16 bits of DX to process-local
state, clears CF, and resumes after the four BOP bytes. It still does not
create a process, translate a CLI exit result, or perform historical
thread/redirection work.

The read-only namespace remains provider-owned. `TARGET` is valid in either
the four-file v6 namespace or the five-file v5 namespace; `QUIT.COM` remains
valid only as the fifth v5 entry.

## Procedure and observations

Using the owned MinGW CMake build directory
`artifacts/build/current/t181-local-mingw`, the following focused executables
were built and run:

| Check | Observation |
| --- | --- |
| `byob-profile-test` | v5 acceptance/rejection remains green; the explicit v6 single-target selection is accepted and exposes exactly one non-terminal declaration. |
| `byob-launch-plan-v2-test` | the old two-slot environment encoding remains accepted; a one-slot encoding round-trips. |
| `bx-ntvdm-cmd-get-next-service-test` | legacy `TARGET -> QUIT` remains green; one target then `54:11` succeeds; early `54:11`, a second `54:01`, and repeated `54:11` are declined. |
| `bx-ntvdm-readonly-namespace-test` and `bx-ntvdm-boot-namespace-provider-test` | retained namespace/provider lifecycle tests remain green. |
| `bx-ntvdm-adapter-runtime-test --t181-v5-install` | current adapter runtime relinks and its retained v5 install branch exits successfully. |

## Interpretation and confidence

High confidence that the profile-to-plan-to-COMMAND-provider source contract
is closed for both explicitly selected modes. This proves neither that the
real guest reaches `54:11` nor that the current engine accepts and executes a
one-slot profile. It intentionally leaves Bochs, native devices, DOS guest
semantics, BOP catalogue ownership, terminal-result transport and host process
execution unchanged.

## Follow-up

T194 S5 must independently audit and, if admitted, prove the current engine's
acceptance boundary for the one-slot immutable plan before any bounded guest
observation. A failed engine admission is a composition fact, not authority to
reintroduce `QUIT.COM`, fabricate a command queue, or change Bochs.
