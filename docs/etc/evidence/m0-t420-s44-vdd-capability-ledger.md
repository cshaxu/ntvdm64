# M0 T420 S44 VDD capability ledger

## Admission baseline

S44 audits the complete selected installable-VDD host package.  An absent
package-local `NTVDM.REG` VDD value is not a package exclusion: the original
service bodies are selected in `ntvdm.exe` and must be distinguished from the
configuration that chooses zero providers.  The product never accesses the
Windows system registry.

## Selected owner inventory

| Original owner | Responsibilities | Current selection evidence |
| --- | --- | --- |
| `mvdm/softpc.new/host/src/nt_msscs.c` | `SetupInstallableVDD`, VDD memory reservation, user hooks, worker termination and `VDDSimulate16` | `ntvdm.exe.map` resolves `SetupInstallableVDD`, `VDDInstallMemoryHook`, and `VDDInstallUserHook` from `original-softpc-host-roots` |
| `mvdm/softpc.new/host/src/nt_vdd.c` | MS-BOP slot allocation, I/O hooks, DMA, IRQ ownership | the same map resolves `VDDInstallIOHook` and `VDDRequestDMA` from `original-softpc-host-roots` |
| `mvdm/softpc.new/host/src/nt_bop.c` | BOP 58 registration, deregistration and dispatch | selected host root; `MS_bop_8` routes modes 0/1/2 to the three original ISV functions |
| `mvdm/dos/dem/demmisc.c`, `demsrch.c` | DOS process create/terminate notification | direct original calls to `VDDCreateUserHook` and `VDDTerminateUserHook` |
| `mvdm/softpc.new/host/src/nt_event.c` | VDM block/resume notification | direct original calls to `VDDBlockUserHook` and `VDDResumeUserHook` |

The original export list in `softpc.new/obj.vdm/obj/i386/ntvdm.def` declares
all listed VDD service names.  This is a real worker API surface, not a
compile-only archive.

## Current configuration and boundaries

`SetupInstallableVDD` retains its original `SafeLoadLibrary` loop.  The one
finite configuration binding loads immutable `NTVDM.REG` beside `ntvdm.exe`
at worker startup and selects the same historical
`HKLM\\SYSTEM\\CurrentControlSet\\Control\\VirtualDeviceDrivers` / `VDD`
`REG_MULTI_SZ` value from that shadow.  Absent shadow file selects zero
providers; malformed/present-but-unreadable shadow follows the original error
path.  The binding never opens the Windows registry and does not emulate a
provider.

`tests/mvdm-host/fixtures/S44-ENTRY-HOOK-NTVDM.REG` selects the controlled x86
`s44-entry-hook-vdd.dll`.  The real `O:\winnt` `run16 MEM.EXE` test loaded
that value through the original loop, registered its original user hook in
the VDD entry, and then recorded `CREATE`, `TERMINATE`, and `BLOCK`; `run16`
returned zero and original MEM output was present.  The separate BOP VDD
proves `VDDInstallUserHook(NULL, ...)` fails and that a controlled
`VDDInstallIOHook` allocation succeeds for a non-device port; neither result
is a mocked success.  The entry intentionally performs no file I/O or other
loader-lock-unsafe observation.  Test deployment removed `NTVDM.REG` and the
VDD DLL afterward.  The absence of a `DETACH` record is source-explained: an
installable VDD is loaded by `SafeLoadLibrary` and held for worker process
lifetime; original `SetupInstallableVDD` has no `FreeLibrary` loop.  Process
exit is therefore its normal provider lifetime boundary, not a DLL detach
callback contract.

