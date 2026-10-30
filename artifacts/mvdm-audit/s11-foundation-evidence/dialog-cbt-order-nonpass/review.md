# Dialog and CBT shared-worker ordering non-pass

An exploratory test adapted the prebuilt idle/CBT shared-worker harness by
substituting the existing common-dialog guest for the long-running idle guest.
The hook launcher exited zero, but the host's required condition that the
first launcher still be running after the hook task exited was false, so the
host and observer exited 1. The lifetime trace shows two task retirements with
status zero; it does not establish the required overlapping window activity.
The host thus rejected its own scheduling prerequisite, not a validated
product behavior. Inputs, stage manifest and trace hash are retained here.

The temporary test adaptation was removed. In particular, this non-pass is
not evidence of either a passing concurrent dialog/hook interaction or a
product defect, and is not the earlier `publish-native-return` failure. A
future deterministic witness needs a guest or test-host synchronization point
that proves both guest workloads are live at the desired window operation;
no guest may be rebuilt under the current restriction.
