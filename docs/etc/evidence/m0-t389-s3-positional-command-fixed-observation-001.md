# M0 T389 S3 — positional `command.com` fixed observation

## Fixed invocation

One non-debug, Console-owning x86 observation used the staged immutable
package and the ordinary product positional spelling:

```text
build\tools\console-startup-observer.exe
  O:\ntvdm64\ntvdm.exe O:\ntvdm64
  O:\ntvdm64\observation-t389-s3-command-com-console.txt
  -f -o command.com --observe-console-input-ver-only
  --observation-timeout-ms 30000
```

The observer passed `command.com` as the first non-SoftPC positional token. It
did not use `--command`, synthesize a BaseVDM record, or write guest memory.
It owns a real Console and waits for the existing original keyboard-ready
marker before offering `ver`.

## Observed result

* The staged `O:\ntvdm64\ntvdm.exe` ran for the bounded 30 seconds and was
  terminated by the observer timeout (`0x53504354`), not by a guest fault.
* `MVDM-BASEVDM state=0104 available=1` followed by
  `state=0000 available=0` proves that the one DOS BaseVDM record was
  requested and consumed.
* The original configuration path copied `00` to guest linear address
  `914E6` (`MVDM-SAS-STORE ordinal=1 ... value=00000000`).  This is the
  observed `SCS_CMDPROMPT` query result: the staged `CONFIG.NT` contains only
  the commented `NTCMDPROMPT` documentation, and the original guest default
  remains the 16-bit `COMMAND.COM` shell-out disposition.  The observation
  therefore did not legitimately select the 32-bit COMSPEC branch.
* The original command path reached `54:01`, then original keyboard-layout
  (`54:0E`) and current-directory (`54:04`) service boundaries. The selected
  package also opened the immutable `system32\COMMAND.COM` more than once.
* No `MVDM-CPU-BIOS-WAITIO` / input-ready marker appeared. Consequently the
  observer did not inject `ver`; the Console buffer remained blank.

## Interpretation

This proves neither the second child shell nor its banner/prompt. It does
prove that the S2 literal `/C command.com` record reaches and passes the
original BaseVDM handoff, and that `SCS_CMDPROMPT` is not diverting the child
to COMSPEC; an app-level no-`/C` record exception is not the missing
capability. The next repair investigation must begin in the original
post-`54:01` COMMAND/SoftPC startup and child-EXEC chain before `DoReEnter`,
not by adding a host command reader or changing guest media.

## Exact retained reports

* `O:\ntvdm64\observation-t389-s3-command-com-console.txt`
* `O:\ntvdm64\observation-t389-s3-command-com-console.txt.base-vdm.txt`
* `O:\ntvdm64\observation-t389-s3-command-com-console.txt.bop-return.txt`
* `O:\ntvdm64\observation-t389-s3-command-com-console.txt.console-ready.txt`