The final same-shaped binding removes `RegQueryInfoKey` from the formal
`ntvdm.exe` import table: `SetupInstallableVDD` now calls its original
`RegQueryInfoKey`/`RegQueryValueEx`/`RegCloseKey` spelling, locally bound to
the shadow provider.  The remaining formal registry imports belong to other
selected owners and are not silently attributed to VDD: `cmdexec.c` (WOW
compatibility flags), `cmdkeyb.c` (keyboard layout), `demgset.c` (boot drive),
`dpmi32/vxd.c` (serial-port enumeration), and `softpc.new/host/src/config.c`
(WOW device/size profile); `nt_umb.c` additionally imports `NtOpenKey` for
hardware ROM description.  These are explicit receivers for the S45
aggregate shadow-registry audit: static configuration reads must bind to
`NTVDM.REG`, while serial/ROM hardware discovery needs a separate finite
provider rather than a fabricated `.reg` default.

`ClearInstanceDataMarking` is different.  Its sole selected call is original
`virtual.c::VxD_System_Exit`, but no function body exists in the examined
OpenNT, NT4.5 or NT5 sources; the historical ntvdmx64 tree has only an
annotated empty stub.  The current fail-closed provider terminates the bound
worker with `ERROR_CALL_NOT_IMPLEMENTED`.  It is neither recovered original
cleanup nor a passing VDD capability.  Do not replace it with a successful
no-op.  It remains a separately recorded VxD-instance-data boundary while
the installable-VDD provider path is tested.

## Lifecycle probe correction

`S44BOP.COM` invokes original BOP 58 registration, dispatch and deregistration
against a separate controlled DLL; the successful guest transcript is
`S44BOP OK`. Its provider trace records `BOP_INIT`, `BOP_DISPATCH`, `DETACH`
and `HOOK_REMOVE_OK`. An earlier `AX=3` result was only the test DLL's x86
stdcall export spelling mismatch, not a worker failure.

An early controlled provider wrote its `ATTACH` marker to a file from inside
`VDDInitialize`; this made startup stop before task creation. A no-op provider
and then the entry-hook-only provider both ran the same original configuration
path and completed `MEM`; the latter recorded `CREATE`, `TERMINATE`, and
`BLOCK`. The failure was therefore the test's loader-entry file-I/O probe,
not a VDD lifecycle or shadow-registry defect. The retained fixture performs
only original hook registration at entry and records from runtime callbacks.
IRQ/DMA/UMB calls still require a source-shaped runtime VDD caller; invoking
them in configuration entry is not valid lifecycle acceptance evidence.

### Verified production-path slice

The retained `verify-s44-vdd-shadow.ps1` test now waits for actual guest text
and callbacks rather than treating `run16` parent exit as success.  Its fresh
x86 result is `S44_VDD_SHADOW_END_TO_END_OK` with this ordered evidence:

1. `NTVDM.REG → SetupInstallableVDD → s44-entry-hook-vdd.dll` reaches the
   original configuration load and produces `CREATE`, `TERMINATE`, `BLOCK`
   while `MEM.EXE` prints its complete memory report.
2. `S44BOP.COM` reaches original BOP 58 register, dispatch and deregister;
   its independent provider records invalid-hook failure, valid user/I/O-hook
   installation, `BOP_INIT`, `BOP_DISPATCH`, and actual guest `IO_INB` plus
   `IO_OUTB` at port `0xE9`, followed by `DETACH` and hook removal.
3. The test removes all temporary `NTVDM.REG`, test DLL and guest-probe
   artifacts from `O:\winnt`; it leaves no product processes.

`verify-s44-vdd-worker-loss.ps1` supplies the dedicated failure/re-load
witness.  It starts original `COMMAND.COM` through the Console-owning observer,
writes the observed original COMMAND prompt snapshot, and then terminates only the
identified test `ntvdm.exe`.  The observer reports a bounded nonzero parent
outcome; the same `basesrv.exe` remains alive.  A subsequent `run16 MEM.EXE`
creates a fresh worker, reloads the controlled provider from `NTVDM.REG`, and
produces original MEM text plus `CREATE`, `TERMINATE`, and `BLOCK` callbacks.
The verifier finally removes its shadow file and test DLL and terminates only
the exact test package PIDs.  Redirecting `COMMAND.COM` to EOF or adding a
synthetic product lifecycle loop is not used as a substitute for this witness.

### Fresh final build and regression

