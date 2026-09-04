# M0 T391 S4 — No-argument COMMAND startup recovery

## Question

Determine why a bare `ntvdm64.exe` did not terminate in the fixed
console-owning, non-debug product container, and restore the smallest
source-shaped product behavior without modifying guest `COMMAND.COM`.

## Source decision

The previous app declaration published `COMMAND.COM` with only an empty
`CR/LF/NUL` tail and left the local BaseVDM broker non-terminal.  That is an
ordinary first-shell request in the historical NT4 product: after it has
consumed the record, CSRSS/BaseSrv may supply further commands.  The
standalone one-session CLI has no such producer, so the original COMMAND path
eventually waited indefinitely after running the default `AUTOEXEC.NT` route.

OpenNT's unchanged guest parser already owns the single-command contract:

- `mvdm-guest/dos/v86/cmd/command/init.asm` parses `/C` and sets `SingleCom`.
- `mvdm-guest/dos/v86/cmd/command/command2.asm` terminates the command path
  when that state is set.
- The explicit positional `COMMAND.COM` declaration remains a normal child
  shell, with its empty `CR/LF/NUL` command line and non-terminal broker
  disposition.

Therefore the app now publishes `/C\r\n\0` only for a bare invocation and
uses the pre-existing local BaseVDM command-exhaustion terminal disposition.
It neither parses an empty DOS command nor writes guest memory.  This is the
registered `APP-DIV-017` product-boundary divergence: it replaces only the
missing CSRSS/BaseSrv producer with a one-shot terminal disposition.

## Verification

Focused binding regression, rebuilt from the formal CPU40/x86 Ninja graph:

```text
cmd.exe /c "call build\M0-T302\S2\x86\msvc-x86.cmd && ninja -C build\M0-T302\S2\x86 test -v"
PASS: local Base VDM broker contract
```

Formal rebuilt product:

```text
build/M0-T389/S6/formal-x86-r2/original-softpc-process.exe
```

Fixed product observation:

```text
node tools/observation/ObserveSoftpcStartup.mjs \
  --launcher build/M0-T390/S5/observer/console-startup-observer.exe \
  --product build/M0-T389/S6/formal-x86-r2/original-softpc-process.exe \
  --stage O:\ntvdm64 \
  --report build/M0-T391/S4/observer/no-argument-fixed.txt \
  --observation-timeout-ms 15000
```

The report records `container=console-owning-nondebug`, `result=exited`, and
`exit=0x00000000` before the 15-second deadline.  Its BOP sidecar records the
normal original startup chain, including `54:05`, `54:01`, and COMMAND
services; no app-side direct process launch marker is involved.

The verified formal binary has SHA-256
`a2eb75c5856a40489081a28369e072ab1f32bb76661f15128a716b9de50dffbc` and was
published unchanged as `build/output/ntvdm64-0245.exe` plus
`O:\ntvdm64\ntvdm.exe` and `O:\ntvdm64\ntvdm64.exe`.

## Outcome

S4 is closed.  Bare invocation is now a source-shaped empty `/C` COMMAND
transaction which terminates instead of hanging.  The distinct interactive
`ntvdm64.exe command.com` product path remains untouched.  T391 next resumes
its original S5 non-zero native-child lifecycle proof, followed by the frozen
S6 observation.
