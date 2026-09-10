# R3 Direct-CCPU Initialization Closure Ledger

Status: R3.A source/link classification, 2026-08-07. This is a diagnostic
ledger, not permission to make the full historical closure link or execute.

## Captured Input

The source-isolated, default-disabled complete-table diagnostic is
`ntdos64-opennt-direct-ccpu-full-source-closure-fixture`. It uses the
historical inactive `host/src/sim32.c` role and the original DPMI32 mapper;
it does not use the SIM32 trace mapper. Its current log is
`artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/direct-ccpu-full-source-closure-dpmi-mapper-r3.log`
with SHA-256
`B06B65E96E1B0C86E67B4EBCA52EAFF1EA3120A5B7AB6135CDA89360AC5E3253`.

The log has 149 distinct `lld-link` undefined-symbol diagnostics and 296
source-reference occurrences. It is intentionally broader than the bootstrap
prefix. In particular, selecting the complete original `BIOS[]` table keeps
the original DPMI dispatch owner link-visible even though no R3 trace may
enter a DPMI BOP.

## Classification Rule

`InitialiseDosEmulation` gives the ordering reference:

```text
io_init -> reset -> SetupInstallableVDD -> scs_init -> NTIO entry
```

The modern first cut excludes `SetupInstallableVDD` and the historical system
directory/image loading code. Therefore an unresolved import falls into one
of these classes:

| Class | Meaning | R3 treatment |
| --- | --- | --- |
| I | A direct candidate to be reached while establishing CCPU/SAS, `io_init`, `reset`, or the pre-NTIO shared state. | Trace its original caller; source-close an original provider or admit a narrow host seam only at that caller. |
| P | Potentially needed only after the first real NTIO prefix or during the original NTDOS/COMMAND path. | Do not implement during initialization closure; retain the source owner for the next trace stop. |
| D | Full-table, optional hardware, protected-mode, UI, or product-shell dependency. | Keep linked only for diagnosis; explicitly unavailable in R3. |
| G | Missing generated CCPU input or insufficiently specified interface. | Recover additional input if possible; otherwise a local completion needs a separate, fail-closed contract and cannot be called "historical recovery." |

Static link reachability is not dynamic reachability. A Class I candidate is
not admitted until an original caller reaches it in the initialization trace.

## Aggregate Shape

Classifying reference occurrences by first historical component gives:

| Referencing component | Occurrences | Initial disposition |
| --- | ---: | --- |
| `base/video` | 106 | D: VGA register/video provider closure; not an initialization substitute. |
| `host/src` | 69 | Mixed: retain diagnostics and CCPU lifecycle candidates; old console/UI/product-shell calls are D. |
| `dos/dem` | 29 | P: NTDOS/namespace transition, after a real NTIO prefix. |
| `dpmi32` | 25 | D: link owner for full `BIOS[]`, runtime unavailable until separately admitted. |
| other support/configuration | 22 | Mixed: profile/resource calls are I candidates; remaining product policy is D. |
| `dos/command` | 21 | P: command smoke-test path, not bootstrap closure. |
| `base/ccpu386` | 11 | I/G: direct software-interpreter/SAS boundary. |
| `base/bios` + `base/system` | 9 | I: reset/ICA/ROM candidates; retain original device ownership. |
| `xms.486` | 4 | P: only after the original `scs_init -> XMSInit` caller reaches it. |

## Initialization Candidate Set

The first source-level closure review is deliberately limited to the following
families. It does not authorize their implementation as a group.

| Family | Symbols visible in the diagnostic | Class | Original reason and next evidence |
| --- | --- | --- | --- |
| CCPU private SAS lifecycle | `host_sas_init`, `host_sas_term` | I | `sas_init` is the original first caller; it must establish original SAS/ROM state before guest execution. Verify the full call order before binding checked private backing storage. |
| CCPU generated vector | `c_effective_addr`, `c_sas_touch`, `c_VirtualiseInstruction` | G | The first has a controlled real-mode fixture only. The latter two occur only in `sascdef.c` vector initialization and have no recovered semantics. They must remain fail-closed until their decision gates close. |
| CCPU interrupt/diagnostic hooks | `host_set_hw_int`, `host_clear_hw_int`, `host_get_jump_restart`, `host_get_q_calib_val`, `host_error`, `trace_file` | I | `reset.c`, `ica.c`, event/timer code and CCPU diagnostics reference them. Original ICA remains the interrupt owner; initial diagnostics may report/stop only. |
| Original I/O, ICA, ROM | `Ios_*`, `ica_*`, `dispatch_q_event`, `rom_init`, `copyROM` | I | These have historical source owners. The R3 target must retain those owners and stop at the first lower host dependency; no IVT/BDA, IRQ, or ROM-byte scaffold is allowed. |
| Profile/resource input | `config_inquire`, `host_read_resource` | I | `rom.c` requires explicit configuration/resources. A future contained provider may supply only prevalidated profile/resource inputs, never ambient search. |
| SAS backing/video/EMS/recursive bridge | `read_pointers`, `LIM_b_write`, `LIM_w_write`, `host_simulate` | I, gated | CCPU SAS can reach these; video and EMS must retain their original ownership. First determine whether the selected no-EMS/text profile avoids the paths. `host_simulate` needs a separately bounded CCPU-recursion contract. |

## Explicitly Deferred Closure

The following large families are not valid R3 work merely because the broad
diagnostic retains them:

- 42 video-register symbols and the graphics-render/display compatibility
  surface: D.
- old console input/output, cursor, display-mode, VDM console operation, PIF,
  process-shell, `ExitVDM`, and UI dialog calls: D. The direct CLI runner owns
  only its own session lifecycle.
- native namespace APIs referenced from DEM and command code, current-drive
  updates, command environment, and directory enumeration: P. They wait for a
  live original `DemInit`/`demLoadDos` or COMMAND caller.
- `XMSInit` allocation APIs: P. They wait for the original `scs_init` call
  path and are not a license to preinitialize XMS.
- DPMI register/TIB/fast-BOP support, all protected-mode dispatch, and
  `NtVdmControl`: D. The DPMI link role is retained but its runtime path is
  unavailable.

## R3.A Decision

The next executable engineering action is not an all-symbol shim pass. It is
to construct an initialization-only, default-disabled closure around original
SAS/`io_init`/`reset`, and to make it report the first Class I or G boundary
with historical caller identity. `scs_init` is reached only after that cut can
prove the preceding original owners ran. The first NTIO image copy remains
outside this task and is governed by the activation ledger.

This evidence does not change `ntvdm64`, mantle, or core source ownership.
