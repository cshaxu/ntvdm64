# M0 T310 S16 P6 — BIOS/firmware final disposition

## Result

S16 closes `SPC-BIOS-FIRMWARE-BINDINGS` at the original source/binding
boundary for the selected Win32/x86 CCPU40 product profile.  It does not claim
that reset, BIOS dispatch, an interrupt, or a guest program has executed.

## Complete selected scope

The complete original `softpc.new/base/bios/sources` manifest has one
disposition for each of its fourteen selected source units in
[the S16 manifest ledger](m0-t310-s16-p3-bios-manifest-owner-ledger-001.tsv).
Every member remains an original body or original table:

- direct original bodies: `build_id.c`, plus source-preserved helper bodies;
- binding-only original BIOS bodies/tables: BOP dispatch, reset, CMOS/RTC,
  EMS, equipment, memory-size, virtual-device and error paths;
- named later owners: S17–S31 for controller, host-BOP, VDD and platform
  behavior, with S13/S14 already providing the declared SAS-memory boundary.

The non-manifest executable route is also fully disposed:

- `base/system/rom.c` and `cmos.c` retain original byte-loading, checksum/SAS
  publication and CMOS persistence flow;
- `host/src/nt_unix.c` retains original `host_find_file` with the registered
  app/session-root path binding;
- `host/src/nt_rez.c` retains original read/write/open/error order, with the
  selected writable retry confined to the resolved session firmware root;
- `host/src/nt_msscs.c` retains original NTIO load sequencing and the complete
  historical system-file body, while its selected first-session installer-only
  write side effect has the explicit no-host-boot-marker disposition.

No firmware path requires a mapping-manager identity: resource paths and
private host file operations are not guest-address or fixed-width handle
crossings.  ROM and CMOS bytes enter the machine only through the selected
original SAS calls already owned by the CCPU/SAS packets.

## Verification

The fresh `s16p5` formal graph compiled the selected complete source archive:

```text
ninja -C build/M0-T310/S16/original-softpc-ccpu40-s16p5 original-softpc-candidate
# 369/369 successful
```

The same graph's `/force:unresolved` ownership audit contains no unresolved
`host_find_file`, `host_read_resource`, `host_write_resource`, `read_rom`, or
CMOS resource symbol.  Its remaining unresolved forms are owned by named
later clusters (LIM/EMS, controller, video, storage, BaseVDM, BOP, debugger,
console and VDD paths); they are not a reason to introduce a BIOS shim.

## Deferred integration

The original reset and BIOS tables call controller contracts in their source
order.  S17 owns the first such system-controller callbacks; S18–S31 own the
subsequent host/controller/BOP families.  S49 alone may assert the integrated
`create → reset → firmware initialization → bounded execution → typed stop →
teardown` result.
