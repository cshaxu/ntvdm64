# T484 S3 P213: retire isolated historical runtime staging

Stage-OriginalSoftpcRuntime.mjs has one executable caller, its own
original_runtime_layout_test.mjs. No current product, build or regression
entrypoint selects either. The old stage selects a single NTVDM plus optional
redirector and guest inputs including build/output/dos/REDIR.EXE and DOSX.EXE;
it does not compose the current eleven-image package or its manifest.
Remove both files rather than migrate this second staging implementation.
The T427 historical test result remains; its link becomes a historical source
name instead of pointing at a deleted live file.

The retained full gate test/observation/verify-guest-media.py verifies approved
archive hash, exact membership, per-file archive identities, unique canonical
package paths and installed media hashes. It allows the existing configuration
and manifested VCDEX host-provider boundaries. No archive or guest is changed.
This standalone read-only assertion is not a build interpreter dependency.
Current complete-package staging remains an explicit S4 migration obligation.

Initial PID14720 exited1: the selected O:/winnt manual installation lacks some
complete-manifest guest files, and without the expected complete host manifest
its VCDEX comparison uses the archive row. This was the wrong acceptance input,
not a deletion regression. No files were installed or repaired there.
Final PID26128 started2026-11-24T14:20:47.7124962Z, using the existing full package
build/t484-s2-worker-cmake/candidate-final. The owned run is
build/t484-s3-original-stage-retirement/run-final.ps1; guest-media-final.log and
result-final.json retain actual validation, cached full build and11raw/22
published image identity results. Documentation/tracker/diff checks precede
commit and push. No fresh guest execution or Full runtime pass is claimed.

Frozen inventory319/355 disposed,36pending;23 supplemental preparation chains
remain. The removed self-test is supplemental and does not increase the355
baseline. The recorded concurrent-output failure remains unchanged and open.
