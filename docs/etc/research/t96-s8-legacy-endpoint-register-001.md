# M0 T96 S8 Legacy Endpoint Register 001

## Rule

The following direct instruction-window recognizers predate the closed BOP
planes. This register freezes their scope for migration: no file may be added
to the list without a new task admission and original-source disposition.
Their presence is not provider admission. Each must be plane-gated or rejected
before S8 closes.

| Source module | Identity | Original owner / target plane | Current status |
| --- | --- | --- | --- |
| `dem_readonly_file_service` | 50:00,02,12,16 | DEM namespace | Freeze; source-derived candidate, plane gate pending. |
| `dem_boot_drive_service` | 50:0D | DEM GSET | Freeze; plane gate pending. |
| `dem_drive_service` | 50:0F | DEM GSET | Freeze; plane gate pending. |
| `dem_debug_service` | 50:3B | DEM misc | Freeze; plane gate pending. |
| `dem_hard_error_service` | 50:32 | DEM error/lock | Freeze; plane gate pending. |
| `dem_ioctl_changeable_service` | 50:21 / AL=08 | DEM raw-media | Freeze; source pair is incomplete for CLI provider. |
| `dem_dta_service` | 50:1B | DEM GSET | Freeze; plane gate pending. |
| `dem_dpb_service` | 50:46 | DEM GSET | Freeze; plane gate pending. |
| `vdd_create_user_notify_service` | 50:36 | DEM misc / VDD composition | Freeze; no VDD admission. |
| `cmd_get_next_service` | 54:01 | COMMAND launch | Freeze; S4 recorded full launch blocker. |
| `cmd_comspec_bootstrap_service` | 54:02,0F | COMMAND launch/environment | Freeze; S4 source reconciliation pending. |
| `cmd_set_info_service` | 54:05 | COMMAND launch | Freeze; no retained guest pointer admission. |
| `cmd_boot_file_service` | 54:0C,0D | COMMAND config | Freeze; paired source-derived candidate. |
| `cmd_current_dir_service` | 54:04 | COMMAND launch | Freeze; contained namespace CWD not yet admitted. |
| `cmd_keyboard_layout_service` | 54:0E | COMMAND console | Freeze; VDD/keyboard dependency. |
| `bios_memory_service` | BIOS form | Bochs memory | Reject as adapter BIOS provider; retain only evidence test. |
| `mouse_install1_mapping_service` | BIOS mouse form | Bochs mouse/input/video | Reject as adapter provider; retain only evidence test. |
| `wait_if_idle_service` | 5A | Top-level VDD idle | Freeze; CLI scheduler contract absent. |
| `spckbd_init_service` | 5F | Top-level keyboard/PIC | Reject as adapter provider. |
| `config_done_service` | 5E | Top-level notification | Freeze pending DEM/UMB/DASD prerequisites. |
| `emm_unavailable_service` | BIOS/EMM form | Bochs memory/device | Reject as adapter provider. |
| `printer_unavailable_service` | BIOS printer form | Bochs device | Reject as adapter provider. |
| `controlled_stop_service` | FE | Bochs CPU lifecycle | Freeze behind generic stop ABI only. |

## No-bypass scan

`Test-BopLegacyEndpointRegister.ps1` makes the register's source-module list
executable policy. It scans adapter C sources for a direct `bytes[2]` and
`bytes[3]` BOP recognizer. Such recognition is allowed only in common ingress,
catalogue/plane modules, or one of the frozen modules above. It rejects any
new direct recognizer and any missing registered frozen module.

The scan does not claim the frozen modules are correct providers. It prevents
unreviewed expansion while migration proceeds by plane component.
