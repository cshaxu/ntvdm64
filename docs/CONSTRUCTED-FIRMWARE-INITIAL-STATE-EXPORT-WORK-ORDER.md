# Constructed Firmware Initial-State Export Work Order

Status: M11 contract closure, 2026-08-08.

## Purpose

This record defines the one-way boundary by which a selected self-owned
firmware provider may supply the five initial-state inputs already admitted by
M9.  It intentionally does not introduce an adapter, an external ROM, a new
runtime target, or a change to `ntvdm64`.

The current candidate is the owned default PC/AT firmware in `ntvdm64`.  Its
interrupt table is assembled dynamically, so the values must be observed from
one completed construction rather than copied from source-order constants.

## Normal Construction Evidence

The candidate's existing lifecycle is precise:

1. `vm_session_storage_initialize` creates the core machine and profile
   binding.
2. `vm_session_control_initialize` invokes
   `vm_session_provider_lifecycle_initialize` and freezes the execution
   provider registration.
3. That provider lifecycle initializes profile firmware, registers CMOS,
   keyboard, DMA, FDC, HDC, and core post services in its retained order.
4. `vm_session_create` calls `vm_session_control_reset`.
5. Its reset callback calls `core_machine_reset`; core first cold-resets the
   machine and then invokes the frozen provider reset callback.
6. `vm_session_provider_lifecycle_reset` resets devices and calls
   `vm_session_profile_firmware_reset`, which calls
   `vm_profile_default_bios_reset`.
7. `bios_load_data` materializes BDA data, and `bios_load_interrupts`
   materializes the IVT after the final service table is known.

The resulting core lifecycle is `STOPPED`, so existing checked physical-memory
reads are legal. No guest instruction, provider refresh, or presentation loop
may run before export.

## Export Input And Output

The future mantle-facing operation is conceptually:

```text
construct(selected self-owned firmware profile)
reset to stopped boundary
read exactly requested fields through checked core memory
attach identity/revision/evidence
return copied descriptor or failure
```

Its request names an explicit profile identity and revision, plus a fixed
field mask. For the first NT4 profile the mask is not extensible by accident:

| Field | Physical location | Output |
| --- | --- | --- |
| incoming INT 10h | `0000:0040` | offset/segment pair |
| incoming INT 15h | `0000:0054` | offset/segment pair |
| incoming INT 19h | `0000:0064` | offset/segment pair |
| incoming INT 1Bh | `0000:006c` | offset/segment pair |
| keyboard mode byte | `0040:0096` / physical `0000:0496` | byte |

The output contains only:

- selected provider identity and an immutable revision identifier;
- an opaque nonzero evidence record identifier;
- the requested copied scalar fields; and
- a stable result code naming a missing field, construction failure, profile
  mismatch, or stopped-boundary/read failure.

It does not contain ROM or RAM bytes beyond the requested fields, code
addresses other than the four requested vector values, raw `vm_session` or
core pointers, host/media paths, device instances, display/UI state, or a
callable firmware service table.

## Ownership And Atomicity

The exporter belongs in mantle/profile composition, above generic core.
It selects and constructs a provider; core continues to own memory mapping,
checked reads, topology freeze, and reset. The NTDOS adapter only selects a
provider and validates the copied output with
`nt4_mantle_initial_state_validate`; it cannot inspect or repair construction.

All requested fields are read into a private temporary record. Identity,
revision, lifecycle, requested locations, and every read must validate before
the operation publishes an output. Any failure leaves the caller's output
unchanged. A result is a snapshot, never a borrowed view: later reset, media
change, or provider destruction cannot change it.

The operation is permitted only on its newly constructed isolated instance at
a stopped boundary. It may not export a running or paused NXVM session, and it
may not reuse a product session as hidden state. That keeps an external
adapter independent of NXVM UI, command loop, and media policy.

## Evidence Class

For this candidate, the record is `source_locked` only when all of the
following are recorded together:

- provider identity and source revision;
- the construction ordering above;
- the exact requested location list;
- confirmation that each read used the checked `core_machine_memory_read`
  stopped-boundary route; and
- a fixture assertion for the captured descriptor.

There is no source-level literal assertion for the dynamically assigned INT
10h or INT 15h offsets. A fixture must capture their actual final pairs after
construction. BDA `0040:0096 = 0x10` has direct source evidence, but it is
still exported through the same snapshot so all five fields share one revision
and construction record.

## Required Fixture

When mantle becomes a buildable consumer, one default-disabled fixture must:

1. construct the named self-owned profile twice in fresh instances;
2. reset each instance without running guest code;
3. export all five fields from each stopped instance;
4. prove both descriptors have the requested identity/revision and valid
   evidence record;
5. prove BDA `0040:0096` is `0x10` for the selected owned profile;
6. prove every IVT pair equals an independent checked-memory read at the
   listed physical location;
7. prove same-revision fresh construction produces equal descriptors; and
8. prove failure of one requested field leaves a caller sentinel unchanged.

This fixture verifies construction result and boundary behavior, not that the
vectors implement a particular DOS service. It must remain outside the normal
`ntdos64-run` graph and must not import Microsoft guest bytes.

## Current Evidence And Gap

The existing `vm-default-pc-at-apply-smoke` executable passed on 2026-08-08,
proving that a normal default-PC/AT session constructs the expected FDC and
CMOS profile configuration. Source inspection additionally proves that reset
reaches the normal BIOS IVT/BDA materialization path before stopped memory
access is available.

No current public `ntvdm64` API exports this bounded descriptor. The present
`vm_session` API exposes a reset vector only, while its full composition type
is product-specific. This is the deliberate remaining gap: add a neutral
constructed-profile export at mantle time, rather than letting a wrapper
borrow `vm_session` or replicate BIOS assembly order.

## Completion

M11 is complete as a contract and source-evidence gate. It does not claim a
runtime exporter exists. The next implementation admission is a generic
mantle/profile composition boundary that can consume this contract without
linking NXVM product control or importing a firmware byte image.
