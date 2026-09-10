# T198 S52: BIOS selector 15 observation

## Witness

The clean r1 finite NTIO run records the first unhandled selector-15 event as:

```text
bios15 observed=1 state=8dc8:0655 eax=000088a7 next=26
```

The bridge identifies it only when the instruction begins `C4 C4 15`; `26` is
recorded separately as the next byte.  Thus the actual BOP is the
three-byte BIOS selector `15`, at `8DC8:0655`, with `AH=88h`.

## Source correlation and disposition

OpenNT `sysinit1.asm` loads `AH=88h` and then issues `BOP 15h` to obtain the
extended-memory size.  The existing finite profile provider
`bx_ntvdm_bios_memory_service_v1_dispatch` accepts exactly selector 15 with
AH=88 in real mode and returns the profile's documented extended-memory KiB
value.  It does not activate the original SoftPC `cassette_io` BOP 15 handler.

The current composition already invokes this provider before generic BOP
ingress.  S52 therefore changes only its stale comment: selector 15/AH=88 is
now source-observed and admitted alongside selector 12.

## Next admission

Run the unchanged composition with that existing provider disposition and
record the next boundary after the typed three-byte resume.  No other BOP 15
subfunction, BIOS device, CMOS, cassette implementation, or host capability
is admitted.
