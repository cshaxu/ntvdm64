# M0 T318 S2 P4 — Original resource and startup-contract recovery

## Question

Can the selected original `ntvdm.c -> nls_init -> host_main` entry proceed
through its first host startup requirements without replacing the SoftPC UI,
guest loader, CPU loop, or DOS lifecycle?

## Inputs

- Selected original `src/mvdm-host/softpc.new/obj.vdm/{ntvdm.c,resource.rc}`.
- Selected original x86 guest media: `NTIO.SYS`, `NTDOS.SYS`, and
  `COMMAND.COM`.
- Selected original SoftPC ROM/CMOS/profile media.
- Formal CPU40 graphs at `build/M0-T318/S2/cpu40-formal-r2` and its x64 row.

## Procedure

1. Stage only the immutable selected DOS and firmware inputs beside a fresh
   x86 product executable in disposable `runtime-r1`.
2. Run the original product entry with a 15-second external watchdog.
3. Trace the resulting `0xc000009a` to original
   `softpc.new/host/src/nt_nls.c::nls_init`, which calls `LoadString` for the
   original SoftPC string table and deliberately raises
   `STATUS_INSUFFICIENT_RESOURCES` when it is absent.
4. Select the existing original `obj.vdm/resource.rc` as a build input.  The
   generator compiles that unchanged resource source once per host-width row
   and links the resulting resource into `original-softpc-process.exe`.
5. Rebuild non-`/FORCE` x86 and x64 product links, then run a fresh staged
   x86 layout with no arguments and with the original `-f` startup flag.

## Observations

- The unresourced `runtime-r1` process exited with `0xc000009a`; Windows Error
  Reporting identifies `KERNELBASE.dll` as the raising module.  The only
  selected original direct raise of that status on this entry prefix is
  `nt_nls.c:120`.
- Both current x86 and x64 product rows compile the unchanged `resource.rc`
  and complete non-`/FORCE` `original-softpc-process.exe` links.  An archived
  x86 `resource.res` is deliberately not reused for x64 because LINK correctly
  rejects its x86 COFF machine type.
- With no argument, the new x86 product exits `0`.  This is the original
  `nt_reset.c::host_applInit` direct-invocation guard: it requires `-f`, and
  calls the existing session-scoped terminal route when that flag is absent.
- With `-f`, the fresh x86 process remained alive until the 15-second external
  watchdog ended only its owned process tree (`exitCode=1`).  It produced no
  stdout/stderr and no new application crash event.
- A separate three-second sample with `-f` found the process still alive with
  `141 ms` accumulated CPU time, `14,520,320` working-set bytes, and
  `4,980,736` private bytes before its owned tree was ended.

## Interpretation

The original resource and startup-argument contracts are now source/build
closed.  The result eliminates the first resource-induced exception and the
false interpretation of a no-argument direct launch as guest completion.

The `-f` run is only a bounded liveness observation.  Its silence and CPU
sample do not prove that `InitialiseDosEmulation`, NTIO, NTDOS `EXEC`, or an
ordinary child/parent return was reached.  No BOP, CPU, BIOS, DOS loader,
resource string, or host command behavior was authored or replaced.

## Follow-up

The remaining S2 work needs an owner-respecting observation boundary after
the original direct-launch guard, sufficient to distinguish original
configuration/reset, DOS-media load, and guest CPU execution without
introducing an app-owned lifecycle.
