# Serial / print-screen device source disposition

The selected original BIOS dispatch maps BOP 14 to `rs232_io`; the helper
implements init, send, receive and status via the original 8250 controller
ports. Send waits for CTS and returns a timeout when absent; receive checks
LSR/data and has a separate timeout. Current worker link map SHA-256
`641E5C33430445ECC73885176250194BE936549654E14864074BD0B7D92D3CE2`
attributes `_rs232_io` to `original-softpc-comms:rs232_io.obj`.

Original versus current `rs232_io.c`, line endings normalized and
SequenceMatcher original-line accounting: 293/294 original lines match
(99.66%). This is **not** feature or restored-behavior coverage. DIV-325 adds
a declaration and `mvdm_softpc_report_setup_hardware_probe` on entry and on
both receive-success and normal function exits; it does not alter the original
port selection or send/receive branch source. The observer reads its optional
path once per thread, emits at most 256 ordinary entries and restores Win32
last error; when a path is supplied it opens/writes a trace file, so do not
claim the enabled probe is timing-neutral. The default-off
`tests/observation/softpc_observer_error_probe.c` checks preservation of
GetLastError at that disabled/null-stage boundary, not all real BIOS/serial
registers or the enabled failure path.

`docs/etc/evidence/m0-t420-s30-external-com-boundary.md` records historical
real COM3 open, timeout with CTS low, and controller-local TX/RX loopback;
its direct selected `nt_com.c` mock checks a success-side open/write/read/
close with test-only host boundary. Neither the loopback nor the mock proves
external physical transmit/receive, CTS-high peer or unplug/teardown.
`docs/states/TODO.md` retains the real peer requirement. Do not claim a new
external device failure or call real I/O passed. Remaining bounded work:
current package guest INT14 init/status/send/receive and error/lifecycle,
including trace enabled/disabled semantic comparison; external CTS-capable
peer, failure and cleanup remain separately pending. Keep `rs232_io.c` partial.

`prt_scrn.c` consists only of an original comment saying functionality moved
to pure Intel BIOS4 ROM. The selected build compiles `prt_scrn.obj`, whose
symbol table has no callable implementation; BOP 05 in `bios.c` maps to
FmDebugBop or illegal_bop depending configuration, not to this file. Source
and map prove an empty host translation unit, not that the deployed immutable
ROM implements a working INT05 print-screen path. Keep the source row pending
for ROM identity and behavior; no guest/ROM was modified or executed here.

Finally, retained source-tree `comms/obj/i386/com.obj` and `printer.obj` are
PE-i386 COFF with frozen SHA-256 respectively
`E4FCABF76BB9C6DBD19D29C797F69DABBBBAAF08A9B1A9E5B91E10D6EA1452C0` and
`6FB1B16F00321C34CB7DE93C73B9FC0FD39E9F4BAA10B1241DF29A5C71148204`.
Both exact paths are absent from current NTVDM, WOW32 and VDMREDIR Ninja target
input closures (raw-output hashes `D95E467D9E75C95737DEFE3ADEC70014681D371D76AEF134A828AFC3947DEC4C`,
`A1C4813C11EBD0640571CA4C388A91675C1E2E800440E8943FE6A2E4CAFBA276`,
`C653E688DFE76D508ABECC63B72CCA20B52E2E66901433FD91B970F59A337BD2`).
Exclude only those stale binary inputs, not their corresponding controller or
printer capability. No object or source was deleted.
