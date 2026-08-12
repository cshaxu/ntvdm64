# BOP `12h` memory-size contract: r20 source and machine evidence

## Scope

This record closes the input provenance for the first reached BIOS-style BOP
after the source-built NTIO prefix. It does not admit an implementation,
modify Bochs, or claim a runnable NTDOS/COMMAND profile.

## Original OpenNT owner

```text
base/mvdm/softpc.new/base/inc/bios.h: bop(0x12) -> BIOS[0x12]()
base/mvdm/softpc.new/base/bios/bios.c: BIOS[0x12] = memory_size
base/mvdm/softpc.new/base/bios/mem_size.c:
  memory_size: sas_loadw(MEMORY_VAR, &memory_size); setAX(memory_size)
```

`MEMORY_VAR` resolves through `BIOS_VAR_SEGMENT = 0x40` and offset `0x13` to
physical address `0x413`. The function only sets `AX`; it has no guest-memory
write and no stated FLAGS modification. NTIO reaches `C4 C4 12`, so a future
successful result resumes at `fault_rip + 3`.

## Native-machine evidence

The r20 custom target executed stock Bochs CPU5 firmware to the neutral
floppy boundary at `0000:7c00`. Its adapter-owned snapshot contains:

```json
{"id":14,"address":1043,"bytes_hex":"7f02"}
```

The little-endian value is `0x027f`, or 639 KiB. It is measured firmware
state, not an adapter default. The complete snapshot digest is
`7959a2d6d3ac5206` at
`artifacts/build/bochs-2.6-native-nogui-ucrt-r20-plan-cpu5/ntdos64-reset-fixture/byob/startup-snapshot-evidence.json`.

## Required next boundary (not implemented)

Use one generic typed CPU-result record, not a selector-specific Bochs
register write. It must be created by the adapter only after an exact real-mode
`C4 C4 12` boundary; authorize the source-derived low-16-bit `AX` replacement
and three-byte resume; preserve all other state; consume an admitted
adapter-owned machine-profile record; and reject a missing record, wrong
window/mode/profile, repeat, or teardown. Bochs may apply that validated
generic delta, but must know no selector, BDA address, DOS, or BIOS semantics.

Any required Bochs change must first be registered in
`adapter-external-intrusion-exceptions.md`.
