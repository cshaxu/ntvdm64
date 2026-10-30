# Winmine diagnostic return-identity observation

The existing PID-bound `verify-wow-multi-dialog.ps1` probe ran four Winmine
Custom dialog open/close and parent-recovery rounds using the separately staged
`current-return-identity/package`, with window and callback tracing enabled.
All four probe checks passed and the launcher observer recorded exit zero.
The retained manifest identifies the actual staged images; this diagnostic
package is not the published production package. The source Winmine executable
was copied into the isolated stage and was not modified.

No `IdentityFailure`, diagnostic substage failure, or
`publish-native-return` marker appeared in the captured window trace. The
main window was minimized (coordinates -32000,-32000) during observation;
the probe exercises message-driven dialog behavior, not visual gameplay or
window placement. Four passing interactions are a bounded non-reproduction,
not evidence that the historical 1400 is fixed. The diagnostic's previously
validated negative controls cover only selected required-root and menu-owner
failures, not every publication failure.
