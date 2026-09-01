# M0 T355 S12 — CONFIG temporary-CDS IOCTL observation

## Scope

This record closes the one fixed-container observation of original
`tempcds` removable-media IOCTL traffic.  The observer records only the
existing scalar service contract at `demIOCTL`; it changes neither drive
projection nor IOCTL results.

## Source contract

Original `sysinit1.asm::tempcds` uses `INT 21h AX=4408h` while locating a
fixed drive for temporary CDS setup.  Original `ioctl.asm` routes this through
`SVC_DEMIOCTL`; `demIOCTL` dispatches subfunction 8 to
`demIoctlChangeable`, which returns `AX=1`, carry clear for a non-removable
fixed drive.

## Verification

- Formal CPU40/x86 Ninja rebuilt the original DEM IOCTL owner and linked the
  selected product.
- The focused scalar fixture passed.
- One immutable `O:\ntvdm` observation emitted three identical completed
  requests:

```text
MVDM-DEM-IOCTL phase=0 subfunction=08 drive=02 ax=4408 dx=0000 cf=0 state=copied
MVDM-DEM-IOCTL phase=1 subfunction=08 drive=02 ax=0001 dx=0000 cf=0 state=copied
```

The companion S10 EOF seek also remains successful.  No `demRead` record was
emitted in the same run.

## Conclusion

S12 is closed.  The original temporary-CDS fixed-drive query succeeds; no
host-drive or DEM IOCTL repair is selected.  The remaining interval is guest
control/data progression after successful `tempcds` calls and before the read
boundary.  Its next step must audit that original continuation rather than
add a filesystem/drive workaround.
