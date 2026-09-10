# Bochs Machine Composition Boundary

## Reconciled Decision

The final runtime must not present stock `bx_devices_c::init` as evidence that
every linked stock-PC device is required by the first NTVDM profile. It must,
however, retain the original Bochs native lifecycle: Bochs assembles,
initializes, resets and stops its own machine. The adapter supplies only a
versioned declarative profile input; it never assembles Bochs C++ objects.

This document records the source-derived profile boundary. It does not
authorize a project-owned composition hook or a product-shell extraction.

## Observed Lifecycle Order

`refs/bochs/main.cc:1118` performs, in order:

1. `bx_pc_system.initialize` and benchmark timer registration;
2. RAM allocation and system/optional ROM loading;
3. CPU object initialization, sanity checks and state registration;
4. `DEV_init_devices()`;
5. machine/device state registration;
6. hardware reset, GUI signal setup and timer start.

`DEV_init_devices()` expands to `bx_devices.init(BX_MEM(0))`. In
`refs/bochs/iodev/devices.cc`, this single call initializes virtual/slowdown
timers, loads the default device plugin set, registers port `92h`, writes CMOS
memory fields, registers a device timer and finalizes plugins. It is therefore
both a mechanics bootstrap and a stock-PC composition policy.

## Boundary

| Remains Bochs-owned mechanics | Adapter-owned declarative profile input |
| --- | --- |
| CPU construction/reset/exception loop | Selected CPU/profile facts accepted by original Bochs configuration |
| Physical RAM, ROM mapping, A20 semantics | Memory size and approved firmware/profile input |
| PC-system timer engine and controlled stop | `nogui` and original controlled-stop configuration |
| Native generic I/O and IRQ registration facilities | Existing optional-plugin denial declarations only |
| Ordinary-RAM commit primitive | No machine-composition control; guest-facing policy stays in the adapter service plane |

The adapter must not replace a Bochs device, CPU mechanism, RAM model, timer
implementation or port handler, and it must not select or connect Bochs
objects. It may provide only declarative profile data consumed by original
Bochs configuration/lifecycle code.

## Required Profile Properties

A future profile extension is admissible only if it:

1. is consumed by original Bochs configuration before native assembly;
2. gives the adapter no versioned ABI field containing a Bochs C++ object or
   host pointer;
3. has a default-off profile: without an installed composition, stock Bochs
   behavior remains intact;
4. makes no BOP, DOS, OpenNT, DEM, COMMAND, WOW or host-service decision;
5. uses one declarative composition request rather than a per-device patch;
6. has explicit rejection for an unavailable requested mechanism before reset;
7. preserves original Bochs initialization, state-registration, reset and
   shutdown ownership.

## Next Evidence Gate

Before implementation, derive the reset fixture's actual required endpoints
and trace which stock `DEV_init_devices` effects it consumes. The first
bare-reset experiment has already shown that reset callbacks can require PIC
even though the fixture guest performs no I/O: `speaker` reset reached
`PIC.lower_irq` and correctly failed at the unassembled PIC stub. Only the
complete traced closure may enter the first profile descriptor. Static link
membership is not endpoint evidence.

## Reset-Closure Admission Record

Every composition descriptor must list each selected mechanism in all three
phases: initialization, state registration, and reset. The r12 bare-reset
experiment establishes this rule. Although its ROM executed no port I/O, state
registration observed the default `unmapped`, `biosdev`, `speaker`,
`extfpuirq`, `parallel`, and `serial` plugin devices. During hardware reset,
`speaker` and `extfpuirq` route through `DEV_pic_lower_irq`; with no assembled
PIC that reaches the native PIC stub and stops the process before guest
execution.

Thus the first descriptor is not permitted to say merely “no guest devices.”
It must either retain the full traced plugin/reset closure or explicitly
prevent unselected default plugins from registering before reset. The latter
requires a single lifecycle-level composition mechanism and a fresh three-phase
trace; it may not be implemented as individual device suppressions.

## Existing Composition Gate: `plugin_ctrl`

The first follow-up trace identifies an existing Bochs configuration gate that
prevents optional default plugins from registering before reset:

```text
plugin_ctrl: unmapped=0, biosdev=0, speaker=0, extfpuirq=0, parallel=0, serial=0
```

This is an adapter-generated declarative composition input, not a Bochs source
hook and not six source-level device suppressions. It avoids the observed
`speaker`/`extfpuirq` reset-to-PIC-stub failure in the r12 fixture while
retaining the stock `DEV_init_devices()` core.

### Composition v0 (proven narrow claim)

| Class | Decision |
| --- | --- |
| Optional default plugins | Explicitly disabled by the one `plugin_ctrl` declaration above. |
| Stock hard-wired core | Still assembled by existing `DEV_init_devices()` (including its timer, CMOS, DMA, PIC, PIT, floppy, VGA and keyboard path). |
| Adapter contribution | Supplies the profile/configuration declaration; it does not initialize a Bochs object or implement a device. |
| Runtime evidence | r12 reset-to-stop fixture completed after applying this declaration, and logged the adapter transaction commit followed by controlled stop. |

This is **not** a claim that the remaining stock core is the final minimum
machine, nor that an NTDOS boot has run. It proves only that the first
composition descriptor can use an existing whole-policy gate before any new
Bochs lifecycle seam is proposed. Future reduction must classify each
remaining core endpoint through initialization, state-registration and reset
traces; it may not remove an individual device merely because the current ROM
does not issue a port I/O.
