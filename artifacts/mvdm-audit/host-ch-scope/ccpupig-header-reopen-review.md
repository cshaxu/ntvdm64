# CCPU PIG header: excluded decision requires reopening

`softpc.new/base/ccpu386/ccpupig.h` was marked `整体排除` at the review snapshot and has since been reopened to pending in `shard-1/file-audit.csv`; its old recorded reason says its inactive PIG test-header role is excluded consistently with the earlier `c_getset.c`/`ccpupig.c` dispositions. The current `shard-3/file-review.csv` now records **both** original implementation files as `pending`, because the original differential CPU/profile capability and its applicability are unresolved. The header's exclusion rationale is therefore no longer consistent with its current source-owner decisions.

Original OpenNT and OpenNT-4.5, plus current copy, share SHA-256 `73b2e7ceda4cb3ba83343b1fe07a428dccab76a196b2d7c3951c37962be1e1db`. Original `ccpupig.h:12-167` wraps its comparison-action enumeration, CPU/NPX state interfaces, synchronization and instruction-history contracts in `#ifdef PIG`. Six observed current worker object dependency records include the header, but the selected x86 flags are non-PIG; consequently a VALID include does not mean these declarations are instantiated or that differential testing is operational. Original `c_getset.c` and `ccpupig.c` hold the corresponding PIG implementations; their cached selected-profile objects contain no ordinary executable bodies. This is not a runtime failure report.

**Recommendation: reopen this physical header to pending**, without claiming the production binary executed it. The S11 CPU owner should:

1. Trace the original PIG comparison source, consumers, second-CPU/test environment and dependencies; decide its applicability within the approved CPU40 scope rather than infer irrelevance from nonselection.
2. If recoverable and required, prove the original register/NPX state transfer, action/synchronization and interrupt/exception behavior with a valid test environment, including startup, negative/failure and cleanup. Preserve original profile and do not revive the prohibited CPU30 runtime route.
3. If a specific dependency cannot be provided, document it and the observable impact; distinguish unsupported differential diagnostics from actual guest CPU instruction/exception acceptance.

Do not mark original PIG/debugging, CCPU instruction fidelity or guest behavior as passed solely because this header is byte-identical or has a compiler dependency. This review changes no product, guest or shared ledger.
