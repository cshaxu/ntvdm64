# T231 S5 OpenNT `cmdGetInitEnvironment` Direct witness

## Question

Can the original `cmdenv.c` `54:0F` body preserve its first-call environment
construction, required-paragraph sizing, COMSPEC/WINDIR disposition, OEM
conversion and later no-op behavior through the typed COMMAND boundary?

## Source and composition

The admitted body is the direct OpenNT mirror
`src/bx-vdm/bop/opennt/command/cmdenv.c`.  Its original loop reads the host
environment, omits `COMSPEC`, omits `WINDIR` for a DOS session, uppercases OEM
variable names, returns required paragraphs on a short buffer, and emits zero
on non-initial calls.

`command_environment_shim.c` supplies only historic command-lib globals and
the unavailable UI notification declaration.  The existing COMMAND session
persists the original `lpszComSpec`/`cbComSpec` pair after `54:02`; the existing
checked boundary maps ES:0 and writes only the detected double-NUL multi-string
extent.  The inline source deltas are limited to the standalone include seam
and explicit 16-bit casts/initializers required by MSVC x64 analysis.

## Procedure and observations

The focused MSVC x64 `/W4 /WX /MT` fixture compiled the admitted COMMAND
closure and ran from `build/M0-T231-S5/command-init-env-r3/`.

1. A one-paragraph ES buffer returned a larger original required BX count
   without overwriting guest memory.
2. Repeating the first call with that count returned the original multi-string
   with the recovered COMMAND COMSPEC and no `WINDIR=` entry.
3. A non-first call returned BX zero and preserved its guest buffer.

It printed:

```text
T231 S5 direct OpenNT cmdGetInitEnvironment sizing, filtering, and no-op paths verified
```

## Scope

S5 is locally complete for `54:0C`, `54:0D`, `54:0F` and `54:10`.  This does
not claim child-process execution, selector routing or native guest continuity;
those remain separate T231 owner packages.
