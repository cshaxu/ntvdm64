# M0 T279 S1 — original support-library ABI/build-form closure plan

The original OpenNT `sources` files declare exactly three runtime candidate
translation units:

- `oemuni/file.c` and `oemuni/process.c` for `oemuni.lib`;
- `suballoc/suballoc.c` for `suballoc.lib`.

S1 reads their actual call/declaration forms, preserving source location,
calling convention, layout, result and failure behavior. It assigns every
external boundary to an existing owner: direct public CRT/Win32 use,
`adapter-win32`, `adapter-softpc` at caller-owned allocator callbacks, or a
named deferred owner. No name-only correlation is sufficient.

`oemuni/toemuni.c`, `suballoc/tsa.c` and `suballoc/debug.c` remain original
tool/test/diagnostic mirrors. `dirs`, `makefil0` and `inc` remain source/build
carriers, not a synthetic runtime library.
