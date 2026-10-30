# Current-source SOL native-return diagnostic

The diagnostic DLL was linked from the current selected WOW32 inputs with a
temporary copy of `wow_user_task_lifecycle.c`. Its only behavioral change is
failure-only tracing identifying window publication, borrowed client graph,
or menu client refresh. Its SHA-256 is
`0D9698761CEBB09366354272BDE728F0C2704EC5600639B7A9DC00F69187C75E`.
This is a diagnostic provider, not a release image. The isolated ten-image
package and source identities are in `manifest.json` and `inputs.csv`.

On the visible desktop the SOL Options probe completed eight open, close and
parent recovery rounds, and the launcher completed successfully. `probe.txt`
and `result.txt` retain the positive checks. The window trace contained no
diagnostic publication failure or `publish-native-return` status 1400. The
full diagnostic build and trace are under
`build/t473-s11/s11-current-return-stage-3/` and
`build/t473-s11/s11-current-return-stage-sol/`.

The earlier intermittent 1400 did not recur in this run. The run cannot
attribute that earlier failure, exclude races, or demonstrate a correction;
instrumentation affects timing. It does not change the performance-work
deferral. No guest, product source or installed release was modified.
