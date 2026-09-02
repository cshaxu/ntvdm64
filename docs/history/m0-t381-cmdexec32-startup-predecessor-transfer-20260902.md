# M0 T381 — COMMAND startup predecessor transfer

The LPT transfer was corrected by T382, but T381's existing current-formal
observation still supplies a separate, source-owned fact: it returns from
original `54:0F cmdGetInitEnvironment` and never reaches original `54:01` or
`54:08`.

That gap belongs to the earlier original COMMAND startup command-delivery
cohort: `cmdGetNextCmd`, initial Base VDM command record, first-VDM state and
the exact `VDMINFO` command/environment/current-directory contract.  It is
not an LPT failure, and it is not evidence against original `cmdExec32`.

T381 is transferred to M0 T383 until the first `54:01` result is proved.
T381 remains the immediate next owner package after T383 and retains the
original `54:08 -> cmdExec32 -> child -> return` scope.
