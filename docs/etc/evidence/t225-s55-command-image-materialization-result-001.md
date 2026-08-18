# T225 S55 COMMAND image materialization evidence

Date: 2026-08-17
Status: passed provider-path regression; not native guest-execution proof.

## Question

The prior S55 static comparison found that the source-built COMMAND.COM evidence
image is 50,384 bytes (SHA-256
908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43), while
the native diagnostic later reported an instruction shape inconsistent with the
ordinary COM mapping hypothesis. This record tests the complete current
adapter-owned declared-image transfer path before attributing that mismatch to
a BOP provider.

## Source and ABI map

OpenNT's DOS loader owns the guest executable load and transfer. The relevant
adapter route is only the ordinary DEM namespace contract:

- 50:12: open the declared C:\COMMAND.COM namespace entry;
- 50:16: bounded sequential read to a guest DS:DX buffer;
- checked mechanical write action: the mantle-owned guest-RAM sink;
- 50:02: close the opaque namespace token.

The fixture deliberately does not emulate DOS EXEC, PSP/MCB setup, COM entry,
or child return. It uses a valid profile declaration and DTA registration
because those are current session prerequisites, then uses a deterministic
50,384-byte immutable image, with 1,024-byte maximum reads.

## Formal result

The fixture is registered as
t225-s55-command-image-materialization-fixture in
tools/build/t225-s7-full-module-manifest.json. A fresh x64 /MT,
CPU5/P-MMX Ninja graph was generated at
build/M0-T225-S55/image-integrity-r4; its target compiled and linked, and
the executable returned zero.

It independently runs Direct and Readonly. For every chunk it verifies the
successful 50:16 result count and byte-for-byte equality between the declared
source image and RAM at the requested physical destination, followed by a
successful 50:02.

## Conclusion

The current declared COMMAND.COM provider preserves a full 50,384-byte
image through open/read/write/close in both supported modes. It is therefore
not evidence for a truncation or transformation in that provider path.

This does not prove native COMMAND execution, the runtime CS:EIP-to-COM
mapping assumption, or the NTDOS EXEC/child/parent-return lifecycle. The
remaining S55 comparison must locate the actual native image materialization
and transfer state before selecting a repair; no new trace-led BOP work is
admitted by this result.
