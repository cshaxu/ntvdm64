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
T231 S4 direct OpenNT console and keyboard fallback: original no-install path verified
```

## Limit

This is only a partial S4 witness. `54:06 cmdGetStdHandle` remains unadmitted:
its historical AX:BX host-pointer input and BX:CX 32-bit handle output require
a session token ABI before the original source may be invoked. S4 is open.
