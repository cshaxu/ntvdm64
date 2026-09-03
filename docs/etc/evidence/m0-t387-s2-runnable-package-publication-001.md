# M0 T387 S2 — Runnable package publication and no-argument disposition

## Question

Can the current formal CPU40/x86 product be published into the fixed
`O:\ntvdm64` media root, accept an ordinary positional DOS command there, and
give direct no-argument launch an honest non-silent disposition?

## Inputs

- Formal product:
  `build/M0-T386/S3/base-env-x86/original-softpc-process.exe`.
- Fixed installed media root: `O:\ntvdm64` and its format-4
  `runtime-manifest.json`.
- Current positional declaration source and focused Base VDM fixture.

## Procedure

1. Rebuilt the focused x86 declaration fixture and linked the formal CPU40
   product.
2. Copied the formal product byte-for-byte to
   `build/output/ntvdm64-0239.exe`, `O:\ntvdm64\ntvdm64-0239.exe` and the
   selected `O:\ntvdm64\ntvdm.exe`.
3. Rechecked all 23 immutable media entries in the installed manifest.
4. Used the fixed console-owning, non-debug observer against the selected
   installed executable with these product argv items:
   `echo M0T387REBUILD > M0398.TXT`.

## Observations

- Every published EXE has SHA-256
  `fe7e5d73d3e061d9fd4daaf1cc2c925befc965a1e6d691be89686b86e528d7077`
  and size `3,138,048` bytes.
- All 23 immutable media entries matched the installed manifest, including
  `NTIO.SYS` and `NTDOS.SYS`; no guest or firmware byte was changed.
- The observer exited `0x00000000`; its fixed system root was `O:\ntvdm64`.
- Original COMMAND wrote `O:\ntvdm64\M0399.TXT` containing
  `M0T387REBUILD` plus CR/LF.
- With no declared command, `entry.c` now presents an app-owned explanation
  that this build supports one declared DOS command per invocation. It does
  not enter SoftPC with an empty Base VDM record, silently exit, or claim an
  interactive COMMAND session.

## Interpretation

The current product route is closed for a positional, declared COMMAND
built-in in the installed media layout. The no-argument behavior is an
explicit temporary capability boundary, not interactive COMMAND. The queued
interactive-console package owns replacing it with a real command producer,
Console input path and original keyboard/IRQ path.

## Follow-up

The next candidate is the single-process interactive COMMAND console recovery
package. It must not alter this declared-command route or guest media.
