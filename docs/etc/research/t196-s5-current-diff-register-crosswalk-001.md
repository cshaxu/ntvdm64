# M0 T196 S5 Current Diff/Register Crosswalk 001

## Inputs and method

The authoritative comparison is the zero-context Git diff between `src/bochs`
and the pinned local Bochs 2.6 tree at
`O:\repos.external\bochs-2.6-compat\bochs-2.6`. It reports 24 paths and 41
physical `@@` hunks. The table below accounts for every current hunk; S2's
60-hunk table remains historical baseline evidence only.

| Current hunk group (count) | Current path(s) | Disposition and register authority | Remaining proof |
| --- | --- | --- | --- |
| Minimal machine files (4) | `bx_ntvdm_minimal_machine.[ch]`, `bx_ntvdm_minimal_sim.[ch]` | Retain: BX-MACH-023/BX-MACH-026, source-boundary tested. | S6 MSVC minimal closure. |
| GCC lexical comparison (1) | `config.cc` | Retain: BX-BUILD-002. | S6 declared compiler proof. |
| Exception ABI header (1) | `cpu/bx_ntvdm_exception_intercept.h` | Retain: BX-UD-001 and ABI-004/005/006/015/018/019/021/022/037/041. | S6 compile/link. |
| CPU entry declarations (1) | `cpu/cpu.h` | Retain: BX-EXEC-016. | S6 compile/link. |
| Generic exception mechanics (3) | `cpu/exception.cc` | Retain only generic copied #UD/result/RAM/stop/startup mechanisms under the named default-off guards; observer/trace diagnostics are removed. | S6 compile/link. |
| Real-mode entry setter (1) | `cpu/init.cc` | Retain: BX-EXEC-016. | S6 compile/link. |
| FMT lexical compatibility (13) | `gui/paramtree.cc`, `gui/siminterface.cc`, `iodev/harddrv.cc`, `iodev/hdimage/hdimage.cc`, `iodev/pit.cc`, `iodev/usb/scsi_device.cc`, `iodev/virt_timer.cc` | Retain: BX-ABI-028; focused lexical test covers all 14 exact adjacencies. | S6 current MSVC proof. |
| Port object archive membership (2) | `iodev/Makefile`, `iodev/Makefile.in` | Retain: BX-BUILD-033. | S6 archive/link proof. |
| Removed full-device definitions (2) | `iodev/devices.cc` | Retain deletion: BX-IO-025/029/030 lifecycle is isolated in the minimal object. | S6 archive/link proof. |
| Machine/port private declarations (2) | `iodev/iodev.h` | Retain: BX-MACH-026/BX-IO-025. | S6 compile/link. |
| Isolated port lifecycle (1) | `iodev/minimal_port_space.cc` | Retain: BX-IO-025/029/030; source-boundary tested. | S6 archive/link proof. |
| Default-off execution/deferred plans (4) | `main.cc` | Retain: BX-EXEC-014/032/034/BX-ABI-042; one guarded post-hardware/pre-loop consumer. | S6 compile/link. |
| Machine friendship/RAM API (4) | `memory/memory.h` | Retain: BX-MACH-026/BX-MEM-007/010/020/024. | S6 compile/link. |
| No-SIM RAM and checked copies (2) | `memory/misc_mem.cc` | Retain: BX-MEM-007/010/020/024; source-boundary tested. | S6 compile/link. |

## Reconciliation result

There is no current unregistered Bochs source hunk. The former unconditional
Bochs BCrypt link is absent; no generated top-level Makefile hunk remains.
Every remaining hunk is either a registered selector-blind mechanical/build
surface or an exact lexical compatibility edit. The only unresolved evidence
is compilation/link proof on the declared MSVC toolchain island, already the
scope of S6; it is not permission for any further Bochs feature or runtime
expansion.
