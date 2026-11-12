# M0 T483 S2 closure

S2 resolves all 76 restore-original entries in 21 mirror files in the frozen
S1 register: 75 implemented, one corrected to retain. Three batch deliveries
preserve the stable T483-D identifiers and original logical source positions.
P1 is `042d67f89`; P2 is `54cc12f5e`; the commit containing this record is P3.

## Results and boundaries

Original packing, callback arguments, declarations, release-build ThunkNLS
initialization, selected diagnostic source and conditional ordering were restored.
Duplicate declarations/list implementations and obsolete report calls/providers
were removed. Native registry and path-output boundaries remain; no private
NT4 implementation was imported merely to remove a diff.

D2233 was misclassified as diagnostic restoration in S1: it covers four whole
unconnected message-name handlers. Current production uses MessageBufferSend,
not these handlers. Their bodies remain omitted; the correction and caller
search evidence are in the [P2 review](../../../artifacts/mvdm-audit/t483-s2/batch2-review.json).
No other S2 entries remain unresolved. S6 pending-evidence entries are untouched.

Across S2, production mirror lines are +459/-200 (net +259), mostly restored
original multiline declaration/source form. Autonomous production is +1/-92
(net -91); tests +119/-11 (net +108). These are literal source-line counts,
excluding release binaries, documents, evidence and generated build outputs;
they are not a claim of reduced binary size or faster runtime. See
[footprint](../../../artifacts/mvdm-audit/t483-s2/source-footprint.json).

## Verification and delivery

Each batch passed cached Ninja/ccache x86/x64 builds, applicable focused checks
and all 16 selected Full gates. P1 covers original packing and real WOW failure
paths; P2 covers original RTL/list contracts and six selected-source ThunkNLS
controls; P3 covers real USER layout, multipass pointer identity and deliberate
capacity-overflow rejection. The initial P3 test-log argument error was fixed;
the successful r2 result replaces no product failure.

Selected MSVC preprocessing proves 24 + 50 + 9 restored/adjacent logical-line
anchors; P3 additionally checks ten framework/macro positions against originals.
This does not claim every pre-existing excluded directive mapping is repaired.
Tests with controlled API boundaries are not guest integration substitutes.
Full's DOS4GW/NESticle external fixture was absent and explicitly skipped.

All three coherent 11-image packages were published and hash-verified at
assets/release and O:/winnt/system32. The current package is `fd1f2722-a425-4881-97f6-174be44d6b6f`.
See [final batch receipt](../../../artifacts/mvdm-audit/t483-s2/batch3-delivery.json),
[Full receipt](../../../artifacts/mvdm-audit/t483-s2/batch3-full-timings.json) and
[execution ledger](../../../artifacts/mvdm-audit/t483-s2/execution.json).
Original guest and assets/winnt.zip are unchanged. Governance, link and Git
checks accompany P3. Parallel QUEUE edits and the win203 proposal are excluded.

Owner authorization automatically admits S3 (120 delete entries / 51 mirror
files), then S4 and S5 in order. S6 must wait for a new owner instruction.
