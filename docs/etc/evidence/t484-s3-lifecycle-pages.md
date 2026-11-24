# T484 S3 P125 native dispatch and actual page-domain producers

B0195 adds four CMD/native entries: native-dispatch-gate, window-procedure,
window-borrow and graph-snapshot. The two page fixtures select the actual worker
page-domain object and its original CCPU40 closure, without the parent storage
stand-in. The procedure provider namespaces only the two callback-scope exports;
the window fixture counts/delegates to those real functions, while the native
gate fixture deliberately refuses guest admission. Its existing bounded storage
profile is retained. Neither native dispatch row claims real guest scheduling.

Two old fixtures lacked current provider initialization. Bind the existing
window access provider in the gate fixture, and window/class/control providers
in the procedure fixture. They point at actual original layout/control functions;
no success-only behavior or product path was added. Remove the procedure
fixture's now-duplicate raw wowuserp.h inclusion, already supplied through the
shared fixture header. Runtime assertions are unchanged.

Failures remain evidence: PID27072 fails compilation on duplicate original
struct declarations; PID33400 then fails with c0000602. Observer PID14756 and the
linked map locate RVA133f2 in wow_control_original_provider, reached at terminal
window cleanup without its required provider. Completing fixture initialization
resolves it. PID8228 passes all four and cached full/publication identity checks.
Counter-review then preserves the old direct-object link order and removes21
net lines of unreachable old producer setup; window-borrow fault variants remain
on the old path rather than being silently discarded.

Final PID21968 repeats all four, validates cache hash/mtime and x64 rejection2,
and runs cached full host build plus11 raw/22 published hash checks on final
code. Actual CCPU markers prove normal/exception borrowing, duplicate-slot and
preparation failures, graph mutation, retained readers and dead-root withdrawal.
Procedure assertions prove A/W CallProc restoration, independent targets after
source retirement, tagged callbacks and native subclass forwarding. These are
bounded actual-CCPU/native tests, not a Windows installation acceptance run.

Eighteen earlier native binaries are raw hash-identical and their final graph
dry run has no work. Existing compiler header/parameter and LNK4217 import/local
warnings remain visible; no linker duplicate-provider override is used.
Production/mirror/original-guest bytes are unchanged. Dual publication was
verified, not republished.

[Receipt](../../../artifacts/build-migration/t484/s3-lifecycle-pages/result.json)
records runs and cache. Raw evidence: build/t484-s3-lifecycle-pages, including
both failed attempts, procedure-debug and final-cases. B0195 is partial:
23/30 normal public entries native,7 plus fault variants remain. Completed
dispositions stay225/355,130 open. Remaining producers, B0166/B0169, retrospective
cleanup and S4/S5 are not closed by this batch.
