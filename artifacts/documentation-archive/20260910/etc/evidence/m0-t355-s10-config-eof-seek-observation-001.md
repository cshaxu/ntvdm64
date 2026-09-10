# M0 T355 S10 — CONFIG EOF-seek observation

## Scope

This record closes the single default-off observation of the original CONFIG
temporary-file EOF seek.  It records only already-live DOS scalar values at
`demChgFilePtr`; it neither changes a seek, handle, file, guest byte, parser
or CPU state.

## Original route

The original `sysconf.asm::doconf` invokes INT 21h `AH=42h`, `AL=2`, with
`CX:DX=0` after its temporary CONFIG open.  Original
`handle.asm::LSeekEOF` calls `SVC_DEMCHGFILEPTR`, whose selected provider is
`dos/dem/demhndl.c::demChgFilePtr`.  The provider retains the original public
`SetFilePointer` call and DOS `CF`/`DX:AX` result contract.

## Verification

- The formal CPU40/x86 Ninja target rebuilt and linked the selected product.
- The focused fixture verifies entry and success report records for the exact
  scalar ABI.
- One fixed `O:\ntvdm` observer run produced:

```text
MVDM-DEM-SEEK phase=0 requested=0000:0000 origin=02 result=0000:0000 ax=0000 cf=0 state=copied
MVDM-DEM-SEEK phase=1 requested=0000:0000 origin=02 result=0000:0A47 ax=0A47 cf=0 state=copied
```

The same run retained the successful temporary-file open and no DEM-read
record.  The EOF seek therefore succeeds and reports a source-consistent file
length of `0x0A47`; it is not the reason that `doconf` fails to reach read.

## Conclusion

S10 is closed.  No DEM `SetFilePointer` repair is selected.  The remaining
source-owned interval is wholly inside the original guest after the successful
EOF seek and before DOS read: `count/config_size`, `pararound`, `confbot`
relocation and `tempcds`.  Its next work must be a source audit before any
additional runtime observer is admitted.
