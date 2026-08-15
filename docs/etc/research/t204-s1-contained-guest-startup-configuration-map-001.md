# T204 S1 contained guest startup configuration map

## Scope and method

This map covers the declared source-built NTIO/NTDOS/COMMAND startup profile
from selected CLI inputs to configuration completion or the fatal terminal.
It is built from original OpenNT owner code and the current bound composition;
the `50:3D` engine witness validates one row but did not create the map.

## Complete declared-profile path

| Stage | Original owner and contract | Current bound composition | Classification and required action |
| --- | --- | --- | --- |
| Configuration selection | `softpc.new/host/src/nt_pif.c::GetPIFConfigFiles` selects PIF-specific values or the system `config.nt` / `autoexec.nt` paths. | The BYOB profile selects only fixed guest names and materialization values; it has no selected host configuration source. | Historical PIF policy is not a CLI dependency. Replace with an explicit, copied CLI startup-configuration input policy. |
| Configuration transformation | `dos/command/cmdconf.c::ExpandConfigFiles` reads the selected sources, expands `%SystemRoot%`, filters/rebuilds `country=` and `shell=`, gathers `SET`/`PATH`/`PROMPT`, and writes short-named temporary files. Failure displays a source-specific diagnostic then terminates the VDM. | No transform exists. `bx_ntvdm_readonly_namespace_v1` hard-codes `REM NTVDM64\r\n` and an empty autoexec image. | Replace this fixture-only synthetic namespace with a contained, session-owned configuration provider; do not recreate system-wide temporary files or mutate the host installation. |
| Guest path handoff | COMMAND `54:0C` / `cmdGetConfigSys` and `54:0D` / `cmdGetAutoexecBat` copy an OEM host temporary-file path to `DS:DX` (at least 64 bytes) and do not return on error. | `bx_ntvdm_cmd_boot_file_service` writes `C:\CONFIG.SYS` / `C:\AUTOEXEC.BAT` from the copied boot-input names and resumes. | Existing source-derived guest-path answer is not the original temporary-file contract. Migrate it under the configuration provider and retain only as an explicit contained/virtual profile mode. |
| Guest configuration consumption | `doskrnl/bios/sysinit1.asm::ProcessConfig` calls `54:0C` before `doconf`; guest DOS then opens and processes that returned path. The corresponding autoexec path is supplied through `54:0D` during command startup. | The boot namespace exposes the synthetic byte arrays through DEM readonly open/read/seek/close. | Guest owner. The provider must be a coherent configuration image and pathname capability, not an unrelated DEM file workaround. |
| Environment/bootstrap coupling | `cmdconf.c` constructs an autoexec environment merge block; COMMAND also owns `54:0F` initial environment, `54:02` COMSPEC and launch context. | COMMAND has a copied bounded host environment/context and separate boot-input paths, but no configuration transform or merge provenance. | Migrate the existing copied context into the later configuration package; do not re-read ambient environment from a BOP. |
| Configuration completion | `nt_bop.c::MS_bop_E` receives notification `5E` code zero, then calls `UMBNotify(0)` and `demDasdInit()`. Other notification values are ignored. | `bx_ntvdm_config_done_service` resumes only code zero; the top-level facade resumes unknown nonzero codes. | Split by owner: UMB/XMS machine capability and DEM raw-media/DASD capability. The zero-code no-op is a temporary fixture disposition, not completion. |
| Fatal startup terminal | `demdisp.c` maps `50:3D` to `demmisc.c::demExitVDM`, which shows the bad-`config.nt` diagnostic and calls `TerminateVDM()` without guest resume. | `bx_ntvdm_dem_cli_unavailable_provider_v1` returns one typed controlled STOP, with no RAM, register, flag or resume mutation. | Retain as the explicit non-GUI fatal terminal until a contained startup path is proven. It is not a normal DOS exit or a `54:0B` result. |
| Post-start cleanup | `cmdmisc.c::cmdGetNextCmd` calls `DeleteConfigFiles`; `cmdconf.c` deletes the original temporary files. | No temporary host file is created, so no equivalent cleanup exists. | The later contained provider must own session teardown of any copied/overlay configuration state; it must not delete host-selected source files. |

## Input and ownership boundary

The admissible replacement is neither host C: boot-file fabrication nor a
copy of the historic PIF/temporary-file implementation:

```text
CLI explicit configuration policy
  -> bx-vdm copied startup-configuration record
  -> session-owned contained configuration image / path capability
  -> COMMAND 54:0C, 54:0D, 54:0F and launch context
  -> guest NTIO/NTDOS/COMMAND startup
  -> zero-code 5E owner actions, or source-shaped fatal terminal
```

`bx-core` remains absent from this path.  `bx-mantle` may own only admitted
machine prerequisites for the UMB branch; it receives no BOP or configuration
meaning.  `bx-vdm` owns copied records and routing.  Original OpenNT remains
the semantic reference; normal user-mode APIs may later read explicit user
configuration sources under the selected profile, but no host installation,
system file, Registry or ambient process environment is modified.

## Current workaround ledger

| Current item | Present behavior | Unique action |
| --- | --- | --- |
| Profile `guest_boot_files` materialization | Requires exactly `minimal-comment-v1` and `empty-v1`. | replace in the startup-configuration provider; retain only as named deterministic fixture mode. |
| `readonly_namespace` boot bytes | Synthesizes `REM NTVDM64` / empty data and offers them as C: files. | migrate to a dedicated contained-image backend, then remove it from default direct-startup composition. |
| `command_boot_input` + `cmd_boot_file_service` | Returns fixed guest C: paths through `54:0C/0D`. | migrate under the same provider; preserve only its checked guest-RAM path-write mechanics. |
| copied COMMAND host context | Bounded environment, COMSPEC and CWD captured at admission. | retain and compose with the configuration record; do not recapture from BOP. |
| `config_done_service` zero-code resume | Suppresses original UMB/DASD actions. | replace with explicit owner dispositions after their package prerequisites are mapped. |
| `demExitVDM` typed STOP | Safely represents the fatal terminal without UI/process kill. | retain as fatal fallback until the package regression proves a nonfatal startup path. |

## Follow-on package order

The following are candidate S boundaries within T204, not active work:

1. Define a fixed-width, copied CLI startup-configuration policy and session
   lifetime, including direct/readonly/overlay/virtual profile disposition.
2. Recover the complete source-derived configuration transformation and
   contained image provider as one package, with original filter/error rules
   mapped before code.
3. Bind COMMAND `54:0C`, `54:0D`, `54:0F`, `54:02` and launch context to that
   one provider and regress both configuration and environment lifetime.
4. Admit or explicitly defer the `5E` zero-code UMB/DASD owner actions with
   their XMS/machine and raw-media package evidence.
5. Run one native integration observation only after the whole package
   regression passes; compare fatal and contained-success startup outcomes.

This ordering prevents a new `50:3D`, `54:0C`, or `5E` endpoint patch from
becoming an independent architecture.
