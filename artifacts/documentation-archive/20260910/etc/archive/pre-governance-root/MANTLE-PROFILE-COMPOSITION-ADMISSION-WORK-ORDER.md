# Mantle Profile Composition Admission Work Order

Status: M12 contract closure, 2026-08-08.

## Result

The first mantle composition unit is a versioned, immutable **memory entry
transaction**. It combines only independently admitted inputs:

```text
selected constructed firmware snapshot (M11)
  + validated five-field initial-state descriptor (M9)
  + NTIO/NTDOS private preload transaction (M7)
  -> one preflighted ordinary-RAM memory transaction
  -> later generic core entry plan
```

It is not a `vm_session`, a BIOS builder, a DOS service dispatcher, or an
execution target. The unit owns copied scalar profile values and borrows only
the already-private image buffers held by M7 until an atomic core commit can
copy them.

## Admitted Memory Writes

For the fixed NT4 profile, composition may declare exactly seven disjoint
ordinary-RAM writes:

| Order | Destination | Bytes | Authority |
| --- | --- | --- | --- |
| 1 | `0000:0040` / `0x0040` | 4-byte INT 10h offset/segment | M9, exported by M11 |
| 2 | `0000:0054` / `0x0054` | 4-byte INT 15h offset/segment | M9, exported by M11 |
| 3 | `0000:0064` / `0x0064` | 4-byte INT 19h offset/segment | M9, exported by M11 |
| 4 | `0000:006c` / `0x006c` | 4-byte INT 1Bh offset/segment | M9, exported by M11 |
| 5 | `0040:0096` / `0x0496` | keyboard mode byte | M9, exported by M11 |
| 6 | `0070:0000` / `0x0700` | private NTIO preload | M5/M7 |
| 7 | `DI:0000` | private NTDOS preload | M6/M7 |

All vector pairs use little-endian offset followed by segment, exactly as the
IVT layout requires. M7 already rejects overlap between the last two entries;
M12 repeats complete-range and pairwise-overlap validation over all seven
widened intervals before publication. The low-memory scalar entries must also
be checked as ordinary RAM at later core commit time. A provider/ROM route is
always rejection, never a fallback.

The transaction preserves the M11 identity, revision, and opaque evidence
record alongside the copied M9 scalars. It also preserves the separate BYOB
image identity records selected by the external adapter, but does not retain
the image content after core has copied it.

## What M12 Does Not Select

The historical x86 source establishes that the host loads NTIO at `0x700` and
sets `CS:IP` to `0070:0000`. It does not, in the evidence currently admitted
to this route, establish the rest of the entry register/FLAGS state as a
profile contract. The existing core entry plan requires a full state record.

Therefore M12 deliberately does **not** create `core_machine_entry_plan`, set
`DS`/`ES`/`SS`/`SP`, choose general registers or EFLAGS, or apply the memory
transaction. Generic core's reset state is observable implementation behavior,
not permission for mantle to depend on undeclared values.

This leaves one explicit next gate: establish an NTIO entry-register baseline
from the original fixed source/build path or a bounded stopped-before-first-
instruction trace. Until then the output is a memory transaction with a
required-but-unfilled entry-state slot.

## Boundary And Sequence

1. The external adapter identifies and validates BYOB images, then supplies
   transient image bytes to M5/M6.
2. Mantle selects the named self-owned firmware provider, obtains an M11
   stopped snapshot, and validates it with M9.
3. Mantle composes the seven writes in a private transaction and verifies
   identity/revision compatibility before exposing it.
4. A future entry-state evidence gate supplies the missing complete CPU
   real-mode record.
5. Only then does mantle translate the transaction to generic core entry-plan
   preloads and call the one core atomic apply operation on a clean stopped
   machine.

Any validation failure before step 5 publishes nothing and writes no guest
memory. Core failure at step 5 must retain its own entry-plan atomicity
guarantee. No transition registration, BOP/SVC recognition, file capability,
device policy, or product control loop is part of this sequence.

## Existing Core Fit

Current core already has the relevant generic mechanism:

- `core_machine_entry_plan` has up to 16 nonempty, pairwise-nonoverlapping
  preload slots.
- `core_machine_apply_entry_plan` validates every preload as ordinary RAM
  before it writes any bytes.
- it validates `entry_physical == CS << 4 + IP` and constructs the CPU
  candidate before commit;
- it is permitted only once at a clean stopped boundary.

Seven declared writes fit the generic capacity without requesting a DOS-
specific exception. The only current blocker is missing evidence for the
complete `core_machine_entry_plan_state`, not memory transaction capacity or
mapping semantics.

## Required Future Fixture

Once the entry-state gate is complete, a default-disabled mantle fixture must
prove:

1. M11/M9/M7 inputs produce exactly seven ordered non-overlapping writes.
2. IVT encoding is offset then segment and BDA is at physical `0x496`.
3. overlap, unproven M9 input, firmware revision mismatch, or invalid image
   range leaves a caller sentinel transaction unchanged.
4. a real core entry-plan application leaves no partial write when any one
   destination is mapped by a provider.
5. no `vm_session`, firmware bytes, historical object, default CLI, or DOS
   service handler enters the fixture link graph.

## Completion

M12 closes the profile-composition shape and exposes the next evidence gap
without inventing a register contract. This is the intended mantle boundary:
firmware selection and versioned initial memory above core, with no NXVM
product session leaked to an external adapter.
