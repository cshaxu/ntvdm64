# T231 S4 console and keyboard partial witness

## Question

Can the OpenNT `cmdInitConsole` (`54:09`) and the no-install failure branch
of `cmdGetKbdLayout` (`54:0E`) be source-admitted without importing the NT4
CCPU/SoftPC product shell?

## Procedure and result

`cmdmisc.c` retains the original console body. `cmdkeyb.c` retains the
original registry, KB16 and failure logic; only its unavailable include
closure is replaced by `command_misc_shim`. The old private
`GetConsoleKeyboardLayoutNameA` is supplied by a shim over public
`GetKeyboardLayoutNameA`. A local MSVC x64 `/MT` fixture ran the original
no-install path and reported zero exit:

```text
T231 S4 direct OpenNT console, keyboard fallback, and standard-handle token ABI verified
```

## Limit

This remains a partial S4 witness. `54:06 cmdGetStdHandle` is source-admitted
for its original non-pipe standard-handle branches: historical AX:BX
host-pointer input and BX:CX 32-bit handle output are transported as checked
session and handle tokens, and stdout is tested through an explicit `NUL`
handle. Pipe composition and the complete keyboard-layout success route remain
unadmitted, so S4 is open.
