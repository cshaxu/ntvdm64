# Bounded native-return diagnostic repeat

The existing failure-stage diagnostic provider, built from the current
`wow_user_task_lifecycle.c`, completed 24 SOL Options opens, closes and
parent recoveries in one visible-desktop run. The owned launcher exited zero;
no diagnostic publication failure or native-return 1400 appeared in the
trace. The package manifest, probe and observer output, and trace hash are
retained here; the full trace remains in the task build directory.

This result does not identify the earlier failure or prove that it was fixed.
One process and one schedule are not exhaustive, and instrumentation may
alter timing. The diagnostic package was not published; no guest was changed.

A separate old-input control reused the nine non-WOW32 component inputs from
the historical SOL non-pass and a failure-only diagnostic WOW32. It also
completed 24 rounds with launcher exit zero and no failure file. Its inputs,
summary and hashes are in [old-input-control.json](old-input-control.json);
full output remains in `build/t473-s11/sol-old-provider-triage`. The WOW32
provider differs from the historical uninstrumented image, so this is not an
exact binary replay. An earlier injected window failure validated that
logger's window-stage path; borrow and menu negative controls were not
established. This control therefore cannot exclude the historical failure or
establish its cause, and does not justify further repetition on its own.

## Controlled stage-label negative controls

A separate x86 executable compiled the exact `retirement_trace` and
`publish_guest_return` function texts extracted from the diagnostic provider
source used for the current-stage repeat (source SHA-256
`9536802E9BD69912FD3E062F120A04E70A84388EAEE657E0499408D7B2253667`).
The [fixture](stage-controls.c) supplies controlled window, borrow and menu
seams, but calls the extracted real trace writer, Win32 last-error APIs and
Win32 file APIs. It is not the production DLL or a real window failure.

For both initial and native-call paths it injected `ERROR_INVALID_WINDOW_HANDLE`
independently at each of the three calls. Each failure wrote its corresponding
`diag-publish-window`, `diag-publish-borrow` or `diag-publish-menu` label and
status 1400; preserved the error; did not call later stages; and retained the
pending-publication flag. Both success controls invoked all three stages,
cleared the pending flag and created no failure log. The fixture exited zero
and compiled under x86 MinGW with `-Wall -Wextra -Werror`. Fixture SHA-256
`DB43A717EBA1A6BD84ABFE69509FF11D57F223E1BA24A3D9087CC58E08BDEEA0`;
test EXE SHA-256
`6D54351239281EC5D208931F3BDCD6A7B6F37F7A418A3DC0FE845D6E62D66B55`.

This establishes controlled logic of all three labels for the current-stage
provider, not an injected failure through that compiled DLL, a real guest
failure, or the source of the historical 1400. In particular, it does not
upgrade the old-input failure-only provider's borrow/menu controls or prove
that an absent trace implies no failure when file writes fail.
