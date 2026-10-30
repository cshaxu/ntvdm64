# Identity diagnostic with shared WOW idle and dialog tasks

The existing `verify-wow-idle-dialog-shared.ps1` ran with the separately
staged `current-return-identity/package` and hash-verified prebuilt guest
witnesses. It exited zero: one worker hosted two distinct tasks, the dialog
task created 17 dialogs and explicitly closed 16, and the idle task continued
receiving an idle dispatch after its peer retired. Both tasks retired once.
The exact staged image manifest, observer and native output are retained here;
the diagnostic package is not the published production image.

The lifetime trace contained no `IdentityFailure`, diagnostic-stage or
`publish-native-return` failure marker. The guest workload and trace are a
bounded non-reproduction, not a deterministically forced retirement race or
proof of the historical Winmine/SOL failure's cause. Diagnostic negative
controls validate selected required-root and menu-owner reporting only.
No guest was rebuilt or modified.
