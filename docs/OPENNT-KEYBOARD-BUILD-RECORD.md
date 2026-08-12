# OpenNT KEYBOARD.SYS Build Record

## Scope

This record covers the keyboard layout device declared by
`base/mvdm/dos/v86/dev/keyboard/makefile`. It is source/build evidence plus a
structural-stage image audit only. It does not show that an NTDOS profile loads
the image, that the keyboard interrupt path works, or that any individual
national layout produces correct scan-code translation. The common evidence
terms and current staged-file check are in `OPENNT-STAGED-ARTIFACT-AUDIT.md`.

## Source Closure and Stage Transformations

The original makefile links 32 objects in `keyboard.lnk`: `kdfnow` first, the
30 layout/data objects in the listed order, then `kdfeof`. A disposable stage
was created under `toolchain-probe/keyboard-stage-v5`; the OpenNT checkout was
not modified.

Two shared inputs were initially omitted by the stage copy, rather than absent
from OpenNT: `postequ.inc` and `copyrigh.inc`. Both were copied unchanged from
the tracked V86 include directory. The SHA-256 of the staged `copyrigh.inc`
equals its source input: `5F5BA93635D6200394F545D20D91532DA8FF30D1EACDA0D786D79D61CD488AE7`.

The stage makes only documented assembler-compatibility transformations:

1. It renames the historic `OPTION` macro and all of its call sites to
   `KEY_OPTION`, and its `MASK` formal to `OPTION_MASK`, to avoid assembler
   directive/reserved-token collisions.
2. It renames the historic byte-emitting `GOTO` macro and all layout call sites
   to `KEY_GOTO`, avoiding a reserved macro-control word.
3. Both JWasm and UASM reject the original 31-level nested `FIND_FLAG` `IFIDN`
   chain. The stage replaces it with shallow, sequential tests in the same
   source order and assigns only on the first match. The mapping table and
   first-match behavior are preserved; this remains a semantic transform that
   needs runtime validation.

## Build

```text
OpenNT revision: 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c
stage:           toolchain-probe/keyboard-stage-v5
assembly:         JWasm -omf -Zm -ml
link order:        keyboard.lnk order, preserved exactly
link:             Open Watcom, explicit format dos
conversion:       source-built opennt-reloc, load segment 0060
```

All 32 original objects assembled with zero warnings and zero errors. The
`keyboard-assembly-audit.json` file in the stage records the module-by-module
result. The compatibility linker emitted two expected historical-shape
warnings: no stack segment and no starting address, defaulting to `0000:0000`.
Those warnings are not suppressed and mean the output is not execution proof.

| Artifact | Result |
| --- | --- |
| linked `KEYBOARD.EXS` | 40,697 bytes |
| `KEYBOARD.EXS` SHA-256 | `356151208A1AB8EDB2210E2A7398BBC268FA68CD2C98B231EE96B4B4EC6325A4` |
| relocated `KEYBOARD.v5.sys` | 40,665 bytes |
| `KEYBOARD.v5.sys` SHA-256 | `1F475C75A35690D27C5C8EBC050673EFE9DCF46F61E115FC48574A9AD8ABD3AF` |

The stage also contained an earlier `KEYBOARD.SYS` candidate with SHA-256
`96FCAC606683CF262DBC030713A9263731EE83F19A10207E5DD632E5EE06C2AD`.
It is deliberately retained rather than overwritten. The two SYS candidates
have equal length but differ first at byte offset 2679 and at 695 byte
positions. This record makes no choice between them and does not claim either
is retail-identical.

## Qualification

The next validation must boot or directly load the chosen candidate through a
fixed research profile, exercise at least the default US layout and one
extended layout, and verify malformed tables, unavailable layouts, BIOS
keyboard interaction, and unload/error behavior. Until then this is a source
closure result only, not a usable keyboard subsystem claim.
