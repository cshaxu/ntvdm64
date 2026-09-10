# M0 T365 — DOS `.COM`/MZ execution and parent return closure

T365 closes its selected low-dependency `.COM` host-file cohort.

- S1 proved that normal DOS EXEC is guest-owned: `$Exec`, JFN/SFT, PSP/MCB,
  termination, parent restore and `INT 21h/4Dh` are distinct from host
  `cmdExec32` and BOP leaf repair.
- S2 recovered the first bounded host seam: original `DEMOPEN`/`DEMREAD`/
  `DEMCLOSE` now retain original file/error/register order while replacing
  native guest aliases with the existing session identity map and bounded
  lease.
- S3 proved that those original DEM sources and bindings are inputs to the
  final CPU40/x86 `original-softpc-process.exe`.
- S4 made the one frozen observation.  The existing `/C EXIT` bootstrap was
  consumed and then the source-defined later `54:01` no-next-command wait
  timed out.  No guest `$Exec` or `.COM` file lifecycle was reached.

T365 does not claim that a DOS child, MZ relocation, parent return, native
child or interactive command session works.  The exact next prerequisite is a
source-shaped app/BaseClient declaration that supplies an actual DOS program
record to the original COMMAND path; it must be planned as an owner package,
not created from this trace as a DEM/BOP patch.
