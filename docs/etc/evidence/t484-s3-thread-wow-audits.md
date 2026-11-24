# T484 S3 P152 thread and WOW boundary audit reconciliation

## Scope

B0240 and B0241 are useful standalone read-only ownership checks. Neither
owns compilation or code generation. The current thread audit passes without
modification: VDMREDIR imports worker thread ownership; generic and compat
thread sources remain independent; initialization stays with the worker.

The WOW checker previously parsed only legacy obj/...: cc graph edges. Replace
that parser with the existing shared native graph/source/compiler-dependency
helpers. Select sources through the actual wow32.dll closure, select their
unique real compiler producer, and require VALID dependency records. Keep
the existing minimum coverage and all nine forbidden-header controls; no
boundary is waived or counted as passing on zero coverage.

## Verification

Owned background PID33656 exits0. Thread ownership plus its private-owner
negative control pass. All80 current WOW compiler sources pass, and all nine
private-header injection controls reject. These are compile-dependency/static
checks, not runtime or Full acceptance.

[Receipt](../../../artifacts/build-migration/t484/s3-thread-wow-audits/result.json).
No production source, graph, guest or publication changed. No rebuild or
republication claimed. Two dispositions complete:270 of355,85 remain.
S3 continues at B0242; earlier incomplete cases and S4/S5 remain open.
