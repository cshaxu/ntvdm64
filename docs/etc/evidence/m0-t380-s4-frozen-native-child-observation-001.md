# M0 T380 S4 — Frozen COMMAND native-child observation

## Fixed request

One non-debug, console-owning observation was made with the existing frozen
CPU40/x86 formal product and the format-4 package staged at `O:\ntvdm64`.
The requested original COMMAND workload was:

`C:\Windows\System32\cmd.exe /c exit 37`

The observer validated the staged parent as `ntvdm.exe`, the immutable media,
and the staged original `VDMREDIR.DLL` companion before launch.  The parent
product SHA-256 was
`adbdfc3bf72fbf41798ab299ee64b489e35096b86727274552b4c30199a57e1f`.

## Durable result

The observer timed out after 8,000 ms and returned its controlled timeout
result `0x53504354`.  This is not an assertion that the requested child
started or that it returned exit code 37.

The durable BOP and DEM records establish the following original-owner
prefix:

- DEM opened `C:\WINDOWS\SYSTEM32\CMD.EXE` successfully.
- Guest COMMAND dispatched and returned original `54:07` (`cmdCheckBinary`)
  with `AX=3EFF`, `CF=0`.
- Guest COMMAND then dispatched original `54:08` (`cmdExec`).
- The last COMMAND marker is `MVDM-CMD-CALL svc=08 stage=0 ax=000E cf=0`.
  There is no `54:08` return, `54:0A`, `54:0B`, child-completion or exit-37
  marker before the fixed timeout.

The console record also shows the guest reached its COMMAND prompt.  This
observation therefore proves the selected native command reaches the original
classification and `cmdExec` entry boundary in the formal one-process product.
It does not prove the asynchronous original `cmdExec32`/`cmdCreateProcess`
completion path.

## Disposition

S4 is complete because its one permitted identity-proven observation has been
retained and classified.  The exact remaining runtime predecessor is the
original `54:08` stage-0-to-return interval, including its Base VDM re-entry
wait and child worker completion.  The trace does not authorize a leaf repair;
the complete original COMMAND native-child owner package remains the only
owner for any later recovery.

## Evidence files

- [observer report](../../../observation-t380-s4-native-child-frozen.txt)
- [BOP return trace](../../../observation-t380-s4-native-child-frozen.txt.bop-return.txt)
- [DEM open trace](../../../observation-t380-s4-native-child-frozen.txt.dem-open.txt)
- [console trace](../../../observation-t380-s4-native-child-frozen.txt.console.txt)
