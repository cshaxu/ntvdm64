# M0 T198 S78: Minimum Reached ROM/IVT Provisioning Closure Map

## Question

What smallest machine-owned input is missing from the current CPU5 runtime
after the verified far jump from `073B:0939` to `C000:014A`, and can it be
supplied from the repository without turning `bx-vdm` into a BIOS, DOS, or
BOP implementation?

## Inputs

- The S74/S76 runtime record: real-mode far transfer bytes `EA 4A 01 00 C0`,
  followed by `#UD` at `C000:014A` with `window=15:ffffffff`.
- `src/bx-mantle/bx_ntvdm_minimal_machine.cc`, the current finite lifecycle.
- `src/bx-core/memory/misc_mem.cc`, especially
  `BX_MEM_C::init_memory_without_sim`.
- Historical OpenNT sources: `softpc.new/base/system/rom.c`,
  `softpc.new/base/inc/rommap.h`, `softpc.new/base/bios/reset.c`, and
  `dos/v86/doskrnl/bios/{spckbd.asm,sysinit1.asm}`.
- A repository input inventory using `rg --files src/opennt artifacts` for
  the historical `bios1.rom`, `bios2.rom`, `bios4.rom`, EGA and VGA ROM names.

## Procedure

1. Decode the captured generic-UD window without assigning BOP meaning.
2. Compare `C000:014A` (physical `0xC014A`) with the current core/mantle
   memory lifecycle.
3. Trace the historical ROM loader and IVT initializer separately from the
   DOS-resident keyboard image.
4. Inventory actual repository byte inputs instead of inferring them from
   historical source names.
5. Assign the smallest possible future input contract by architectural owner.

## Observations

### Current machine

`bx_ntvdm_minimal_machine_c::initialize` creates only a minimal SIM,
ordinary memory, an empty port space, CPU state, A20 and reset. It does not
load a BIOS, expansion ROM, initial IVT or a SoftPC image. The core allocates
ROM backing and initializes it to `FF`. Thus the 15-byte captured window at
`C000:014A` begins `FF FF FF FF` for the expected reason: the machine has no
declared byte supplier for that expansion-ROM address.

The generic-UD seam merely copies the current instruction window. It neither
recognizes BOPs nor owns ROM contents. `bx-vdm` correctly rejects this event
before ingress because it has no `C4 C4` prefix.

### Historical ownership

Historical SoftPC `rom_init` first fills the ROM region with bad opcodes, then
loads named resource payloads through `host_read_resource`: normally
`bios1.rom` at `BIOS_START` and `bios2.rom` at `BIOS2_START` (`bios4.rom` for
the CPU-40 configuration). Its preceding `read_video_rom` can load a VGA/EGA
resource at `EGA_ROM_START` (`0xC0000`), the region containing the reached
address. It finally marks populated ranges as ROM. Its `ROMMapInfo` contract
separately represents a read-only initial IVT plus a list of ROM mappings.

Historical `setup_ivt` writes a broad vector table before BIOS/device reset;
it assigns, among others, INT 15 to the historical cassette handler. That
routine is inseparable from the old SAS, device and BIOS composition, so it
is source evidence rather than an admissible direct modern call.

`spckbd.asm` is a different, DOS-resident image. `sysinit1.asm` copies its
`SpcKbdBeg..SpcKbdEnd` range below the normal device drivers and later writes
its INT 15 vector. It is not evidence that a byte at physical `0xC014A` is a
keyboard routine, and the current evidence does not source-map
`073B:0939` or `C000:014A` to an assembled source offset.

### Input availability

The approved repository inputs contain the historical source references but
no matching `bios1.rom`, `bios2.rom`, `bios4.rom`, EGA or VGA ROM payload in
`src/opennt` or `artifacts`. No current build manifest proves that those bytes
can be reproduced from the available sources. Therefore no source-backed
payload may be synthesized, guessed, or loaded for `0xC014A` in S78.

## Owner-indexed disposition

| Need | Current fact | Required owner | S78 disposition |
| --- | --- | --- | --- |
| ROM backing and fetch mechanics | Core maps initialized `FF` ROM backing | `bx-core` | Existing selector-blind mechanism; no change |
| Machine lifecycle/order | Mantle initializes memory before CPU reset | `bx-mantle` | Existing lifecycle lacks an image stage |
| ROM and initial-IVT byte identity | Historical product loaded external named resources | Declared machine-image input, applied by `bx-mantle` | Blocked: no approved byte payload/provenance |
| BOP decoding/provider selection | Only applies to canonical copied BOP bytes | `bx-vdm` | Explicitly excluded; no route is added |
| DOS-resident vectors | Guest image initialization owns its resident code | OpenNT guest/NTDOS image | Source relationship recorded; no address attribution claimed |

## Minimal Future Boundary

If and only if an owner supplies pinned, policy-approved machine bytes, the
next boundary is a **mantle-owned opaque machine-image provisioning request**.
It must contain only fixed-width descriptors: physical load address, byte
length, immutable source/hash identity, byte payload reference/copy and a
ROM-versus-writable initial-IVT mapping policy. The mantle validates ranges
and overlap, applies the descriptors before CPU reset, and keeps the core's
fetch/memory mechanics unchanged.

The descriptor carries no BOP selector, DOS service, BIOS function, device
policy or host callback. `bx-vdm` may participate only in explicit
composition/profile selection of an already-declared input; it may not inspect
or manufacture the firmware semantics. No interface is implemented in S78,
because the required payload and exact vector/image relation are absent.

## Interpretation And Confidence

High confidence: the reached `C000:014A` fault is a missing machine-image
provisioning condition, not a BOP classification failure. High confidence:
the current repository lacks the named historical ROM payloads. Medium
confidence: an opaque descriptor is the narrowest compatible future boundary;
its final field layout must wait for a valid approved input and a focused
mantle/core write-path audit.

## Follow-up

The next bounded investigation must determine whether a reproducible,
source-authorized ROM/IVT payload can be recovered or built under the source
policy. Until then retain the controlled non-BOP stop, do not fabricate a
single-address stub, and do not expand BOP providers or device models.
