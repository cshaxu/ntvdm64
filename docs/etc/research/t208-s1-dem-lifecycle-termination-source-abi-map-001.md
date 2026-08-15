# T208 S1 — DEM lifecycle/termination source, ABI and disposition map

## Question

What is the complete current disposition of the DEM lifecycle/termination
owner package, without choosing work from a native trace?  The bounded set is
the six contiguous original DEM services `50:38..3D`.  `50:3E` (`demWOWFiles`)
is a WOW boundary and is deliberately not part of this package.

## Inputs

| Source | SHA-256 | Role |
| --- | --- | --- |
| `src/opennt/base/mvdm/inc/dossvc.h` | `82b5135e78bd3010128bd5dd54978b0e00aec0073f0a7e6b547cbe8dff08a91c` | authoritative selector names |
| `src/opennt/base/mvdm/dos/dem/demdisp.c` | `7ea3a829e3f8aad2dfe644d101cabaff514de25909db937675fc323dfce8fe3c` | authoritative array order |
| `src/opennt/base/mvdm/dos/dem/demmisc.c` | `19676fa5995555ec0b16c2923953b4e6236467dc42bf36cc7e746f45d0fcf912` | debug and fatal-exit owners |
| `src/opennt/base/mvdm/dos/dem/demsrch.c` | `2fb56aad4d35a02dec38e9202d41b94a25076428c3d9eb08211d89f890c896ee` | PDB/search owner |
| `src/bx-vdm/bx_ntvdm_dem_*` | current worktree | current routed disposition |
| [T200/S31 ledger](t200-s31-host-integrated-bop-provider-audit-ledger-001.md) | current authority evidence | all-table owner/mutation/profile admission baseline |

The source procedure was an array-order comparison in `demdisp.c`, followed
by handler-body and current route inspection.  No guest trace selected a
service or supplied provider semantics.

## Original owner and ABI map

| BOP | Original owner and purpose | Original input/effect | Historical dependencies | Current bx-vdm disposition | Mutation/profile class | Workaround action |
| --- | --- | --- | --- | --- | --- | --- |
| `50:38` | `demmisc.c:demDOSDispRet`; developer diagnostic on DOS dispatcher return | In `DEVL` only, and only for a debuggee plus `fShowSvcMsg`, reads `SS:SP` and emits register diagnostics; ordinary build returns | `IsDebuggee`, `GetVDMAddr`, register macros, debugger output | `dem_misc_plane` resumes as a no-op | no host mutation; no-debug profile | retain only as the source-equivalent ordinary no-debug path; no guest-stack gather is admitted |
| `50:39` | `demmisc.c:demOutputString`; debugger-only guest text output | If debuggee, maps `DS:SI`, length `BX`, and calls `OutputDebugStringOem`; otherwise returns | `Sim32GetVDMPointer`, debug port, debugger output | `dem_misc_plane` resumes as a no-op | no host mutation; deferred debugger capability | retain no-debug path; a future debugger package, not CLI console, owns any debugger implementation |
| `50:3A` | `demmisc.c:demInputString`; debugger-only prompt | If debuggee, maps `DS:DI`, length `BX`, and calls `DbgPrompt`; otherwise returns | `Sim32GetVDMPointer`, debug port, debugger prompt | `dem_misc_plane` resumes as a no-op | no host mutation in selected no-debug profile; debugger input deferred | retain no-debug path; do not reinterpret it as COMMAND console input |
| `50:3B` | `demmisc.c:demIsDebug`; debugger-state query | Writes `AL=0` absent a debug port; sets debug bits only for a debuggee | `NtQueryInformationProcess(ProcessDebugPort)` and register macros | `dem_debug_service` writes only `AL=0`, preserving `AH`, then resumes | no host mutation; selected no-debug capability | retain source-derived no-debug result; a debugger profile must separately prove any nonzero state |
| `50:3C` | `demsrch.c:demTerminatePDB`; process/PDB teardown notification | `BX` is the PDB. Original calls VDD/user and host teardown hooks, then releases that PDB's `FFIND` list | VDD hooks, `HostTerminatePDB`, process state, search list | `dem_session_lifecycle_provider` releases bounded per-PDB search state and resumes | session-state mutation, independent of storage mode | retain the proven search-state cleanup; migrate/recover remaining VDD/host-process notifications only with a complete lifecycle owner contract |
| `50:3D` | `demmisc.c:demExitVDM`; fatal VDM exit | Historical function displays the bad-system-file error then calls `TerminateVDM`; it never resumes | resource dialog, historical `TerminateVDM` engine hook | `dem_cli_unavailable_provider` returns a typed controlled stop | VDM lifecycle terminal; not direct/readonly/overlay/virtual storage | retain as explicit fatal terminal fence; it must not become ordinary success or an adapter-synthesized host process exit |

`demTerminatePDB` is physically in `demsrch.c`, but it is semantically a
lifecycle notification.  Its source owner therefore spans the lifecycle
package and per-PDB search state.  This is not a justification to move search
or VDD semantics into bx-mantle.

## Current route reconciliation

The current `dem_package_session` reaches `dem_misc_plane` before
`dem_session_lifecycle_provider`, then falls through to its explicit failure
provider.  That ordering produces the following source-shaped boundary:

- `38`, `39` and `3A` are accepted only as ordinary no-debug no-ops.
- `3B` is accepted as the fixed no-debug `AL=0` response.
- `3C` is a narrow source-derived cleanup of the adapter-owned PDB search
  transaction; it is not a recovered historical host-process/VDD lifecycle.
- `3D` is an explicit controlled stop.  It is neither a DOS service result
  nor normal COMMAND/CLI completion.

The existing focused tests already establish the `3B` register contract and
the `3C` bounded PDB search cleanup.  They do not prove a complete debug
provider, VDD lifecycle, historical `TerminateVDM`, GUI error presentation or
ordinary engine completion.  These limits remain part of the package ledger.

## Findings and next package decision

The selected non-debug CLI profile has a coherent source basis for `38..3B`:
the original code itself returns before guest memory, console, or debugger I/O
when no debuggee is present.  They require no host-mutation capability.

`3C` and `3D` are intentionally incomplete historical compositions.  `3C`
has one legitimate retained session cleanup; its other original hooks belong
to unavailable VDD/host-process composition.  `3D` has a truthful controlled
terminal outcome but no current engine result/presentation contract.  Neither
gap may be hidden by a generic access-denied result or by a trace-led handler.

Therefore S2 is a **reconciliation and regression-admission** subtask, not a
new BOP implementation: it must make the six-service family regression state
the above dispositions explicitly, remove any test wording that credits the
family as recovered, and record the two deferred lifecycle dependencies for a
later engine/VDD package.  Only after that evidence is complete may T208 make
one package-level close/defer decision.  A native trace, if later run, can
only check the entire stated family outcome.

