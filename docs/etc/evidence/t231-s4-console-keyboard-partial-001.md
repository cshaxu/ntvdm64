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

This remains a partial S4 witness. `54:06 cmdGetStdHandle` and its complete
original `cmdredir.c` pipe helper lifecycle are source-admitted: historical
AX:BX host-pointer input and BX:CX 32-bit handle output are transported as
checked session and handle tokens; a focused fixture covers both a `NUL`
non-pipe stdout handle and an anonymous stdout pipe, including the original
temporary-file/output-thread transfer. The complete keyboard-layout success
route remains unadmitted, so S4 is open.