`build/M0-T420/S44/shadow-registry-r2` is a fresh current-source x86 graph.
It links `run16.exe`, `basesrv.exe`, `ntvdm.exe`, `dtmgr.exe`, and both
controlled VDD providers.  Its deployed artifacts pass both VDD witnesses:
`S44_VDD_SHADOW_END_TO_END_OK` and
`S44_VDD_WORKER_LOSS_END_TO_END_OK`.

The same deployment passes all 17 established Console-text-gated product
routes in `s44-shadow-registry-r2-final3-summary.json`: direct and interactive
COMMAND, native stream and EOF routes, direct/repeated/three-depth nested MEM,
`COMMAND.COM /c`, guest and native exit-status routes, and EDIT return.  The
regression verifier now compares the build and runtime guest fixture bytewise
without relying on an ambient `Get-FileHash` cmdlet, normalizes physical
Console row wrapping before counting nested MEM reports, and deletes only its
four generated fixtures under `O:\winnt\tests` in `finally`.  These are test
transport corrections; no product or guest-media behavior changed.

The follow-up isolated x86 graph `build/M0-T420/S44/memory-api-r1` additionally
loads `s44-memory-vdd.dll` through the same original configuration path.  Its
real `MEM.EXE` run records `CREATE`, `MEMORY_CYCLE_OK`, `IRQ_CYCLE_OK`,
`DMA_CYCLE_OK`, `TERMINATE`, and `BLOCK`: original VDD UMB hook reserve,
commit/decommit/release; a free virtual IRQ reserve/release; DMA query and
writeback; and the original invalid-channel failure direction all completed.
This is a finite API-cycle witness, not a fabricated hardware interrupt or
external DMA-medium claim.

## Remaining surface disposition

The VDD DOS-handle family is not missing: `demfile.c` contains the original
`VDDAllocateDosHandle`, `VDDAssociateNtHandle`, `VDDReleaseDosHandle`, and
`VDDRetrieveNtHandle` bodies.  Its only selected higher-level consumers are
WOW32 `wkfileio.c` file thunks, so its end-to-end consumer witness belongs to
the queued WOW32 recovery, not a synthetic VDD replacement.

`VDDSimulate16` is also an original one-line `cpu_simulate()` entry, but its
contract requires a real 16-bit VDD stub to set CS:IP and execute
`VDDUnSimulate16`; no selected DOS profile supplies that stub.  It is a
source-proven no-consumer profile disposition, not a passing simulated-device
claim.  `VDDTerminateVDM` intentionally terminates the worker and will be
verified only by a separately isolated destructive worker-loss witness.

`ClearInstanceDataMarking` remains the explicit unavailable VxD instance-data
boundary described above: no OpenNT/NT4.5/NT5 body was found, and it must not
be converted into a successful no-op.

The isolated `s44-terminate-vdd.dll` provider reaches original
`VDDTerminateVDM` from a real DOS-task create callback; the trace is
`CREATE`, `TERMINATE_VDM_REQUEST`.  The worker exits and the broker remains
available.  In the standalone parent contract the corresponding `run16` exit
is zero, which is source-proven original behavior: `nt_term.c::host_terminate`
calls `ExitVDM(FALSE, 0)` for non-WOW workers and then `ExitProcess(0)`.
Thus VDD-initiated termination is a normal worker completion, not a missing
parent error propagation path and must not be changed by inventing a code.

## Required S44 evidence

1. Build a controlled x86 test VDD using the original exported worker API.
2. Prove original registry load, entry attach, BOP 58 register/dispatch/
   deregister, and the user-hook create/terminate/block/resume callbacks.
3. Prove an original I/O or memory request path plus invalid-request failure.
4. Prove normal DLL unload and worker/broker loss cleanup without stale hook
   state; retain `ClearInstanceDataMarking` as an explicit unavailable
   boundary unless a source-owned body is recovered.
5. Run fresh x86 linkage and all established 17 DOS routes after any product
   change.  Guest media remain immutable throughout.
