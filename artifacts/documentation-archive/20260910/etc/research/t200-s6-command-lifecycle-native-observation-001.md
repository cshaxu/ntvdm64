# T200 S6 — COMMAND lifecycle native observation

One source-built run at
`artifacts/build/t200-s6-command-lifecycle-native-r1/` reports `runs: 1` and
the fixture controlled exit.  The repaired `54:0B` is not observed; the
frontier remains valid `54:01` with `delivered=1` and an exhausted one-slot
plan.  OpenNT `tcode.asm` shows the ordinary COMMAND loop returns to
`do_again` and calls `54:01`; `54:0B` is the distinct shell-out return path.

The next owner package is therefore `cmdGetNextCmd`'s no-next-command
disposition for the contained CLI profile, not another `54:0B` alteration.
