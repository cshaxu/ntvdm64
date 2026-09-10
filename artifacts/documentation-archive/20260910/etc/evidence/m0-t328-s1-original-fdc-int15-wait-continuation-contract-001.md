# M0 T328 S1 — Original FDC/INT15 wait-continuation contract

Date: 2026-08-31

Status: closed static owner map; no missing FDC, ROM, vector or BOP binding
was found.

## Question

After T327 crossed the original COMMAND configuration package, does the
observed zero-address exception in the original FDC wait path identify an
unconnected ROM/vector/FDC/INT15 prerequisite that this package can recover?

## Exact attribution

The fixed short-root observation reported a loaded-image-relative return
offset of `0x4fba8`. The exact x86 map for
`build/M0-T319/S3/termination-x86/original-softpc-process.exe` places original
`floppy.c::wait_int` at preferred-image address `0x0044fb40`, i.e. relative
offset `0x4fb40`. The reported address is therefore inside that original
function, `0x68` bytes after its entry: the return position immediately after
its first recursive `host_simulate()` call.

The exception report's `address=0`, `ip=0` remains an observation, not proof
of which instruction or callback frame supplied the zero value.

## Original contract map

1. `softpc.new/base/disks/floppy.c::wait_int` saves AX/CS/IP, sets
   `AH=INT15_DEVICE_BUSY` and `AL=INT15_DEVICE_FLOPPY`, invokes
   `host_simulate()` at `int15_seg:int15_off`, restores the saved state, then
   invokes it again at `wait_int_seg:wait_int_off`. It reads `SEEK_STATUS`
   afterward and returns the original success or timeout result.
2. The selected non-`MONITOR` profile initializes those slots in
   `softpc.new/base/keymouse/keybd_io.c` to original ROM values
   `RCPU_INT15_SEGMENT:RCPU_INT15_OFFSET` and
   `RCPU_WAIT_INT_SEGMENT:RCPU_WAIT_INT_OFFSET` (`FE00:0CE0`). The NTIO KIO
   table replacement is confined to the original `MONITOR` branch, which this
   product does not select.
3. `softpc.new/roms/bios4.asm` places the wait body at `ORG 0CE0`. Both that
   body and the INT15 helper exit recursive CPU execution through the original
   `BOP BIOS_CPU_QUIT` (`FE`) endpoint.
4. FDC completion is original and layered: `fla.c` schedules
   `fla_int_call_back`, raises `ica_hw_interrupt(0, CPU_DISKETTE_INT, 1)`, and
   maintains `fdc_interrupt_pending`; `floppy_i.c::diskette_int` sets
   `SEEK_STATUS`, acknowledges the ICA and runs the original INT15-complete
   recursive simulation.
5. The selected CPU40 binding is original in shape:
   `nt_cprgs.c::load_sw_cpu_access_functions` publishes
   `host_simulate_func = c_cpu_simulate`; `c_cpu_init` calls
   `ccpu386InitThreadStuff`; `c_cpu_simulate` obtains a per-thread `setjmp`
   frame through `ccpu386SimulatePtr`; and BOP `FE` calls
   `c_cpu_unsimulate -> ccpu386Unsimulate` to return through that frame.

## Disposition

Every FDC, ICA, ROM vector, CPU entry and BOP-FE binding reached by this
contract is already selected from its original source owner. In particular,
there is no source basis to add a ROM loader, KIO vector install, FDC success
shortcut, BOP implementation, alternate executor or guest rewrite.

The remaining narrow owner is **CPU40 recursive frame integrity and guest
re-entry**. `ccpu386Unsimulate` assumes a positive per-thread simulation
level; its original base-level diagnostic does not return before decrementing
and `longjmp`-ing. The current observation is consistent with a broken or
exhausted nested return contract, but static evidence does not prove that this
specific branch was taken. It must be audited as the complete CCPU
entry/frame/unsimulate owner package, not papered over by a guard or an FDC
result.

## Verification

The unchanged formal selected-source products were verified after this
disposition:

```text
x86: ninja -C build/M0-T319/S3/termination-x86 -j 8 original-softpc-process.exe
x64: ninja -C build/M0-T319/S3/termination-x64 -j 8 original-softpc-process.exe
```

Both reported `no work to do`, preserving the already-linked formal product.
The x64 result is compile/link closure only; this record makes no x64 guest
runtime claim. No additional observation was run because this S made no
behavior change and its source-defined owner transfer is already established
by the fixed T327 container.
