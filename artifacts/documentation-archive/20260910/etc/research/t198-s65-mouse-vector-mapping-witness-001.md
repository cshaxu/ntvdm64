# M0 T198 S65: Headless `BOP C8` Mouse-Vector Mapping Witness

## Result

The C8 provider now executes as a finite two-phase composition transaction:

1. it performs a checked ordinary guest-RAM read of the four bytes at the
   source-observed `CS:BX` `mio_table` address;
2. it uses the existing provider to form the original vector mapping; and
3. it performs the matching checked ordinary guest-RAM write to IVT INT 33 at
   physical address `0xCC`, then resumes after three BOP bytes.

This does not implement a mouse.  The guest retains ownership of the copied
vector and the following INT 33 handler installation.

## Clean source-built witness

The MSVC x64 `/MT` clean run at
`artifacts/build/t198-s65-mouse-vector-mapping-r1` advances past C8 to:

```text
next=17:5a terminal=1:0203:02d7
```

`17` is a top-level BOP selector and `5A` is following guest code, not a BOP
minor service.

## Next frontier

The original SoftPC BIOS table names BOP 17 `printer_io`.  The current bx-vdm
tree has a dedicated unavailable provider.  The next task must audit the
actual request and the source-defined printer-unavailable continuation before
any behavior is admitted.

