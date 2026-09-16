# T416 S2 — original WOW command-carrier restoration

## Question

Can the standalone session fallback for a missing WOW `-a` command-line tail
be removed while preserving the original OpenNT `run16 -> BaseGetVdmConfigInfo
-> ntvdm -w -a KRNL386` construction and all established DOS behavior?

## Inputs and source-first disposition

- S1 baseline commit `2031f14de` and its fixed measurement tool.
- Original `base/win32/client/vdm.c::BaseGetVdmConfigInfo` and selected
  `dos/command/cmdmisc.c::GetWowKernelCmdLine`.
- Current `run16_entry.c`, `base_config.c`, session, firmware adapter and
  formal x86 graph.

The original source already owns both needed algorithms:

1. `BaseGetVdmConfigInfo` appends `-w -a <KRNL386>.exe` for a Win16 record.
2. `GetWowKernelCmdLine` finds the final ` -a ` in the worker OS command line
   and fails through its existing original environment-error route if absent.

The removed fallback was project-authored behavior: it copied a session field
only when the original command-line carrier was absent. A complete caller
sweep found **no setter** for that field in `src`, `tests` or `tools`; it
could therefore never be a live product provider.

## Change

- Restored `cmdmisc.c` to the original missing-`-a` branch.
- Deleted `mvdm_softpc_wow_bootstrap_kernel_copy`, its declaration, the
  session storage field, setter and getter.
- Retained the finite `base_config.c` binding: it supplies an immutable
  package value to original `BaseGetVdmConfigInfo`, and does not classify an
  application or replace worker parsing.

## Verification

1. `node tools/audit/Verify-T416S2WowCommandCarrier.mjs` passes: complete
   fallback absence, original parser, run16's original command construction
   call, and the exact `CreateProcessW` command-buffer handoff.
2. `tests/broker/original_server_lifecycle.c` rebuilt in
   `build/M0-T416/S2/original-lifecycle-001`; direct approved local execution
   passed every row, including original DOS/WOW worker construction, package
   spaces, session ID and rejection. Its restricted runner received `EPERM`,
   an execution-environment limitation rather than a fixture failure.
3. Node 22 generated `build/M0-T416/S2/formal-x86-002`; all **478/478**
   sequential formal commands passed, including the three final links and
   VDM_TIB gate. `run16.exe`, `basesrv.exe` and `ntvdm.exe` are x86 (`014c`).
4. `git diff --check` and documentation governance pass.

No `O:\winnt` publication is made: this removes an unreachable WOW-only
fallback and does not claim a new full-WOW/WRITE runtime result. The fresh
formal link is the applicable gate; DOS/nested-COMMAND integration remains
T416 S5 acceptance.

## Measured result

| Surface | Before | After | Net reduction |
| --- | ---: | ---: | ---: |
| MVDM paired mirror distance | 4,695 | 4,685 | **10 lines** |
| `cmdmisc.c` distance | 304 | 294 | **10 lines** |
| MVDM overlay | 7 files / 266 lines | 7 files / 266 lines | 0 |
| `adapter-mvdm-host-out` physical inventory | 10,219 | 10,197 | **22 lines** |
| Project session code, displacement guard | 19 lines | 0 | **19 lines** |

The adapter result is a true autonomous fallback deletion. No code moved;
`opennt-host` metrics are unchanged.

## Follow-up

S3 will decide EOI/localfm/SAS/C-VID and pending-worker overlays as coherent
source-owner groups. Each remains until its complete original translation unit
can compose without duplicate state or broken ABI.
