# Debt Ledger

| Priority | Debt | Admission path |
| --- | --- | --- |
| P1 | Preserve a reproducible Bochs 2.6 source/notice/patch manifest after T95 imports the local baseline. | Source-policy review and manifest verification. |
| P1 | Keep the Bochs build surface minimal as later guest callers request features. | Per-feature OpenNT caller evidence, boundary rationale and negative test. |
| P2 | Produce a neutral Bochs/OpenNT hard-boundary report for later `ntvdm64` architecture work. | First admitted bridge slice with no runtime dependency on `ntvdm64`. |
| P2 | Research original OpenNT shell-out/session-broker paths for a DOS guest request to launch or wait for a Win32 program, a WOW16 program, or another VDM. | Establish exact OpenNT caller/ABI/failure maps and decide whether a modern non-invasive, explicitly permitted host-launch capability exists; otherwise retain an explicit permanent deferral. |
| P2 | Reconcile T271's residual BOP/package recovery evidence with the canonical `mvdm-host` rebootstrap: five compatibility headers, app-held selector/service composition, and test-to-overlay privacy. | During rebootstrap, classify each residual as an original MVDM package unit, app composition, session/broker state, or a declared adapter contract; use `m0-t271-closure-20260825.md` and its P10 ledger as historical evidence, not as a source-layout precedent. |
| P3 | Migrate indexed legacy documentation without breaking provenance links. | One manifest-backed migration batch with link verification. |
This ledger contains no active work, roadmap, or completed-task record.
