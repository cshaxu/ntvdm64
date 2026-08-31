# M0 T330 — Original exception-context attribution

## Purpose

Capture and map the first fixed-container original guest/CPU40 exception
context after the closed FDC and recursive-frame source packages. This is a
diagnostic-only package: it decides the next original owner from actual
exception data without changing guest, BOP, FDC, ROM or CPU behavior.

## S plan

### S1 — Original exception-filter context contract

Trace the selected original `VdmUnhandledExceptionFilter` and all current
opt-in report seams. Define the copied, fixed-width context fields needed for
source mapping and prove that the report cannot affect exception disposition,
guest state, CPU frame, timer or process result.

### S2 — Opt-in copied context report

If S1 proves the existing report lacks a required field, add only a
report-path-gated copied diagnostic at the existing exception boundary. Keep
the original filter's suspend/filter/resume/termination behavior unchanged.

### S3 — Fixed-container attribution

Rebuild independent x86/x64 formal products. Run one bounded x86 observation
with the immutable short-root media and opt-in report. Map the actual context
to the exact source owner or record the exact source-defined boundary when no
selected original filter receives an exception record.

## Exit

Close only with paired formal links and one exact runtime source attribution,
or with evidence that the selected original filter is unavailable before it
receives an exception record. The report is not an x64 runtime claim and may
not become a permanent product behavior change.
