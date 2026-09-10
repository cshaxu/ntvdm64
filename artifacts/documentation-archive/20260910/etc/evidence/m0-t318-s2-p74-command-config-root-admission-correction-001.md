# M0 T318 S2 P74 — original CONFIG root-admission correction

## Correction

P72 correctly repaired the **Base VDM host-current-directory transport** from
an incorrect 81-byte private carrier to the original `MAX_PATH + 1` carrier.
It did not prove successful CONFIG preprocessing.

The original COMMAND `cmdconf.c::ExpandConfigFiles(TRUE)` has a separate,
older boot-file condition: it calls `GetShortPathNameA` into the fixed
`achSysRoot[64]` buffer and treats a zero or over-63 result as the original
bad-system-file terminal path.  This is not the `VDMINFO.CurDirectory` ABI and
must not be widened by the Base VDM transport repair.

## Reproducible path facts

The P72 DOS media root is 100 characters.  On the current volume, its short
path spelling is identical to the long spelling, so it cannot fit the
original 64-byte CONFIG carrier.  The P72 console timeout can consequently be
an original `RcErrorDialogBox` wait before `TerminateVDM`; it is not evidence
that `cmdGetConfigSys` returned to NTDOS.

The P73 disposable root is 51 characters and likewise has no distinct 8.3
alias, but it fits the original carrier.  It is therefore the only admitted
runtime container for tests that require original CONFIG/AUTOEXEC preprocessing
until a future product packaging decision supplies an equally source-valid
short media root.

## Boundary preserved

No COMMAND source is widened or replaced.  The following original contracts
remain separate:

- Base VDM command transport: `VDMINFO.CurDirectory`, `MAX_PATH + 1`;
- CONFIG preprocessing: `cmdconf.c::achSysRoot[64]`, including original
  `GetShortPathNameA` error direction; and
- guest return buffer: the NTDOS caller's 64-byte `DS:DX` CONFIG filename
  buffer.

The P73 short-root access violation remains unattributed and is still not an
ordinary-child `EXEC` or parent-return result.

## Disposition

P72 remains valid as the Base VDM transport repair and COMMAND-ingress
evidence, but its former implied CONFIG-continuity reading is superseded.
All further NTDOS/CONFIG/ordinary-child runtime observations must use a root
that satisfies the original 64-byte CONFIG contract without modifying the
mirror.
