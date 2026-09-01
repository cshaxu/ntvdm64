# M0 T355 S5 — NTDOS `SHELL` pass-one execution cohort audit

Date: 2026-09-01

## Scope and evidence boundary

This is a static audit of the selected original NTDOS/SoftPC sources and the
selected `NTIO.SYS` map.  It adds no guest change, parser change, CPU change,
build, or runtime observation.  The only runtime premise is S4's copied
post-CONFIG state: `CS=8E08`, `multi_pass_id=2`, and
`commnd=\\COMMAND.COM`.

## Original NTDOS path

The selected map identifies the relevant loaded-image offsets in the same
`009D` file-layout section: `confbot` at `0332`, `org_count` at `0463`,
`multi_pass` at `1F82`, `organize` at `2DEB`, and `commnd` at `3466`.
The map does not expose local label `trys`; that absence is a symbol-visibility
fact, not evidence that the label is absent from the selected source.

The original source establishes this complete pass-one contract:

- `sysinit1.asm` increments `multi_pass_id` from zero to one and calls
  `multi_pass` before the later `2` and optional `3` passes.
- `sysconf.asm::multi_pass` uses the original `confbot` segment and `org_count`
  byte count, calls `organize`, resets the character cursor, and then walks the
  reorganized byte stream.
- `sysinit2.asm::organize` recognizes `SHELL` through `comtab`, emits the
  one-byte `S` indicator, and uses the special `org_file` branch to store the
  filename as an ASCIIZ value followed by its parameter text.
- In `sysconf.asm::tryi`, pass one neither enters the pass-zero DOS= filter nor
  the pass-two/three filters; an `S` indicator flows through the ordinary
  command chain `tryb ... tryk -> trys`.
- `trys` verifies `AH == 'S'`, initializes `command_line`, copies the filename
  to `commnd + 1`, and writes its NUL terminator.  `dfil` therefore consumes a
  configured command only if that exact in-guest sequence executed.

The generated `shell=` source text is already a valid 60-visible-character
line in the fixed `O:\\ntvdm` container.  Consequently, S4's retained default
does not admit a pathname, DEM, or host COMMAND workaround: one of the
in-guest input/reorganization/ordinary-dispatch/write transitions was not
observably completed.

## CPU40 and memory contracts

The selected CPU40 path retains the original owners for the required mechanics:

- `nt_cprgs.c` binds DS/ES getter/setter slots to the CPU40 `c_getDS`,
  `c_getES`, `c_setDS`, and `c_setES` family.
- `ccpusas4.c` owns the original byte/word/string SAS loads and stores; its
  `c_sas_loads`/`c_sas_stores` contracts are the underlying linear-memory
  transport for the NTDOS code and configuration buffers.
- `cpu4gen.h` keeps numeric segment selectors and 32-bit effective offsets in
  the CPU40 interface; this selected x86 product does not introduce a new
  pointer or mapping ABI for the NTDOS parser.

The formal selected CPU40/x86 product linked these original owner packages.
That proves only selected composition, not that the live instruction sequence
entered `organize` or `trys`; no static source fact can distinguish a skipped
instruction boundary from an unobserved buffer/state mismatch.

## Disposition

The next bounded owner is a **read-only CPU40 instruction-position/state
observer** for the original NTDOS configuration cohort.  It must use selected
binary/map identity and copied scalar state to distinguish:

1. whether the original `organize` entry is reached;
2. whether pass one reaches its ordinary `tryb ... trys` chain; and
3. whether the `commnd` store occurs before ConfigDone.

It must not alter guest bytes, parser source, CPU semantics, BOP/DEM results or
the staged media.  A new S is required because it adds an execution observer
outside this static-audit scope.
