# M0 T330 — Original exception-context attribution closure

T330 closed the original top-level exception-filter investigation without a
production behavior repair. The original filter and every selected caller
were mapped, and its report-only adapter companion was extended to copy the
full bounded native context without affecting exception disposition.

Paired x86/x64 formal links passed. One new immutable x86 short-root container
exited with `0xc0000005` after original BOP ingress, but did not invoke the
original `VdmUnhandledExceptionFilter`; consequently it produced neither the
console marker nor the observer exception report. This is the source-defined
boundary of the selected filter contract, not evidence for a CPU, FDC, ROM,
vector or BOP repair.

Evidence: [T330 S1–S3 exception-context attribution](../etc/evidence/m0-t330-s1-s3-original-exception-context-attribution-001.md).
