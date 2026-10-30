# Real early WOW32 unload cleanup

The preceding [missing-export witness](../wow32-explicit-unload/review.md)
was repeated with test-only copies of the selected bitmap and registry
binding objects that record their actual shutdown entry and state. All other
provider objects and the missing-export setup were unchanged. Both default
and `--wait` launchers reached the original `init-export-missing` branch and
returned 1114. In each real worker, process detach with a null reserved
argument was followed by registry shutdown and bitmap shutdown, in that order.
The registry token list and bitmap-lock initialization flag were both empty
on entry and remained empty on exit. The records were checked against each
worker PID and the expected six-event order.

The package was diagnostic-only, sealed as a complete isolated package and
never published. Its staged WOW32 SHA-256 was
`CF8759C9B0181A300D9D7F4D3F7AC079968F592AA0C7189979F2E77361A867F5`;
the unsealed diagnostic objects and source identities are in `inputs.csv`.
The marker performs file I/O and can affect timing. This tests an early unload
before the observed binding resources were initialized; it does not prove
cleanup of populated registries, an initialized bitmap lock, or concurrent
unload after guest execution. No guest or production image was modified.
