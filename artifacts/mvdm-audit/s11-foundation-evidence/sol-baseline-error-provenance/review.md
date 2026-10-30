# Historical SOL 1400: preceding error observations

The retained baseline window trace records `WindowEntry` for the same main
HWND, message `WM_NULL`, with error 1400 at lines 205, 261 and 315. Each is
followed by creation of the next Options dialog; the independent probe reports
three successful open/close/parent-recovery rounds. The fourth dialog is
created at line 328; a later native-return publication fails with status 1400
at line 348. The excerpt, complete probe report, source-file hashes and line
numbers are retained here. The complete historical window trace remains in
the task build directory, identified by its hash.

In both current source and the version before the S11 terminal-lock commit,
`wow_window_dispatch_immutable` saves the calling thread's `GetLastError`
before writing `WindowEntry`, then restores it before dispatch. Thus these
entry records report incoming last-error values, not by themselves failed
window operations. The three earlier 1400 values did not prevent the following
dialog rounds. This narrows interpretation of the trace: a 1400 value can
already be present on that thread in a successful sequence. It does not prove
the final failure inherited the same value, establish the failing publication
substage, or identify a dead HWND. The historical runtime manifest does not
itself bind this exact source revision; the source comparison is a conditional
interpretation, not a source-to-binary proof. No runtime or guest was changed.

The next useful failure witness must distinguish a newly reported error from
an incoming value and identify the failing operation and object generation;
clearing an error or suppressing failure would not repair the underlying
contract.
