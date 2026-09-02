# M0 T360 S6 — invalid long-stage-root observation disposition

The report and JSON envelope were successfully written for the new package,
but that package was located at
`build/M0-T360/S4/original-systemroot-package`. Its app-selected SystemRoot
was 67 visible bytes (including the repository path), not the intended short
installation spelling.

The console-owned process timed out and its mapped product frames again enter
`app_report_media_root_rejected` and `main`; no BOP, SAS-store or exception
report was produced. The correct root-relative media topology therefore does
not by itself make a long installation path valid. This is an app admission
result before SoftPC/COMMAND, not a CPU or guest result.

The sole replacement S7 uses the user-designated `O:\\ntvdm64` root, whose
complete original `shell=` value is 54 visible bytes. Its existing `mvdm`
subtree is explicitly preserved and is not an input in the new manifest.
