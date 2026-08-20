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
T231 S4 direct OpenNT console, keyboard fallback/success, and standard-handle token ABI verified
```

## Limit

`54:06 cmdGetStdHandle` and its complete
original `cmdredir.c` pipe helper lifecycle are source-admitted: historical
AX:BX host-pointer input and BX:CX 32-bit handle output are transported as
checked session and handle tokens; a focused fixture covers both a `NUL`
non-pipe stdout handle and an anonymous stdout pipe, including the original
temporary-file/output-thread transfer. The `54:0E` fixture also proves the
original success branch through a process-local HKLM registry override and
disposable `KB16.COM`/`KEYBOARD.SYS` inputs: it returns `DX=1` and separately
writes both original guest output buffers. The fixture removes its override,
temporary key and files before exit. S4 is complete.
