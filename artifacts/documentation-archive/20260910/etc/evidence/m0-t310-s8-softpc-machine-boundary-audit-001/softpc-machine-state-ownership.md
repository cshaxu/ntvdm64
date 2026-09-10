# Candidate state ownership

| State | Current evidence | Target owner | Boundary constraint |
| --- | --- | --- | --- |
| CPU/FPU register and executor state | `base/ccpu386`, CCPU `cpu_simulate` path | machine core | No external direct register globals; frame transactions only. |
| Guest physical memory/SAS | `base/ccpu386/ccpusas4.c`, generated SAS carrier | machine core | Physical copies and explicitly bounded external-memory attachment; no permanent host alias. |
| EMS physical aliases | adapter divergence 026 | machine core | Numeric physical-page alias only; no host-object identity. |
| PIC/PIT/DMA/CMOS/ROM device state | `base/system` manifest | machine core | Host clock/event and persistence passed through ports. |
| BIOS and firmware BOP bridge | `base/bios`, `base/system/rom.c` | machine profile/core boundary | Core exposes typed firmware trap; profile installs only permitted mechanical handlers. |
| Video/controller state | `base/video`, `host/src/nt_{cga,ega,vga,graph}.c` | split: controller core, presentation backend | Guest IO/VRAM state remains core-side; rendering/window state stays host-side. |
| Keyboard/mouse/device model | `base/keymouse`, host NT providers | split: device core, input backend | Input samples/events supplied by host; no WOW/console policy in core. |
| Disk/floppy/comms | `base/disks`, `base/comms`, host NT providers | split: controller core, block/serial/printer ports | Files/handles do not cross the core ABI. |
| Scheduler, TEB, ICA lock, threads | `host/src/nt_cpu.c`, `nt_reset.c` | host adapter/session | Core has no `NtCurrentTeb`, handle or host thread ownership. |
| DOS/DEM/COMMAND/WOW/VDD/BOP service state | MVDM host, `nt_bop.c` MS table | MVDM product host | It may consume traps/results; it never defines machine behavior. |
| Firmware/DOS media paths | adapter divergences 024/025 | session/app configuration | Machine sees immutable bytes or narrow read port, not path policy. |

## API shape to validate in a future design packet

```c
machine *machine_create(const machine_profile *, const machine_ports *);
machine_result machine_attach_memory(machine *, machine_memory_region);
machine_result machine_load_firmware(machine *, machine_blob);
machine_result machine_reset(machine *);
machine_run_result machine_run(machine *, machine_budget);
machine_result machine_destroy(machine *);
```

`machine_run_result` must carry a mechanical reason (budget, halt, firmware
trap, external interrupt boundary, fault, unavailable) and copied mechanical
context.  It must not carry a BOP selector interpretation, a DOS service
result, an unbounded native pointer, or a host handle.
