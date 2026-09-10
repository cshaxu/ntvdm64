# M0 T338 S1 — Original child/parent-return precondition contract

## Question

Does the original selected source already define the package-media-to-DOS-child
path, and what is the first unresolved owner before choosing an implementation?

## Inputs and procedure

- Reviewed the exact original guest mirrors `sysinit1.asm`, `sysinit2.asm`,
  `file.asm`, `msproc.asm`, and `msctrlc.asm`.
- Reviewed selected original host `cmdconf.c`, `nt_pif.c`, `demfile.c`,
  `demmisc.c`, `cmdexec.c` and their existing same-shaped bindings.
- Inspected T337's unchanged staged `mvdm/config.nt` and fixed x86 observation.
- Recorded every reached edge in the companion ledger.

## Observations

1. NTDOS initially carries `\\COMMAND.COM` in `sysinit2.asm`, but normal
   configuration processing overwrites the shell target.  Original
   `cmdconf.c` emits `shell=%SystemRoot%\\System32\\command.com /p ...`.
2. The selected `GetPIFConfigFiles` binding resolves `config.nt` from the
   session MVDM system root.  Thus the original `cmdconf.c` expansion creates
   the guest shell target beneath the packaged `mvdm\\system32`, without
   creating or modifying a real `C:\\COMMAND.COM`.
3. NTDOS then preserves its original file-open, EXEC, child-entry, parent
   cleanup and COMMAND `54:0B` result-return sequence.  No app guest loader,
   BOP substitute or second executor occurs in that sequence.
4. T337's fixed x86 observation became a bounded timeout rather than the
   prior `50:3D -> demExitVDM` terminal.  Its retained external snapshot does
   not identify a subsequent guest CS:IP or a unique CPU40/guest owner.

## Interpretation

The packaged SystemRoot binding is source-shaped and eliminates the earlier
false inference that a portable product must copy `COMMAND.COM` to the host
drive root.  The child/parent-return chain itself is already original source
and requires no replacement.  The earliest unresolved item is now the
post-configuration CPU40/guest-continuation observation boundary, not a DOS
file, BOP, drive-virtualization or media-loader provider.

S2 must first obtain one fixed, source-attributable continuation observation.
It may recover a source-defined prerequisite only after that result identifies
one; it may not fabricate a child result or infer a new BOP service.

## Confidence and limits

High confidence for source ordering and package-media identity.  No DOS child
execution, parent return, or x64 guest execution is claimed.
