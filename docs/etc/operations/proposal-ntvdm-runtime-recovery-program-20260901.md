# NTVDM runtime recovery program

## Objective and rules

Recover the original NTVDM design as a CLI-hosted CPU40/SoftPC product. Every
candidate maps its full original owner path, recovers only original bodies and
same-shaped bindings, runs package tests, and makes one frozen integration
observation. A link, BOP ingress or host-only fixture is never guest-execution
proof. CPU30 and Bochs are not runtime alternatives; guest media stays
immutable. CSRSS/kernel-VDM recreation is excluded: only bounded public
Win32/session equivalents retaining original caller results are admissible.

The ordered list is an **activation frontier**, not a fixed promise to test
every currently linked library before recovery. When the earliest workload is
blocked by a named original owner component missing from the formal graph, the
next T is immediately that component's complete adoption/binding/formal-link/
same-workload-runtime closure. Only then does the predecessor resume. This
prevents a known missing component from being deferred behind unrelated work.

## Ordered capability packages

1. **SoftPC conventional-device and DOS workload matrix:** T362 transfers the
   frozen `/C EXIT` workload at the exact original
   `c_IOVirtualised -> inb -> printer_io` owner.  Recover/test the complete
   CPU40 device path before resuming COMMAND runtime claims.
2. **DOS `.COM`/MZ `.EXE` execution and parent return:** original `$Exec`,
   file/JFN/SFT, PSP/arena/environment, relocation, child exit and COMMAND
   continuation using immutable low-dependency workloads.
3. **COMMAND native-child (`cmdExec32`) lifecycle:** reached `54:08/0A/0B`,
   public `CreateProcess` child lifecycle, original re-entry/event/return and
   explicit unavailable CSRSS-era branches.
4. **XMS, DPMI and DOS-extender execution:** original XMS/A20/UMB/INT15 and
   DPMI/DPMI32 runtime paths with mapping-manager identity boundaries.
5. **Redirector and host-resource execution:** local VDMREDIR first, then
   bounded public NetAPI; legacy NetBIOS/DLC/private-RPC remains explicit.
6. **WOW16 bootstrap, callback and Win16 NE lifecycle:** original WOWEXEC
   command route, task/callback/monitor, single-session UI and NE start/exit.
7. **WOW16 service, UI and host integration matrix:** selected WOW32 provider
   packages through public APIs, preserving private-shell failures.
8. **Cross-family execution and control-broker closure:** COMMAND/DOS/native
    child/WOW lifecycle, event and cleanup; public IPC only when reached.
9. **SoftPC graphics presentation and workload integration:** selected DOS
    graphics workload through original video and app presentation.
10. **Runtime matrix, x64 compatibility and product release:** selected DOS and
    Win16 matrix, x64 profile work, unpack-and-run manifests/release.
