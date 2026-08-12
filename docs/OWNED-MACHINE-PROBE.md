# Owned Real-Mode Probe

## Purpose

`nvtdm --probe-real-mode target` is a deliberately narrow execution probe for
the owned machine path. It proves that a locally supplied COM or simple MZ
image can be placed in a private 1 MiB guest image, execute selected real-mode
instructions, emit bounded console output, and return a guest exit code.

It does not consume a BYOB runtime directory and must not be described as an
NTDOS boot path.

## Implemented Boundary

- COM placement at PSP segment `1000:0100`; MZ module placement after the
  256-byte PSP at `1010:0000`, with relocation delta based on that module
  segment. The MZ loader validates declared image page length and relocation
  table containment before loading, so a trailing overlay is not guest module
  data. Both paths initialize PSP `INT 20h` and an empty command tail.
- `MOV r16, imm16`, `MOV r8, imm8`, `NOP`, near and short relative jumps,
  carry-condition short jumps (`JC`/`JNC`), `HLT`, and `RET` termination.
- `INT 20h` termination.
- Limited `INT 21h`: character output (`AH=02h`), `$`-terminated output
  (`AH=09h`), version probe (`AH=30h`), process termination (`AH=4Ch`), and,
  only with an explicit rooted host capability, ASCII read-only open/read/close
  (`AH=3Dh/3Fh/3Eh`).
- A one-million-instruction budget and checked segment:offset translation.

The reported value for `AH=30h` is an owned probe compatibility value. It is
not a claim about any Microsoft DOS version or a contract for real applications.

## Deliberate Omissions

There is no IVT/BDA, PIC/PIT, BIOS interrupt implementation, PSP/environment,
DOS allocator, device model, protected-mode transition, BOP/SVC adapter,
redirection, or NTDOS binary loading. The file pilot does not implement DOS
write/seek/create, standard streams, wildcard enumeration, or OEM-code-page
path conversion. The file pilot sets carry plus a minimal DOS-style error code
for its own failures, but does not yet reproduce the full extended-error model.
Unsupported opcodes and services stop with an
error instead of falling back to host hooks.

## Verification

`tests/smoke-probe.ps1` creates a transient owned COM fixture containing only
the supported instructions. It asserts console output and the guest exit code
through CTest. No Microsoft byte or runtime file is used by that test.

## Relationship to Bootstrap Work

The probe is the lowest owned execution layer under phase 2 of the bootstrap
contract. Phase 3 onward remains blocked on evidence: a locally supplied,
version-locked BYOB runtime and a read-only trace that identifies the actual
bootstrap entry, memory layout, and service sequence.
