# Adapter Atomic Multi-Write

## Purpose

Some source-closed guest services need several bounded writes and a checked
CPU result. `DemGetDPBList` is the first candidate. The existing v1 host
transaction deliberately carries exactly one write, so it must not be stretched
with implicit concatenation or DPB-specific branches.

## Admitted v1 contract

The adapter returns one fixed-size transaction containing up to 64 opaque
`runtime_guest_write` descriptors, one contiguous adapter-owned payload, the copied
boundary identity, and a v2 CPU result. The ABI contains no service ID,
pointer, Bochs object, segment or host handle. Every descriptor must have a
non-zero range, non-overlapping payload offset and no range overlap; a
separate declared payload size binds the complete payload.

Bochs receives only this opaque list plus the same-island payload. It must:

1. validate every descriptor and ordinary-RAM writability before a byte is
   modified;
2. copy all payload ranges in descriptor order only after complete preflight;
3. apply the already validated generic result only after every copy succeeds.

Any validation or copy failure follows native exception delivery and must not
expose a partial commit. The Bochs loop contains no service, selector, address,
payload-format, drive or DPB term.

### Existing-mechanics finding

`copy_to_ordinary_ram` already proves one entire writable span before copying,
but its preflight is private and it then mutates the span. Bochs currently has
the read-only counterpart `ordinary_ram_readable`, not a writable counterpart.
The smallest required external mechanics extension is therefore
`ordinary_ram_writable(addr,len)`: extract exactly the existing side-effect-free
write validation (A20, handlers, monitored pages and writable ordinary mapping)
without copying. The multi-write bridge first calls it for every opaque range,
then calls the unchanged `copy_to_ordinary_ram` loop. It must not weaken either
existing copy primitive or add a DPB-specific method.

## `DemGetDPBList` specialization outside Bochs

Adapter code alone selects inventory entries whose type is `REMOVABLE` or
`FIXED`, derives the ES:BP sequence, and prepares the source-written fields
only: `DriveNum`, `Unit`, and packed `Next`. It must preserve all other bytes
as the historical source does; therefore it requires a source-derived write
strategy that does not zero whole DPB records. Its successful generic result
replaces only BP with the first free offset and resumes at `RIP+4`.

The authoritative `demdasd.h` declaration is `#pragma pack(1)` and uses
32-bit `DWORD`/pointer fields. Thus `sizeof(DPB) == 33`, with `DriveNum` at
offset 0, `Unit` at offset 1, and `Next` at offset 25. A list entry requires
two disjoint writes (two bytes at `base+0`, four bytes at `base+25`) rather
than a synthetic 33-byte record. At most 26 admitted drives therefore produce
at most 52 descriptors and 156 payload bytes. The generic ABI therefore has a
64-descriptor maximum and a 4096-byte payload maximum. `add` derives the exact
payload upper bound from each descriptor; preflight requires the supplied
payload length to match it.

`session-local host context` treats this as a distinct pending-operation kind.
It binds the complete transaction and borrowed same-island payload to the
copied boundary, allows exactly one matching take, and clears it on either
take or mismatch. This remains adapter-only: no Bochs consumer is implemented
or implied by this lifecycle evidence.

## Gates before implementation

- Prove `sizeof(DPB)` and offsets against the exact OpenNT packed declaration.
- Define a no-partial-write preflight primitive for a descriptor array.
- Register a new external-intrusion exception before modifying Bochs.
- Add ABI negative tests: overlap, payload holes, range overflow, segment
  wrap, non-RAM target and failed later descriptor.
- Prove the r20 trace advances from `50:46` without claiming filesystem or
  interactive DOS support.
