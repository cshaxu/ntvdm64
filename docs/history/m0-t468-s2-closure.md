# M0 T468 S2 closure — printing and spool bounded acceptance

S2 closes the selected existing original printing/spool bindings without a
production-source or guest-media change.  It proves real paths where the
current observed provider exposes one and records an explicit non-pass rather
than installing a substitute where it does not.

## Accepted evidence

- The [S1 owner matrix](../etc/evidence/m0-t468-s1-printing-spool-owner-matrix.md)
  establishes one original owner for raw jobs, HDC documents, device setup,
  and PrintDlg conversion/callback state.
- The [S2 witness record](../etc/evidence/m0-t468-s2-hdc-print-witness.md)
  retains a real Win16 Microsoft Print to PDF HDC normal/cancel document,
  balanced abort callback and two DC retirements.
- The same record retains original raw `OpenJob`'s explicit no-legacy-profile
  return (`openJob=0; closeJob=0`), not a provider replacement.
- Real Win16 `COMMDLG@20` accepts `PD_RETURNDEFAULT` with result one and
  extended error zero; its original hook cancellation returns zero with zero
  extended error and one balanced `MakeProcInstance`/`FreeProcInstance` pair.
- Current g454 device capability remains an explicit unavailable endpoint in
  the isolated provider path; no test-only provider, host mapping, or runtime
  workaround was accepted.

## Boundary and handoff

S2 does not claim raw normal-job delivery, g454 provider completion, returned
PrintDlg DC retirement, or abrupt task loss cleanup.  These are finite next-
subtask failure/retirement concerns.  The current published ten-image package remains
`build/t467-s3/package-r2`; S2's tests and documentation contain no production
binary and guest media remains unchanged.
