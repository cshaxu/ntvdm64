# T231 S9 — Direct OpenNT `cmdExitVDM` witness

The original `cmdexit.c` `54:00` body is directly admitted.  Its only
historical action is `TerminateVDM()`.  The existing typed COMMAND shim maps
that non-returning historical operation to `BX_NTVDM_CPU_RESULT_V2_STOP`; it
does not call `ExitProcess`, mutate the host, or resume guest execution.

`build/M0-T231-S9/direct-r1/t231-s9.exe` source-builds the admitted COMMAND
mirrors and neutral shims with MSVC x64 `/W4 /WX /MT`, then proves zero-exit
controlled stop, no resume RIP and no guest register delta.

This is local Direct source parity only; final COMMAND whole-package routing,
legacy removal and bounded native observation remain S10.
