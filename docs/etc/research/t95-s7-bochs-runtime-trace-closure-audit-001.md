# T95 S7: Bochs runtime trace closure audit 001

Date: 2026-08-11
Status: build-closure audit; no build, link, executable, or guest run

## Purpose

Determine whether a prior S6 startup-transaction root can prove S7's reached COMMAND and DEM file operations, and if not, define the only admissible source closure for a new trace observation.

## Prior roots are not reusable

`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r30-cli-root-release/narrow-startup-transaction-build-root.json` is a frozen S6 bridge root. Its rejection list explicitly excludes `adapter_runtime`, `host_session`, generic guest-read, gather-read, observation, and the general multi-write service. Its 34 objects stop at `bx_ntvdm_cpu_result_bridge_v1.c`, whose source dispatch contains only then-admitted BIOS/boot-drive/debug/drive/error/set-info/DTA services. It has no readonly namespace, `DEMOPEN`, `DEMREAD`, bulk-result, or COMMAND boot-file service.

Therefore r30 cannot consume `50:12`, `50:16`, `54:0C`, or `54:0D`; it is negative closure evidence, not a candidate to rebuild or patch.

## Current source-owned bridge

1. `refs/bochs/cpu/exception.cc` has the registered, default-zero `BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT` branch (`BX-UD-001`), which invokes `bx_ntvdm_adapter_runtime_v4/v3/v2` and consumes only opaque pending write/multi-write/bulk-result records.
2. `BX-ABI-041` makes the one-range `50:16` bulk result a generic ordinary RAM preflight/copy. That Bochs code contains no selector, service, path, token, DOS, COMMAND, or OpenNT decision.
3. `BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER`, separately default-zero, observes every #UD BOP identity before dispatch. It remains observation-only.
4. `src/bx-ntvdm-adapter/bx_ntvdm_adapter_runtime.c` owns the selected S7 semantics: v3 copied path read for `50:12`, v2 seek/read/close, and the profile-owned `54:0C/0D` pathname multi-writes.

`BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE` and `BX_NTVDM_ENABLE_STARTUP_TRANSACTION` are a historical, narrower S6 route. They must be **zero** in this observation so a legacy handler cannot consume or mask a S7 boundary before the runtime does.

## Exact adapter source closure

The current CMake target `bx-ntvdm-adapter-runtime-test` supplies the source closure, excluding its test translation unit. A fresh Bochs trace root must copy and hash exactly these 43 adapter sources and three CLI sources, plus the current root adapter/CLI headers they include:

```text
bx_ntvdm_cpu_state_abi.c
bx_ntvdm_instruction_window_abi.c
bx_ntvdm_guest_range.c
bx_ntvdm_guest_write_abi.c
bx_ntvdm_multi_write_abi.c
bx_ntvdm_multi_write_transaction.c
bx_ntvdm_observation_transaction_abi.c
bx_ntvdm_startup_snapshot_abi.c
bx_ntvdm_startup_plan_abi.c
bx_ntvdm_startup_snapshot_evidence.c
bx_ntvdm_machine_profile_abi.c
bx_ntvdm_host_service_catalog.c
bx_ntvdm_host_transaction_abi.c
bx_ntvdm_session_resource_abi.c
bx_ntvdm_host_service_plane.c
bx_ntvdm_host_session.c
bx_ntvdm_bulk_result_transaction.c
bx_ntvdm_readonly_namespace.c
bx_ntvdm_dem_readonly_file_service.c
bx_ntvdm_exception_abi.c
bx_ntvdm_cpu_delta_abi.c
bx_ntvdm_cpu_result_v2.c
bx_ntvdm_guest_read_action_v1.c
bx_ntvdm_guest_gather_read_action_v1.c
bx_ntvdm_bop_catalog_v1.c
bx_ntvdm_adapter_runtime.c
bx_ntvdm_bios_memory_service.c
bx_ntvdm_dem_boot_drive_service.c
bx_ntvdm_dem_debug_service.c
bx_ntvdm_emm_unavailable_service.c
bx_ntvdm_mouse_install1_mapping_service.c
bx_ntvdm_printer_unavailable_service.c
bx_ntvdm_spckbd_init_service.c
bx_ntvdm_wait_if_idle_service.c
bx_ntvdm_dem_drive_service.c
bx_ntvdm_dem_ioctl_changeable_service.c
bx_ntvdm_dem_dta_service.c
bx_ntvdm_dem_hard_error_service.c
bx_ntvdm_cmd_boot_file_service.c
bx_ntvdm_cmd_current_dir_service.c
bx_ntvdm_cmd_set_info_service.c
bx_ntvdm_dem_dpb_service.c
bx_ntvdm_host_drive_policy.c
byob_profile.c
byob_identity.c
byob_image.c
```

The listed CMake target is the authoritative compile closure, not proof that all listed capabilities execute. In particular, drive/DTA/DPB/error services remain compiled adapter code and do not authorize their guest semantics beyond an actually reached BOP.

## Fresh-root observation contract

The next root, not yet generated, may use an intact native Bochs r4-derived container only as immutable machine provenance. It must replace and rebuild only current `main.cc` and `cpu/exception.cc`, compile the exact closure above under MSVC x86, and link through the existing original native-container archives. Its macro set is exactly:

```text
BX_NTVDM_ENABLE_EXECUTION_PLAN=1
BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=1
BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1
BX_NTVDM_ENABLE_STARTUP_TRANSACTION=0
BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=0
```

The root must pin the actual Bochs source/config/archive hashes and emit one manifest. It must not use a prior startup root as an input, modify the original r4 root, enable a device/plugin/firmware feature, or add an adapter or OpenNT source beyond the listed closure. A first compile/link failure is evidence and stops that observation; a successful link authorizes only one controlled trace with BOP listener logging and the admitted profile.

## Pinned startup-plan provenance

The required plan input is available as source-built evidence, not a fabricated entry: `artifacts/analysis/t95-s6-r3-real-ntio-startup-transaction-005-20260811-001/byob` contains NTIO (33,792 bytes, `CFC8…4937`), NTDOS (27,858 bytes, `9576…BC84`) and COMMAND (50,384 bytes, `908A…C43`) whose hashes still match that profile. Its `machine_startup_plan=true` and `machine_startup_entry=ntio-v0` are the same accepted plan declaration. The current runtime's `bx_ntvdm_adapter_runtime_v1_prepare_profile_startup_plan` deterministically uses that declaration to form the already tested real-mode entry `CS=0070:EIP=0000`, an NTIO payload write at `0x700`, and preservation range `0x714..0x717`.

The fresh root must generate a v2 profile from exactly those identities, retaining those two startup fields and adding only the S7 verified COMMAND/CONFIG/AUTOEXEC placement fields. It must reject absent or mismatched artifact evidence rather than silently selecting a different entry. This is a trace-input requirement, not a request for new Bochs machine semantics.
