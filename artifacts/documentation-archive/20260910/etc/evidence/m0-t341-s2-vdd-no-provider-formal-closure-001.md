# M0 T341 S2 — Original VDD no-provider formal closure

## Question

Can the selected product retain the complete original VDD service surface
without selecting an external VDD, sample DLL or kernel/monitor substitute?

## Inputs and procedure

1. Re-read the S1 complete VDD ledger and the original selected configuration
   branch in `nt_msscs.c::SetupInstallableVDD`.
2. Inspected the formal CPU40/x86 graph and final map for original
   `nt_vdd.c`, `nt_msscs.c`, the narrow SFT shadow boundary and VDD
   configuration binding.
3. Ran the fixed formal target outside the sandbox:

   ```text
   call build\M0-T337\S3\formal-x86\run-ninja-parallel.cmd original-softpc-process.exe
   ```

## Observation

Ninja entered the fixed formal CPU40/x86 directory and reported `no work to
do`.  The existing final map identifies `SetupInstallableVDD`,
`VDDInstallIOHook`, `VDDReserveIrqLine`, and
`mvdm_softpc_open_installable_vdd_registry` in the selected final link.

The retained original configuration flow treats only a missing former NT4
registry key as `MVDM_SOFTPC_VDD_CONFIGURATION_NONE`; it returns before the
original `SafeLoadLibrary` loop.  Any other registry result continues to the
original error direction.  Thus the current no-provider profile neither
creates a synthetic VDD nor converts arbitrary failures into a success.

## Result

The selected no-installable-VDD package is source and formal-build closed:

- original IO, IRQ, DMA, UMB-memory, user-hook, synchronous re-entry and
  DEM handle service bodies remain selected;
- no VDD sample, `ieuvddex`, kernel monitor, private CSRSS path or arbitrary
  host DLL is selected; and
- all VDD-visible guest physical numeric addresses retain the existing
  `MVDM-HOST-DIV-063` mapping boundary.

This is not evidence that a VDD DLL can load, nor that a hardware/device VDD
works.  Such a request requires its own complete original provider package
and machine/monitor admission.
