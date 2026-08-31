# M0 T318 S2 P9 — Real-console original startup liveness

## Question

After restoring the original CPU40 V7 VGA carrier, can the selected original
SoftPC product remain alive when supplied with the valid console handles that
its original host startup requires?

## Procedure

A disposable, build-only x86 Win32 launcher allocated a console, opened
`CONIN$` and `CONOUT$`, supplied those handles through `STARTF_USESTDHANDLES`,
then launched the isolated x86 product with original `-f -o
--ordinary-child`.  It used the Win32 debug event loop only to retain the
child process handle for a five-second bounded observation and to terminate
that owned test child after the bound. The tool is neither a product source
nor a formal build input.

The existing public-Win32 window observer inspected only that child during the
run.

## Observation

The launcher recorded `timeout 259`; `259` is Win32 `STILL_ACTIVE` after the
five-second observation bound. The observer found no child error dialog.
The launcher then terminated only its own isolated child process.

## Interpretation

This proves that the selected original CPU40/V7 VGA startup route is alive for
the bounded interval when its original console precondition is met. It also
confirms that P7's invalid-handle dialog was a pipe/headless observation
artifact, not a product startup blocker.

This remains liveness evidence only. It does not identify the current guest
instruction frontier and does not prove `NTIO.SYS`, NTDOS, COMMAND, EXEC, or
parent return.
