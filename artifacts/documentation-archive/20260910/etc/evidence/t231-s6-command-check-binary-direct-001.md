# T231 S6 OpenNT `cmdCheckBinary` Direct witness

The original OpenNT `cmdexec.c` `54:07` body is directly admitted.  Its DOS
bypass, host `GetBinaryType` path/error distinction, first-call guard, command
tail size failure, WOW compatibility test and 32-bit `/z` handoff remain in
the original function.

The historical CCPU/SAS dependencies are limited to a named binary shim:
checked DS:DX application, ES:BX parameter block and command-tail reads;
session-owned SCSINFO; OEM/ANSI conversion; and fixed-width return state.
The original SAS host-pointer arithmetic is inline-replaced by a checked guest
SCSINFO address calculation.  A mechanical CPU delta extension transports ES
and DS changes through the mantle outcome to Bochs' native segment loader; it
does not identify a BOP or OpenNT term in Bochs.

The MSVC x64 `/W4 /WX /MT` fixture in
`build/M0-T231-S6/check-binary-r2/` passed: it used the fixture executable as
a 32-bit host binary, verified `/z` SCSINFO construction, guest writeback and
typed DS/ES results, then verified the original `DontCheckDosBinaryType`
bypass.  It printed:

```text
T231 S6 direct OpenNT cmdCheckBinary handoff, session writeback, and DOS bypass verified
```

This is local source parity, not child-process lifecycle or native guest
continuity proof.
