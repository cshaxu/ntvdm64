# T217 S1 — COMMAND Initial-Environment Source-Reuse Audit

## Question

Does the current CLI COMMAND initial-environment seam follow the mandatory
source-first recovery ladder, and what smallest correction restores the
OpenNT `cmdGetInitEnvironment` contract without importing the historical NT
host product shell?

## Inputs

- OpenNT owner source:
  `src/opennt/base/mvdm/dos/command/cmdenv.c`,
  `cmdGetInitEnvironment` and `cmdCreateVDMEnvironment`.
- Current composition capture:
  `src/bx-vdm/bx_ntvdm_composition_runtime_v1.c`.
- Current typed COMMAND publication service:
  `src/bx-vdm/bx_ntvdm_cmd_comspec_bootstrap_service.c`.
- Current copied host-context ABI:
  `src/bx-vdm/bx_ntvdm_command_host_context_v1.h`.
- Prior capacity evidence:
  `t214-s3-opennt-command-environment-recovery-001.md`.

## Original Contract

On the first COMMAND request, `cmdGetInitEnvironment` snapshots the process
ANSI environment, discards drive-directory entries beginning with `=`, then
uses its historical first-prefix state machines to discard the first matching
`COMSPEC=` host entry and (for DOS) first `WINDIR`-prefixed entry. It retains later
duplicates and nonempty malformed entries because the source does not require
an equals sign. It adds `PROMPT=$P$G` when absent, converts each retained ANSI
entry to OEM, and uppercases each variable name but not its value. It grows
the copied block as needed, prepends the guest-supplied `COMSPEC`, and reports
the required guest buffer size in paragraphs through `BX` before writing. It
has no 4 KiB policy limit.

`cmdCreateVDMEnvironment` is a separate later merge path: it retains a
32-bit environment snapshot and combines it with AUTOEXEC data.  It is not
silently claimed by the initial-environment seam.

## Recovery-Ladder Audit

| Rung | Result | Evidence and disposition |
| --- | --- | --- |
| 1. Direct original source | Cannot compose | `cmdenv.c` includes the historical `cmd.h`, `cmdsvc.h`, `demexp.h`, `softpc.h`, and `mvdm.h`; it directly uses CCPU register macros, `GetVDMAddr`, historical globals such as `IsFirstCall`/`fSeparateWow`, VDM termination/UI routines, and the historical build/product shell.  Those are neither available nor admissible as a modern x64 in-process dependency. |
| 2. Smallest source-derived seam | Required | Retain the original owner, filtering order, ANSI→OEM conversion, name-only uppercase rule, PROMPT default, `COMSPEC` prepend, paragraph retry, and explicit bounded failure.  Replace only CCPU guest access with the existing checked fixed-width adapter transaction and the unavailable product-shell environment lifetime with a copied CLI admission snapshot. |
| 3. External-code intrusion | Rejected | No Bochs, core, mantle, or other external-code change is needed; this is host-service composition, not machine mechanics. |
| 4. New behavior | Rejected | The behavior and failure contract exist in OpenNT source.  New policy or a synthetic capacity rule is not allowed. |

## Correction Selected

The existing seam had already removed the obsolete 4,023-byte rejection and
preserved the actual one-transfer bound: at most 65,535 bytes after the guest
`COMSPEC` is prepended. The repair now uses the same host representation and
conversion sequence as OpenNT: `GetEnvironmentStringsA` followed by
`CharToOemBuffA` for each retained entry, and retains the source's filtering
order, first-prefix state, malformed-entry acceptance, and name-only case
conversion. The copied data remain adapter-owned; no host environment handle
or pointer crosses a BOP boundary.

The 65,535-byte bound is not a replacement 4 KiB policy.  It is the largest
fixed-width, single checked guest-RAM publication transaction and is reported
through the original paragraph retry path.  Input that cannot be represented
there fails explicitly at admission rather than being truncated.  A later
whole COMMAND environment package must decide whether and how to recover the
separate AUTOEXEC merge behavior of `cmdCreateVDMEnvironment`.

## Failure And Ownership Map

| Aspect | Original behavior | Current seam after correction |
| --- | --- | --- |
| Owner | OpenNT COMMAND `cmdenv.c` | OpenNT-owned, source-derived COMMAND provider in `bx-vdm` |
| Host input | Process ANSI environment | `GetEnvironmentStringsA` copied at CLI composition admission |
| DOS conversion | `AnsiToOemBuff` | `CharToOemBuffA` |
| Guest ABI | `ES:0`, `BX` paragraphs | Existing checked transaction and typed `BX` result |
| Too-small guest buffer | Required paragraphs in `BX` | Same paragraph result, no write |
| Capture/allocation failure | Historical VDM termination path | Explicit composition-admission failure; no fabricated environment |
| Later AUTOEXEC merge | `cmdCreateVDMEnvironment` | Deferred to its complete COMMAND package |

## Verification

`Invoke-T217S2CommandInitialEnvironmentProbe.ps1` source-builds the new
capture seam and its focused fixture with MSVC x64 `/MT`. The fixture injects
mixed-case ANSI data, a host `COMSPEC`, `WINDIR`, and an absent `PROMPT`; it
proves the retained entry has an uppercased name, the first
`COMSPEC`/`WINDIR` are filtered, and `PROMPT=$P$G` is supplied. A
source-permitted duplicate/malformed block also passes the copied-context
validation, which ordinary Win32 environment APIs cannot construct for this
test. The seam itself uses `CharToOemBuffA`, matching the OpenNT ANSI→OEM
conversion call class.

The existing x64 `/MT` host-context regression again proves a 5,001-byte
double-NUL environment travels through `54:02`/`54:0F` and exceeds the former
4 KiB threshold. Finally, the full current-source composition fixture links,
runs, and reaches its unchanged controlled `50:3D` terminal. A native trace
is integration evidence only; it did not select this correction or establish
COMMAND execution.

## Follow-Up

T217 S8 completes the source-level semantic repair and focused regression.
Resume the paused SYSINIT command-name realization map separately. Do not use
this repair to claim the separate AUTOEXEC merge or COMMAND execution is
complete.
