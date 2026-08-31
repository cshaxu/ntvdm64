# M0 T327 S1 — CPU40-to-guest execution prerequisite contract

## Result

The post-`demLoadDos` terminal is not a DEM, COMMAND or new BOP failure.  The
unchanged fixed x86 container reaches the original SoftPC floppy wait's first
recursive CPU transaction:

```text
NTIO.SYS
  -> 50:11 / DemDispatch(11h) / demLoadDos
  -> 50:3B, 50:0F, 50:1B, 54:05
  -> SoftPC base/disks/floppy.c::wait_int
  -> host_simulate
  -> CPU40 c_cpu_simulate
  -> address-zero access violation
```

This is the same source cohort previously frozen by T318/S2/P76, now
reconfirmed against the final T326 product.  It is not an invitation to add a
BOP provider or fake a floppy completion.

## Runtime attribution

T326's fixed `console-owning-nondebug` observation reported:

```text
MVDM-ORIGINAL-EXCEPTION code=0xc0000005 address=0x00000000
ip=0x00000000 return=0x00a8fba8 base=0x00a40000
```

The loaded-image-relative return address is `0x4fba8`.  The exact x86 product
map resolves that point to `original-softpc-disks:floppy.obj`, within
`floppy.c::wait_int` (`_wait_int` begins at preferred `0044fb40`).  The
returned-to instruction follows the source's first `host_simulate()` call.
The map establishes the active source caller, but intentionally does not
pretend that an address-zero report alone identifies the nested target.

## Original contract

`wait_int` saves AX/CS/IP, sets the original `INT15_DEVICE_BUSY` request,
loads `int15_seg:int15_off`, and calls `host_simulate()`.  It subsequently
loads `wait_int_seg:wait_int_off` and calls `host_simulate()` again before it
tests SoftPC's original `SEEK_STATUS` completion state.

The first host call is the original `nt_cpu.c::host_simulate`, which enters
CPU40 `c_cpu_simulate`.  CPU40 obtains a nested, per-thread simulation frame
through `ccpu386SimulatePtr`, performs `setjmp` on that frame, and enters the
same original CCPU execution body.  The frame provider is initialized by
`c_cpu_init -> ccpu386InitThreadStuff` for the main thread and is extended by
the original `host_CreateThread -> ccpu386newthread` lifecycle for qualifying
workers.

The vector values are not arbitrary host addresses: the original
`keybd_io.c::kb_setup_vectors` loads them from the NTIO keyboard/ROM table.
The complete reached cohort is consequently:

```text
ROM/NTIO keyboard-vector table
  -> kb_setup_vectors (int15/wait_int pairs)
  -> FDC/PIC completion state and SoftPC wait condition
  -> host_simulate / CPU40 nested simulation-frame transaction
  -> source-shaped return to floppy.c::wait_int
```

## Disposition

All selected bodies already belong to the imported original SoftPC mirror.
No missing BOP family, guest image, BaseSrv provider, synthetic device result,
or new executor is admitted.  The earliest recoverable owner is the complete
original recursive CPU40/FDC/INT15 transaction cohort.  S2 must first audit
the vector-table initialization and CCPU thread-frame lifecycle together,
then recover only the first missing prerequisite it proves.  If the first
remaining dependency is FDC/PIC behavior, it remains an original-machine
owner transfer; it must not be masked as a successful wait.

## Verification boundary

S1 is static attribution plus reuse of T326's immutable observation and map.
It makes no production behavior change and does not claim that NTDOS `EXEC`,
a DOS program, parent restoration, or `54:0B` has executed.
