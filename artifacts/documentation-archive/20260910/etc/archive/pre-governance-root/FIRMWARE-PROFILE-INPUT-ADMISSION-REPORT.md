# Firmware Profile Input Admission Report

Status: M10 read-only conclusion, 2026-08-08.

## Result

The current `ntvdm64` default PC/AT firmware is a lawful candidate source for
an initial-state profile because it is self-owned source, not an imported
third-party ROM. It must be consumed as a selected, constructed firmware
provider, not as a list of copied constants and not as an external ROM bundle.

This does not yet make it an admitted NTDOS profile. The current provider is
owned by `vm` composition and needs a generic mantle-facing export contract
before an external adapter can consume its result without inheriting NXVM
product/session behavior.

## Direct Evidence

| M9 field | Current default firmware evidence | Admission status |
| --- | --- | --- |
| incoming INT `10h` | `profile_firmware.c` registers the self-owned video service; `bios_load_interrupts` writes the completed vector after code assembly | candidate, runtime-built address must be exported |
| incoming INT `15h` | `profile_firmware.c` registers the self-owned misc service; `bios_load_interrupts` writes the completed vector | candidate, runtime-built address must be exported |
| incoming INT `19h` | `bios_load_interrupts` writes the default F000 code target when no service is registered | candidate only, actual post/reset policy must be selected and snapshot-tested |
| incoming INT `1Bh` | same all-vector initialization path provides the default F000 code target when no service is registered | candidate only, snapshot-tested value required |
| BDA `0040:0096` | `bios.h` defines `VBIOS_ADDR_KEYB_MODE_TYPE=0x0496`; `bios_load_data` writes `0x10` | source-locked candidate value `0x10` |

`bios_load_interrupts` first writes an IRET byte at the self-owned firmware
start, then materializes all 256 IVT entries from the completed service table.
The exact offsets for INT `10h` and INT `15h` therefore depend on the selected
service set and assembly/build order. Exporting a precomputed literal vector
would silently duplicate this policy and become stale.

## Why The Old Scaffold Remains Excluded

The OpenNT `fakeinit.c` scaffold is historical interim V86 product code and
contains its own unimplemented-interrupt instruction service. It is evidence
of historical needs but not an own firmware provider. The `ntvdm64` default
firmware, in contrast, is self-owned and subject to its existing source and
test policy. This distinction is essential: neither “a BIOS-like value” nor
“historical source exists” is sufficient admission evidence.

## Required M11 Contract

Introduce, in the future core/mantle workstream, a generic **constructed
firmware initial-state export**:

1. A selected self-owned profile constructs its ROM/BDA/IVT state through its
   normal provider ordering on an isolated machine instance.
2. At the stopped boundary, it exports only requested initial memory fields
   and their profile identity/revision/evidence record, not ROM bytes, host
   paths, session pointers, or product UI state.
3. Mantle validates the export against M9 and includes the selected fields in
   its frozen entry transaction.
4. The external NTDOS adapter may select this firmware profile explicitly;
   it may not assume NXVM's default profile or synthesize omitted vector
   values.

The exporter belongs above generic core memory mechanics. Its profile choice,
firmware ordering, BDA/IVT policy, and service set are mantle responsibilities.
Core retains only mapping, checked memory, freeze, reset, and entry-plan
commit. The future profile provider must remain usable by NXVM and external
adapters without importing either product's UI/session loop.

## Current Limit

No M9 descriptor is populated from this candidate yet. M10 establishes the
only currently lawful source candidate and the required construction/export
boundary. A bounded initial-state snapshot test is the next evidence gate.
