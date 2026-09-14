# M0 T411 S3 — Quick-event provider disposition

## Result

The selected original `qevnt.c` has distinct inputs: restart initializes at
100 and seeds conversion state, while calibration is sampled during periodic
recalibration and zero explicitly takes the no-sample return path.  No exact
OpenNT implementation body for either CCPU provider exists in the searched
selected OpenNT/OpenNT-4.5 trees.  The registered NTVDMx64 fallback supplies
the source-derived disposition `host_get_q_calib_val() == 0` and restart 100.

The former binder incorrectly returned its mutable restart value for both
slots.  S3 changes only calibration to zero; restart remains independently
stored and writable because the C-VID table includes its setter.  This is a
three-line semantic correction, not a new timing algorithm.

## Verification

The focused x86 binder fixture was rebuilt in
`build/M0-T411/S2/r004-cvid-vector-slim-run` and returned zero.  It verifies
calibration remains zero, initial restart is 100, setter changes only restart,
and normal/second vector publication remains valid. `git diff --check` and
the T411 static vector verifier pass.

## Disposition

The local three-function carrier remains necessary only for the writable
restart API; its calibration branch now matches the registered source-derived
fallback.  No further C05 deletion is source-proven. E01 event policy moves
to S4.
