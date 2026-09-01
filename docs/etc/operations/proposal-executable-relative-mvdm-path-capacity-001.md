# Executable-relative MVDM package-path capacity disposition

## Purpose

Close the bounded installation-path contract shared by app's executable-
relative `mvdm` root and original COMMAND configuration preprocessing.  The
task preserves `cmdconf.c`'s historical short-path capacity; it does not widen
or rewrite its configuration algorithm.

## Source facts

- Original `base/mvdm/dos/command/cmdconf.c` stores the normalized system root
  in `achSysRoot[64]`, and accepts `GetShortPathNameA` only when its result is
  nonzero and shorter than that buffer.
- Original `softpc.new/host/src/nt_pif.c` selects `config.nt`/`autoexec.nt`;
  the admitted product divergence replaces only the NT4 installed-system-root
  lookup with app-selected immutable `mvdm` media.
- App currently derives that media root from the executable directory and
  stores it in a larger session buffer.  Storage capacity must not silently be
  mistaken for the original COMMAND configuration capacity.

## Ordered subtasks

1. **S1 — source/ABI/caller capacity audit.**  Compare selected original
   `cmdconf.c` and `nt_pif.c` forms, current app/session/adapter binding, and
   all literal capacity/failure checks.  Record the exact accepted and
   rejected shapes.
2. **S2 — source-shaped package admission.**  If S1 proves a missing early
   product disposition, add one app-owned preflight that rejects only a root
   which original COMMAND cannot normalize into its 64-byte short-root form
   or whose default system-media names cannot fit original caller buffers.
   Retain the original mirror's later file-existence checks and failure
   direction; do not fabricate a short path, copy media, mutate the host,
   change `CONFIG.NT`, or widen a historical buffer.
3. **S3 — local contract and formal-link closure.**  Prove accepted/rejected
   boundary behavior with a bounded app/session contract, rebuild the affected
   selected CPU40 Win32/x86 graph, and close documentation governance.

## Exclusions

No guest source/content change, BOP/provider repair, path virtualization,
symlink/junction fabrication, fallback to host `%SystemRoot%`, broad long-path
support, registry/host installation change, CPU30, Bochs, x64 runtime
admission, or guest execution-continuity claim.

## Completion

The product has one explicit disposition for a selected `mvdm` root that is
not representable by original COMMAND configuration preprocessing, while the
imported COMMAND/PIF source remains minimal-diff and its original algorithm is
still the only configuration producer.
