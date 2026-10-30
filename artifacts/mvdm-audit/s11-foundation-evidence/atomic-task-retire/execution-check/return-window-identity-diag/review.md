# Failure-only window identity diagnostic

An isolated WOW32 provider adds failure-only logging to the existing ordered
native-return publication: window/payload, borrowed graph, or menu substage;
selected graph capture/commit failures; failed property/name acquisition; and
the enrolled-menu `GetWindowInfo` failure. Where that code still has an HWND,
the diagnostic emits its full numeric value, thread ID and existing error.
It preserves `GetLastError`; logging does not run on the successful path. It
does not cover every possible failure inside graph normalization or menu
publication, and a failure's last-error may already have been changed by the
original checks. Sources, build logs and image/map hashes are retained here.

The diagnostic code was removed from project runtime source after linking,
and the non-diagnostic cached WOW32 was rebuilt. A separate coherent staged
diagnostic package ran 24 SOL Options interactions with ordinary high-volume
traces disabled; every dialog recovered and the launcher completed. No
failure file was created. This is a non-reproduction of the separately
observed baseline 1400, not a fix or Full product pass. The window-ID logging
paths have not yet been validated with targeted injected failures; absence of
output alone does not prove their coverage or that the diagnostic's slight
code-layout changes cannot alter timing. No guest was modified or rebuilt,
and the diagnostic package was not published.

The real-CCPU graph fixture was then linked with the diagnostic provider.
Its controlled dead required root emitted `diag-graph phase=required-root`
with the exact HWND and error 1400; its separately destroyed enrolled-menu
window emitted `diag-menu` with a different exact HWND and error 1400. The
fixture retained its normal `WOW_GRAPH_SNAPSHOT_CCPU` and
`WOW_MENU_DEAD_ENROLLMENT_CCPU` success markers. The fixture executable
contains the diagnostic format strings, and the link map names the selected
graph/menu provider objects. Logs, map and hashes are in `fixture-negative`.
This validates those two logger entrances in a controlled test, not all
diagnostic branches or the unlocated SOL failure.

An initial fixture invocation produced no diagnostic file because copied
source timestamps did not force the intended object rebuild; it is a
non-measurement, not a logger pass or a product non-pass. The corrected run
forced the diagnostic source timestamps, then verified and restored all four
original working-tree source hashes and rebuilt the normal cached provider.

A further eight-round run used the same class of ordinary window/callback
trace as the baseline non-pass, plus the validated failure-only window
identity logger (`traced-02`). All interactions and launcher exit completed;
no failure-only file was produced. That bounded result does not reveal the
intermittent 1400's owner or prove a product fix. The logger's failure paths
remain verified only to the controlled scope described above.

A concurrent external-window churn experiment (`churn-01`) used the existing
test fixture's hidden-window helper and an independent test-owned stop event.
All 16 SOL Options open/close/parent-recovery observations completed and no
identity failure file appeared, but the overall run did **not** pass: the
launcher observer still reported `result=running`, and the helper's own
120-second limit expired before the complete probe ended. Thus continuous
churn for the full run and normal launcher retirement were not demonstrated.
The helper was no longer running and the stage cleanup ran; raw probe,
observer report, package manifest and runner are retained. This does not
establish whether churn caused the launcher non-completion or identify the
baseline 1400, and must not be counted as a passing product regression.

The follow-up `churn-02` used eight rounds so that the fixture's bounded
helper remained alive until the probe and launcher had completed. All eight
open, close and parent-recovery observations passed; the launcher report says
`result=exited`, `exit=0x00000000`, and the observer process exit was zero.
The runner verified that the helper had not exited prematurely, then signaled
and waited for it. No failure-only identity trace was produced. The individual
dialog openings took 2.469–3.484 seconds and close recoveries 3.109–3.500
seconds: this is not a performance fix. The probe and observer reports,
package manifest, runner and hashes are retained in `churn-02`. This controlled
non-reproduction does not explain the baseline 1400 or establish the result
of the earlier 16-round non-pass.
