# M0 T310 S24 P2 — host-communications formal assembly

## Question

Do the selected original host-communications units assemble under Win32/x86
CCPU40, and can the public serial-open failure direction be checked without
touching a real host serial endpoint?

## Inputs and procedure

Generated a fresh graph at `build/M0-T310/S24/formal-host-comms-x86` using
`New-T310OriginalSoftpcNinja.ps1 -Architecture x86`.  Outside the sandbox,
ran Ninja for `original-softpc-host-roots.lib`, `original-softpc-comms.lib`
and `original-softpc-forced-closure.dll`.  Then built and ran
`tests/adapter-mvdm-host-out/win32/serial-endpoint-unavailable-fixture.c`.
The fixture calls public `CreateFileA` only for deliberately impossible
`\\\\.\\COM999`; it succeeds only when the open fails and closes an unexpected
handle before returning failure.

## Observations

- The fresh selected graph completed 369/369 edges.  It produced
  `original-softpc-host-roots.lib` (377,386 bytes),
  `original-softpc-comms.lib` (27,358 bytes), and the 3,014,144-byte forced
  ownership-audit image.
- The host-root archive includes the original selected `nt_com.obj`,
  `nt_wcom.obj`, `nt_lpt.obj` and `nt_ntfun.obj`; the communications archive
  contains all five original `base/comms` manifest members.
- The forced-link log contains no unresolved `host_com*`, `host_lpt*`,
  `host_print*`, `FastSetCommMask`, `FastWaitCommsOrCpuEvent`,
  `GetCommHandle` or `GetCommShadowMSR` symbol.  It deliberately retains
  unrelated later-owner unresolveds under `/force:unresolved`; the image is
  not claimed runnable.
- The safe impossible-name fixture compiled and exited zero.  It did not open
  an installed COM device, change a DCB, create an original RX worker, write
  bytes or retain a host handle.

## Interpretation and follow-up

S24 has formal source-assembly closure for the original public serial
endpoint.  The negative fixture confirms the selected public host-open
failure direction only; it is not evidence of live serial traffic.  WOW
handle callbacks remain S39, and the original LPT/MONITOR branches remain
exact condition-unselected paths.  Real device behavior joins the mandatory
S49 integration pass.
