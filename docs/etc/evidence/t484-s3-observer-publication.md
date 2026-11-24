# T484 S3 P93 observer and publication cleanup

Retention review covers B0154-B0156, including the already-native thread
lifecycle runner. Observer error transparency has independent negative
coverage and remains; its compiler recipe now uses the shared CMD/native
fixture graph. The software-video PowerShell wrapper contains no assertions
or current callers and is deleted. Its unchanged native C test remains a
direct native target. The thread lifecycle runner already delegates to CMD;
its session binding, rollback and terminal ownership assertions remain useful.

Both x86 and x64 pass: observer disabled/long-path/missing-directory/enabled
modes preserve caller error10004; publisher latest-frame coalescing, idle,
repeat, palette, capture, drain, resume, sticky failure and50 stop/join cycles
pass; thread session/module binding, rollback, unbind and terminal-intent
markers pass. These are isolated native contracts, not guest acceptance.

Runtime supervisor PID536 exited0. Final supervisor PID30348 exited0:
cached full host build,12 unchanged fixture hashes/mtimes and11 raw host
inputs/22 published copies pass. Product bytes are unchanged; the existing
dual publication is verified, not described as a new release.
No product, mirror, original guest or approved asset content changes.

[Receipts](../../../artifacts/build-migration/t484/s3-observer-publication/result.json)
retain terminal outcomes, native cache identity and runtime markers. Raw logs
remain under build/t484-s3-observer-publication. Three inventory dispositions
complete:190 complete/165 remaining of355. Retrospective retention review
is still incomplete; S3 remains active and S4/S5 remain pending.
