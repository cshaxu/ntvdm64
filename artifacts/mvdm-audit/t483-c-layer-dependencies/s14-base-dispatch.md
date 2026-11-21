# T483 S14 P153 BASE client dispatch

The owner approved using the existing C request layouts. The boundary gate
now admits only the 13 enumerated C header files for base_dispatch.c.
Include-search directories permit locating those files; compiler dependency
records still reject other C headers. Other B source files receive no new
permission from this entry.

The final published package also passes eight malformed-size requests in the
existing real admitted-client fixture. Each returns unsuccessful status and
preserves the entire original union payload. The same process then completes
normal admission and lifecycle checks. All three runs passed, covering
admitted-vdm, frontend-io-fault and worker-console-exit. Evidence uses the
size-check report prefix under build/t483-s14-base-dispatch-final.

Run16 x64 and NTVDM x86 resolve NtvdmBaseClientCallServer to base_dispatch.obj in compat. NTCON has no execution dispatcher. The original C callers retain the CSR spelling and calling convention. No C mirror file changed.

B selects all eight BASE operations, validates the original request sizes, writes the CSR return status, retains the worker's local ConsoleRecord wait across deliveries and transfers it on ExitVDM. B also owns the command-delivery observation and activation decision. D registers one immutable transport table when an execution client authenticates. Disconnect clears it during the existing joined-caller cleanup. Ordinary NTCON and NTVWM clients retain the core connection entry.

The table has eight typed operations and a connection status query. D owns authenticated RPC, connection failure, wire encoding and receipt translation. Check/Get/Update still require D to marshal typed stream and event attachments; Exit and ExitCode still consult D's broker receipts and WOW registration state. The direct C-to-D symbol edge is gone. These runtime callbacks are visible and must remain in the implementation audit. C0619 and C0621 have independent F providers.

The compiler dependency records for x86 NTVDM, x64 Run16 and reference x86 agree on the same 13 C headers. Original basemsg.h supplies the request numbers, union members and message lengths; basevdm.h and vdmapi.h supply VDM values. The remaining selected declarations are conroute.h, winss.h, devioctl.h, nt.h, ntcsrmsg.h, ntlpcapi.h, ntpsapi.h, ntregapi.h, ntrtl.h and nturtl.h. The owner approved the file-scoped ABI admission, which is now enforced by exact header paths.

The P152 reply control failed before its first request with Windows error 13. The isolated fixture lacked the staged package identity. Stamping the fixture copy with the manifest identity made the control pass. In P153 a separate connect case completed authentication but its subsequent First query failed because the fixture had reconnected with the core-only entry. Using the execution entry covers registration on reconnect.

Final package dd1cb2fd-76c6-4b03-86db-6b6b6dee8f29. Cached x86, x64 and reference builds passed. Both 11-image release manifests were verified against their destinations. Final-package evidence under build/t483-s14-base-dispatch-final includes five successful wait contracts (reply, connect, disconnect, loss, bootstrap), admitted VDM, frontend I/O failure, worker Console exit, native EDIT return through COMMAND and the eight-phase WOW dialog guest. These are functional and lifecycle tests, not an individual successful RPC trace of all eight operations.
