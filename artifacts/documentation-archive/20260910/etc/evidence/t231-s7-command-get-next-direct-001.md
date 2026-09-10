# T231 S7 — Direct OpenNT `cmdGetNextCmd` witness

## Scope

This witness admits the original OpenNT `cmdmisc.c` body for COMMAND service
`54:01`, `cmdGetNextCmd`.  It is a local Direct source-parity result only.  It
does not claim native guest continuity, a host child process, or whole COMMAND
closure.

## Preserved original work

The admitted body retains OpenNT's ordering: read `CMDINFO`; ask the host for
an environment when a later COMMAND invocation needs one; return `CF/AX` when
the guest segment is too small; re-enter with `ASKING_FOR_SECOND_TIME`; obtain
the next command; construct the COMMAND tail; set extension, drive, code page,
standard-handle token and `SCS_ToSync` outputs; then resume.

## Declared divergence boundary

Historical `GetNextVDMCommand` was a BaseSrv/NT4 host-composition API and is
not linkable into the portable CLI.  `command_misc_shim.c` supplies only that
input seam from a session-owned declared CLI command and optional copied
multi-string environment.  It has no ambient command queue or raw guest/host
pointer transport.  Every guest span remains bounded and copied.  The source
file annotates the required x86/x64 pointer-token and width divergences inline.

## Reproduction

The following MSVC x64 `/W4 /WX /MT` direct source closure compiled the
admitted COMMAND source files, all current neutral COMMAND shims and the
fixed-width CPU ABI sources, then ran with zero exit:

`build/M0-T231-S7/direct-r2/t231-s7.exe`

Its fixture, `tests/bx-vdm/t231_s7_command_get_next_direct_fixture.c`, proves:

1. CLI-declared `C:\TOOLS\HELLO.COM -x` reaches the original `CMDINFO`,
   receives OpenNT's uppercase path, `.COM` extension value, drive/code-page
   outputs and constructed command tail.
2. A copied `FOO=BAR\0\0` environment first exceeds the guest's four-byte
   segment, returning original-style `CF` with `AX=9`; the next invocation
   restores the held environment, clears `CF`, writes it to guest RAM and
   delivers `C:\TOOLS\RETRY.EXE`.
3. An exhausted declared source takes the original terminal error path through
   the typed controlled-stop boundary rather than inventing a command.

The formal Ninja graph was regenerated with this fixture as a declared target.
Its first full-module run became runner-idle after preexisting module objects;
that runner behavior is not counted as a passing S7 result and remains a build
executor investigation item.  The successful direct source closure above is
the bounded evidence claimed here.
