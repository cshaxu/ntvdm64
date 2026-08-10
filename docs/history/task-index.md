# Completed Task Index

## Numbering Decision

This is the authoritative crosswalk for historical completed work. Legacy
`M0`--`M92` identifiers are retained as evidence references, while all project
tasks use the monotonically increasing `T` series from this record forward.
`T001` maps to `M0`; therefore `T093` maps to `M92`. The active S5 packet is
`T094`; queued work remains unnumbered until admitted.

Completion means that the historical status ledger records a completed result,
which may be an implementation, audit, reconciliation, design closure, or
explicitly bounded negative conclusion. It does not imply an unrestricted
runtime capability.

## Completed Historical Tasks

| T | Legacy ID | Completed task |
| --- | --- | --- |
| T001 | M0 | Owned user-mode monitor contract |
| T002 | M1 | Generic transition contract design |
| T003 | M2 | Oracle-adapter admission |
| T004 | M3 | Owned execution-backend admission |
| T005 | M4 | Core/mantle contract delta |
| T006 | M5 | NTIO bootstrap preflight |
| T007 | M6 | NTDOS load-transaction preflight |
| T008 | M7 | Bootstrap transaction composition |
| T009 | M8 | NTIO IVT/BDA first-live dependency |
| T010 | M9 | Mantle initial-state descriptor |
| T011 | M10 | Firmware profile-input admission |
| T012 | M11 | Constructed firmware initial-state export |
| T013 | M12 | Mantle profile-composition admission |
| T014 | M13 | NTIO entry-register baseline |
| T015 | M14 | NTIO pre-instruction trace admission |
| T016 | M15 | NTIO first-instruction dependency ledger |
| T017 | M16 | SVC_DEMLOADDOS historical contract |
| T018 | M17 | NTIO image-layout provenance correction |
| T019 | M18 | NTDOS post-load first-instruction dependency |
| T020 | M19 | DEMGETDRIVES contained-namespace contract |
| T021 | M20 | DEMGETDPBLIST guest-memory contract |
| T022 | M21 | DOSINIT post-DPB dependency ledger |
| T023 | M22 | SYSINIT command-handoff dependency ledger |
| T024 | M23 | COMMAND artifact-role reconciliation |
| T025 | M24 | Contained namespace and command-service ledger |
| T026 | M25 | DOS file open/exec containment ledger |
| T027 | M26 | Historical DEM file-shim admission |
| T028 | M27 | Contained DEM file-token table |
| T029 | M28 | Historical DEM token-bridge design |
| T030 | M29 | DEM read guest-memory bridge admission |
| T031 | M30 | BYOB bootstrap resource ownership |
| T032 | M31 | Historical DEMLOADDOS resource-bridge design |
| T033 | M32 | Command-service first-cut admission |
| T034 | M33 | Command guest-record transaction design |
| T035 | M34 | Historical COMMAND object-boundary audit |
| T036 | M35 | Neutral core/mantle delta synthesis |
| T037 | M36 | Core backend-adoption gate audit |
| T038 | M37 | Core-delta handoff record |
| T039 | M38 | Historical first-cut overlay granularity audit |
| T040 | M39 | Historical fidelity/overlay decision |
| T041 | M40 | Lane P service-dossier schema |
| T042 | M41 | Lane P set-info and COMSPEC dossiers |
| T043 | M42 | Lane P environment dossier |
| T044 | M43 | Lane P CONFIG/AUTOEXEC dossiers |
| T045 | M44 | Lane P first-command dossier |
| T046 | M45 | Lane P directory/lifecycle dossiers |
| T047 | M46 | Contained DOS namespace dossier |
| T048 | M47 | Lane P trace-fixture plan |
| T049 | M48 | Backend-adoption readiness |
| T050 | M49 | Core consumer-evidence audit |
| T051 | M50 | Historical host-prefix reconciliation |
| T052 | M51 | BYOB profile-validator implementation audit |
| T053 | M52 | BYOB profile-validator fixture closure |
| T054 | M53 | Historical generated-ABI recovery audit |
| T055 | M54 | Contained-profile fixture implementation audit |
| T056 | M55 | External-backend handoff package |
| T057 | M56 | BYOB-validator regression expansion |
| T058 | M57 | Historical profile-boundary correction |
| T059 | M58 | Governed backend-readiness re-audit |
| T060 | M59 | Historical provider-archive purity |
| T061 | M60 | NT4 BOP/SVC dispatch ledger |
| T062 | M61 | DEMLOADDOS bootstrap ABI ledger |
| T063 | M62 | DEM file guest-buffer ABI ledger |
| T064 | M63 | DEM drive current-directory ABI ledger |
| T065 | M64 | DEM error/hard-retry ABI ledger |
| T066 | M65 | DEM debug-stream exit ABI ledger |
| T067 | M66 | DEM command-exit and Win32 handoff ledger |
| T068 | M67 | COMMAND bootstrap environment-stream ABI ledger |
| T069 | M68 | Guest COMMAND CMDINFO bootstrap ABI ledger |
| T070 | M69 | NT4 EN-US bootstrap capability matrix |
| T071 | M70 | Neutral backend capability handoff |
| T072 | M71 | NT4 boot minimum-machine matrix |
| T073 | M72 | NT4 boot memory BOP ABI ledger |
| T074 | M73 | NT4 A20/XMS/HMA fallback ledger |
| T075 | M74 | NT4 boot input/display/clock gating |
| T076 | M75 | NT4 executable-prefix prerequisites |
| T077 | M76 | NT4 guest-closure evidence audit |
| T078 | M77 | Historical host-closure evidence audit |
| T079 | M78 | Runner BYOB handoff policy |
| T080 | M79 | NTVDM64 current backend-gate re-audit |
| T081 | M80 | NT4 startup-service closure |
| T082 | M81 | OpenNT source-scope reconciliation |
| T083 | M82 | NT4 early-service ABI details |
| T084 | M83 | NT4 EN-US first-profile contract |
| T085 | M84 | Redacted trace envelope |
| T086 | M85 | Guest-closure governance reconciliation |
| T087 | M86 | Core second-consumer candidate audit |
| T088 | M87 | NTVDMx64 HAXM adapter-boundary audit |
| T089 | M88 | Current core archive-probe reverification |
| T090 | M89 | Historical host event/thread boundary audit |
| T091 | M90 | Frozen NVTDM reference reconciliation |
| T092 | M91 | NTVDM64 current backend-gate re-audit |
| T093 | M92 | Local-code governance reconciliation |

## Most Recent Completion

**T093** (`M92`, local-code governance reconciliation) is the most recently
completed historical task. Its closure is recorded in the former status ledger
and supported by `../../LOCAL-CODE-GOVERNANCE-LEDGER.md` and
`../../M92-OPENNT-OVERLAY-AUDIT.md`.
