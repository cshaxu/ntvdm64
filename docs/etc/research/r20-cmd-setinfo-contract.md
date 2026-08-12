# r20 `CmdSetInfo` Contract

`C4 C4 54 05` is `BOP_CMD` / `SVC_CMDSETINFO` (`bop.h`, `cmdsvc.h`). Its
historical dispatcher calls `cmdSetInfo` in `dos/command/cmdmisc.c`. That
routine only retains guest locations: DS:DX for packed `SCSINFO`, its
`SCS_ToSync` byte at source-derived offset 206, DS:BX for `SCS_Is_Dos_Binary`,
and DS:CX for `SCS_FDACCESS`. It has no output and no host operation.

The adapter records only these four physical locators and resumes at `RIP+4`.
It does not implement COMMAND execution, environment, console, filesystem or
SCS policy. Its focused C11 test passes. The r20 incremental fixture trace
proves `9346:749d -> next=74a1`; the next unhandled boundary is DOS `50:46`
at `9346:7588`.
