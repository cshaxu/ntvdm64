# T198 S92 normal-return candidate prerequisite map 001

## Question

Is the first source-built normal-return candidate presently available, and what
must the current native composition prove before another run can meaningfully
seek COMMAND `54:11`?

## Inputs and procedure

This is a read-only reconciliation of the BYOB v6 root, retained source-build
manifests, the current native fixture and composition, original OpenNT startup
and termination sources, and the one prior v6 observation. Every present
artifact was re-hashed; no guest was executed and no target was created.

## Candidate identity

`artifacts/inputs/t194-v6-normal-return-root-r1` contains all four declared
profile inputs. Its v6 profile selects `TARGET.EXE`, and its byte counts and
SHA-256 values match the file contents:

| Role | Bytes | SHA-256 | Source/provenance result |
| --- | ---: | --- | --- |
| NTIO.SYS | 33,792 | `cfc8be16…b3ab4937` | Existing source-built profile input. |
| NTDOS.SYS | 27,858 | `95766232…7f93bc84` | Existing source-built profile input. |
| COMMAND.COM | 50,384 | `908a77ac…c732c43` | Existing source-built profile input. |
| TARGET.EXE / SHARE.EXE | 882 | `69dabbdb…3996fc` | Exact source-built SHARE normal-return candidate. |

Both retained SHARE locations have the same 882-byte identity, which matches
`share-build-manifest.json` and the T177 recovery manifest. The latter records
the OpenNT `share.asm` source path, historical MASM/LINK16 inputs, no guest
source transformation, and internal-research artifact status. Thus the
candidate is present and provenance-eligible for a future bounded fixture; it
is not fabricated.

## Original guest path and host-service ownership

SHARE sets `AH=4Ch`, `AL=0`, and invokes `INT 21h`. A normal DOS child return
must then traverse the DOS PDB-termination route (`50:3C` carries the
terminating PDB in BX) and COMMAND's `tcode.asm` return path. COMMAND copies
its return code to DX, queries the current drive through `INT 21h/AH=19h`,
passes the RdrInfo pair in CX:BX, and emits `CMDSVC SVC_RETURNEXITCODE`, i.e.
`C4 C4 54 11`. The original COMMAND dispatcher maps that service to
`cmdReturnExitCode`; its source defines `DX` as guest exit code and CF clear as
the end of the shelled-out session.

This is a future expectation only. It neither conflates `54:11` with a host
process exit code nor bypasses the separately required engine-owned result
transport.

Before that sequence can occur, the DOS startup has an ordered prerequisite
chain. The original sources and current global composition establish these
relevant facts:

| Boundary | Original owner/contract | Current contained disposition |
| --- | --- | --- |
| `50:11` | DEM `demLoadDos`; publishes NTDOS for NTIO startup. | Source-derived bounded transaction, native-proven. |
| `12`, `15/AH=88` | Machine memory queries. | Individually admitted mechanical results. |
| `50:0D`, `50:0F` | DEM GSET boot drive and drive count. | Source-derived profile-drive snapshot routes; `50:0F` requires a pre-bound immutable snapshot. |
| `50:1B`, `50:32`, `50:46` | DTA, hard-error, and DPB startup structures. | Existing bounded component routes; no general file/device ownership. |
| `50:3B` | DEM `demIsDebug`, which returns AL debug flags. | Existing CLI false branch: AL=0, preserving AH. |
| `54:0C`, `54:0D`, normal file lifecycle | COMMAND config/autoexec materialization and later DOS file access. | Must use the existing bounded namespace/session planes; no ambient host filesystem. |
| `50:3C -> 54:11` | DOS child PDB cleanup then COMMAND normal-return result. | Not runtime observed. |

The retained T194 v6 trace observed only `50:11`, machine selector `12`,
`50:3B`, then `50:0F`; it never reached the candidate, `50:3C`, or `54:11`.
Its watchdog outcome is not a guest result. S91 independently proves that the
newer stream fixture's later `50:3D` is a different fatal configuration path.

## Current native-fixture blocker

The current S89/S90 native fixture is deliberately a startup/mechanics
fixture, not the v6 profile: it embeds exact NTIO/NTDOS arrays but substitutes
`COMMAND.COM` with `{90,C3}` and the target with `{F4}`, while manually
constructing a profile-shaped selection. It does pre-bind a C-drive snapshot,
so it can exercise the `50:0F` route, but it cannot prove source-built COMMAND
loading, SHARE execution, PDB termination, or `54:11`.

Therefore a repeat of that fixture cannot advance the normal-return question.
The concrete pre-observation blocker is test-input fidelity, not a missing
Bochs device, new BOP recognizer, or terminal-result ABI.

## Result and follow-up

S92 admits one narrow next step: generate compile-only fixture arrays from the
already pinned source-built COMMAND.COM and SHARE.EXE inputs, bind them through
the existing immutable namespace/composition and C-drive snapshot, and retain
their hashes in the build evidence. That step must preserve the existing
profile-selected path and must not run until its source membership and
observation markers are explicit. It may not add a provider, expose a host
filesystem, implement `50:3D`, or carry a DOS status out to CLI.

## Confidence

High for candidate identity, original normal-return contract, and the fixture
mismatch because each is directly inspected. Medium for the complete runtime
order after `54:0C`: it is source-derived and prior-trace-informed, but no
current native trace reaches the actual source-built COMMAND image.
