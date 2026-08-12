# M74 NT4 Boot Input, Display, and Clock Gating

Status: source-derived first-profile device classification, 2026-08-08.

## Scope

This record distinguishes guest data/vector construction from a live external
input, display, timer, RTC, or CMOS dependency. It is limited to the default
EN-US path and the first noninteractive `COMMAND.COM /C` target; it does not
claim that an interactive shell can omit those facilities.

## Sources

- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/msinit.asm`
- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm`
- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/msbio1.asm`
- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/spckbd.asm`
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msinit.asm`
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/time.asm`
- `src/opennt/base/mvdm/softpc.new/base/bios/bios.c`

## Startup Facts

| Area | Direct startup behavior | Classification |
| --- | --- | --- |
| Keyboard BDA state | `msinit.asm` reads BDA `40:96` bit 4 to choose ordinary versus extended INT 16 function numbers. | profile-owned initial BDA bytes; no host key event is consumed by this check |
| Keyboard resident layer | `sysinit1.asm` copies the SoftPC keyboard resident code into free guest memory, calls `InstSpcKbd`, and retains INT 09 state. | required historical guest layout/vector work; live keyboard input remains trace-gated for `/C` |
| Video linkage | startup stores the resident `int10h_vector` address and the INT 29 fast-output pointer, while the historical BIOS table binds BOP 10 to `video_io`. | vector/layout preparation is required; visible display output is profile/stream policy and trace-gated |
| Clock device | `msbio1.asm` declares `CLOCK$`; NTDOS's `msinit.asm` walks the device chain, calls device initialization, and stores the first clock device as `bclock`. | guest device-chain contract is required; it does not by itself demand a PIT/RTC/CMOS device |
| Time-of-day API | `spckbd.asm` has INT 1A support and `time.asm` uses INT 1A functions 00/01 for DOS time calls. BIOS BOP 1A maps to `time_of_day`. | a bounded monotonic/date-time capability may become necessary when reached; hardware timer source remains trace-gated |

The `sysinit1.asm` temporary INT 09/10/16 vector substitution and local-driver
validation are inside the `TAIWAN` conditional path. They are not evidence for
the fixed EN-US profile. The EN-US initialization still installs the historical
SoftPC keyboard resident layer, so a future initial-state plan must preserve
its necessary code/data/vector bytes rather than treating the vectors as
optional decoration.

## What a First `/C` Profile Must and Must Not Assume

1. It needs a valid IVT/BDA, the selected profile's keyboard/video resident
   bytes, and a guest `CLOCK$` device-chain initialization path. These are
   guest-image and entry-plan facts, not evidence to promote a PC/AT device
   family into core.
2. It must not claim interactive keyboard input, frame presentation, timer IRQ
   delivery, CMOS contents, or RTC programming until a trace reaches the
   relevant handler.
3. It needs an explicit wrapper stream policy for command output. Mapping DOS
   output to a contained stream does not require a host window or a historical
   INT 10 renderer, although a particular guest path may later invoke one.
4. If `INT 1A` is reached, the appropriate neutral requirement is a configured
   clock/time capability with deterministic test policy, not direct exposure
   of BOP 1A, RTC registers, or CMOS bytes through core.

## Core Boundary Result

No new core gap is admitted. Existing generic input/display/PIC/PIT foundations
remain candidates only when an independent consumer and an actual trace justify
their exact contract. The first wrapper-specific work is entry-plan data,
resident guest code, and contained stream policy; those stay above core.

The future trace should separately record first reach of: keyboard status/read,
video or INT 29 output, `CLOCK$` request entry, INT 1A, timer IRQ0, and CMOS/RTC
ports. A reference to the shared historical BIOS dispatch table is insufficient
for any of those promotions.
