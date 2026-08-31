# M0 T318 S2 P36 — `scs_init` Provider Cohort Audit

Date: 2026-08-31  
Status: static prerequisite audit; no guest-reachability claim

## Scope

This audit follows the fixed CPU40 startup route without changing the formal
observation container:

```text
ntvdm.c -> host_main -> config -> InitialiseDosEmulation -> scs_init
```

It covers the complete original `scs_init` cohort rather than treating a later
runtime symptom as a request for an individual BOP or observer change.

## Original order and selected ownership

`softpc.new/host/src/nt_msscs.c::scs_init` performs, in order:

1. `GetNextVDMCommand(NULL)` and, only for the first VDM, `AddSystemFiles()`.
2. `CMDInit(argc, argv)`.
3. `DemInit(argc, argv)`, whose false result calls the original
   `host_error(... ERR_QUIT ...)` and `TerminateVDM()`.
4. `XMSInit(argc, argv)`, with the same original terminating failure path.
5. `DBGInit(argc, argv)`, whose selected normal result is true.

`InitialiseDosEmulation` calls `reset()` and `SetupInstallableVDD()` before
this cohort.  `base/support/main.c` calls `config()` before
`InitialiseDosEmulation`, and only calls `host_start_cpu()` after it.  Thus no
member of this cohort can be treated as a post-CPU observation detail.

## Disposition by original provider

- **Base VDM / first-session record:** the selected
  `adapter-mvdm-host-out/basesrv/{base_vdm_client,base_vdm_local}.c` preserves
  the original `GetNextVDMCommand` call shape and supplies the declared
  `VDMINFO` record.  It is the unique selected Base-VDM provider; this audit
  found no separate original BaseSrv body in the formal source list.
- **`AddSystemFiles`:** the mirror retains the original first-session call
  order.  `MVDM_SOFTPC_NO_HOST_BOOT_FILE_MUTATION` selects the registered
  compatibility function instead of creating `C:\\IO.SYS` and
  `C:\\MSDOS.SYS` placeholders on a real host drive.  This is an explicit
  product policy divergence, not an accidental loader failure.
- **COMMAND:** original `dos/command/cmd.c::CMDInit` only reads
  `GetSystemDirectory`, retains the drive letter in `cmdHomeDirectory`, and
  returns true.  It has no direct error or private Console/CSR termination
  branch in this startup cohort.
- **DEM:** original `dos/dem/dem.c::DemInit` retains its public
  `SetErrorMode` and caller-owned root buffer.  Its sole selected divergence
  obtains that root through `mvdm_softpc_dos_copy_root`; false is reserved for
  allocation or root-copy failure and follows the original `scs_init`
  termination path.
- **Debugger:** the formal graph excludes `mvdm-host/dbg/dbg.c` and selects
  exactly `adapter-mvdm-host-out/debugger/source/dbg_init.c`.  The latter
  retains the `DBGInit(int,char**)` signature and original
  `NtQueryInformationProcess(ProcessDebugPort)` observation, resolving the
  public NTDLL export dynamically.  A missing export or failed query retains
  the original non-debugged/true outcome; it is not a private CSR dependency.
- **XMS:** `config.c` sets `xmsMemorySize` before `scs_init` and `XMSInit`
  commits free UMB then initializes the original SubAlloc allocator.  However
  the formal CPU40 common flags currently do **not** define
  `MVDM_XMS_SESSION_BACKEND`.  The selected x86 preprocessor path therefore
  enters the original `VdmAllocateVirtualMemory` branch instead of the already
  supplied `xmsCommitBlock` / `xmsDecommitBlock` / `xmsMoveMemory`
  session-mapping backend.  This is the one concrete corrective action from
  the cohort: select the existing mapping-manager backend in the formal graph
  and verify both width rows.  It is a build selection repair, not a new XMS
  provider.

## Console and worker conclusion

No reached cohort member directly calls `DisplayErrorTerm`, returns the old
console error `120`, or requires CSR/CSRSS transport.  Console/display setup
and the original heartbeat remain distinct earlier/later host-owner cohorts.
The fixed non-debug, console-owning container stays unchanged.  It must not be
run again until the XMS selection repair changes the product executable.

## Next action

Add only `MVDM_XMS_SESSION_BACKEND` to the formal CPU40 common compilation
flags.  Then use incremental Ninja to relink the affected XMS/product targets
and perform one unchanged fixed-container observation.  The result may
attribute or eliminate this prerequisite; it cannot by itself claim NTIO,
NTDOS, EXEC, or parent-return reachability.
