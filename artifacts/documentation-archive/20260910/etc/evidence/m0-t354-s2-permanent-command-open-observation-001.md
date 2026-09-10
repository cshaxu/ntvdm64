# M0 T354 S2 — permanent COMMAND canonical-open observation

## Diagnostic contract

The only production change is the default-off `MVDM-HOST-DIV-177` recorder at
the original `demOpen` boundary.  It receives the original `DS:SI` pathname
location and scalar outcome, then obtains its own bounded, synchronous copied
string through the existing session mapping-manager lease.  It does not retain
a guest pointer, alter pathname bytes, alter `AX`/`CF`, replace a handle, or
change DEM/BOP dispatch.  The fixed observer supplies the report path only to
the child and restores its prior environment after launch.

## Formal build and immutable container

- Formal CPU40/x86 Ninja target `original-softpc-process.exe` linked cleanly;
  a subsequent verification invocation reported `ninja: no work to do`.
- The new fixed stage is `build/T354S2`; its media manifest SHA-256 is
  `ee907754cb7f2a42ae5e25475e52969d77af45854f99c5dc5bbe4a55a189b1de`.
- The staged product SHA-256 is
  `510c81d966476af3eeee2991d97aac64602803c530415c899311f8bb8297740a`.
- Exactly one console-owning, non-debug observation was made.  Its main report
  SHA-256 is
  `d022ea17bbd93fa52f9a7f7ee0bd1b3c3bf44d15bbf70ef990356b5f7c32dfc4`;
  its DEM-open report SHA-256 is
  `fd6e6d15bcbc83a5e148598a0e7877fef00080324da453885980a2981ec58cf0`.

## Observed original cohort

The bounded report contains two different original opens:

```text
MVDM-DEM-OPEN phase=0 ... path=C:\USERS\NEKO\APPDATA\LOCAL\TEMP\SCSCCC.TMP
MVDM-DEM-OPEN phase=1 ... path=C:\USERS\NEKO\APPDATA\LOCAL\TEMP\SCSCCC.TMP
MVDM-DEM-OPEN phase=0 ... path=C:\COMMAND.COM
MVDM-DEM-OPEN phase=2 ... status=00000002 ax=0002 cf=1 path=C:\COMMAND.COM
```

The first pair proves the observer does not merely report failures and that the
temporary transformed configuration file opens successfully.  The second pair
is the permanent command open: it is the original default `C:\COMMAND.COM`,
and its original `ERROR_FILE_NOT_FOUND` result (`AX=2`, `CF=1`) is what reaches
the existing `50:3D` failure terminal.  The accompanying BOP record preserves
the prior source order through `54:0C`, `50:12`, and then `50:3D`.

## Selected successor

S3 must audit the complete original configuration-to-`commnd` cohort:
`cmdGetConfigSys`/`cmdconf.c` transformation, `doconf` parsing, default shell
selection, and app-provided SystemRoot identity.  It must identify why the
source-generated configuration does not override the default `commnd` in this
CPU40 container.  It must not special-case `50:12`, redirect `demOpen`, or
write package media into the real host C drive.
