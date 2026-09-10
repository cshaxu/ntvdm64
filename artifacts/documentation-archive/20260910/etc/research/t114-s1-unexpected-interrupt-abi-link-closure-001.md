# T114 S1 unexpected-interrupt ABI/link closure 001

## Exact unresolved ABI

The selected OpenNT headers and the fresh T113 x86 COFF object agree on
ordinary x86 C linkage (the symbols are `_inb`, `_outb`, `_c_sas_store`, not
stdcall-decorated names):

| Symbol | Original prototype | Actual use in `unexpected_int` | Bochs mechanical equivalent |
| --- | --- | --- | --- |
| `inb` | `void inb(IU16 port, IU8 *value)` | one-byte PIC ISR/IMR reads | `bx_devices_c::inp(Bit16u, 1)` |
| `outb` | `void outb(IU16 port, IU8 value)` | one-byte OCW3/mask/EOI writes | `bx_devices_c::outp(Bit16u, Bit32u, 1)` |
| `c_sas_store` | `void c_sas_store(IU32 address, IU8 value)` | one byte at BDA offset `6Bh` | checked Bochs ordinary-RAM byte write |

`ios.h` lines 465--466 defines the first two prototypes; `base_def.h` fixes
`io_addr=IU16`, `half_word=IU8`, `sys_addr=IU32`; the selected x86
`sas4gen.h` maps `sas_store` to `c_sas_store` at lines 285--286.

The original `ios.c` implementation itself dispatches through SoftPC I/O
tables. It must not be imported: a compatibility wrapper, if ever admitted,
would invoke Bochs's already registered port dispatcher instead. This preserves
the device owner while avoiding SoftPC I/O-model import.

## Feasibility and remaining boundary

A same-binary x86 C-linkage compatibility island is technically finite: three
wrappers, one current Bochs machine context, and the T113 object. The two
security-cookie imports are toolchain runtime support and require the future
object recipe to make an explicit `/GS` choice; they do not widen handler
semantics.

It is not yet an admissible implementation. The handler must be entered
synchronously after the generic #UD event, but the adapter result ABI has no
mechanical callback/continuation facility and must not become a PIC client.
Conversely, a selector-specific call in `exception.cc` would put SoftPC BOP
semantics into Bochs. The missing owner is a **machine-BOP composition
dispatcher**, separate from the adapter host-service plane.

T115 must decide that dispatcher placement before any wrapper, handler link or
Bochs exception registration is considered.
