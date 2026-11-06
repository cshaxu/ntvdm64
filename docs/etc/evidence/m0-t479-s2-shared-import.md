# T479 S2/shared-library-import delivery

## Scope and source identity

The owner authorizes the shared-component import and subsequent generic test
consolidation. The [audit](../operations/m0-t479-s2-shared-import.md) fixes the
upstream revision and compatibility decisions. The [source manifest](m0-t479-s2-shared-source.json)
records all 46 byte-identical files in handle, process-image, process-launch
and process-hook. The upstream worktree was clean when copied and rechecked;
it was not modified. Existing licenses and third-party attribution remain.

Thirty files already matched, six changed and ten were added. Imported source
adds 342 and removes three lines. The seven-line downstream MSVC shim is
deleted. There are no guest or original MVDM/non-MVDM mirror changes.

Product consumers keep their existing classification, propagation and NTSRV
lifecycle ownership. New batch/propagation/unwind APIs are compiled and tested
but are not wired into product lifecycle policy. Existing product paths gain
upstream odd-packet rejection, DLL-aware staging idempotence and native MSVC
startup-hook support. No new product timeout/retry policy is introduced.

## One reusable test suite

[Shared tests](../../../test/lib-win32/README.md) retain the component layout.
The imported process-launch test's run16s-private dependency is replaced with
a public-provider suspended-create/exit/failure test. Command-line tails keep
the local cmd, redirection and pipe assertions. Local cross-width, process
access, late-guard race and low-address-exhaustion checks remain.

Duplicate handle/attribute/launch tests, two obsolete shim headers, the
downstream override manifest and five redundant compiler-specific runners are
removed. A shared CMake entry and one cross-width matrix runner replace the
runner duplication. Test paths derive from their own location, so test/ and
tests/ layouts both work. The sibling can import this complete test subtree;
its current tests are not yet claimed identical.

MSVC's stale CMake tool-information file selected GNU ar despite the cache's
MSVC librarian entry. Nested project() calls made this visible for new static
test libraries. Nested project calls are now avoided and the product CMake
selects the librarian beside its actual MSVC compiler. No cache was cleared.
GCC exposed an unquoted TEST keyword in the imported CMake test loop and
missing Windows API version declarations on two test targets. Both are fixed
in the common test build, without modifying imported component source.

## Verification record

Evidence root: build/t479-s2-shared. All runs are hidden, owned background
processes with retained start/PID/exit records. Failed configuration attempts
remain as attempt1/2/3 logs and are not counted as passes.

| Check | Observed result |
| --- | --- |
| Source inventory and SHA-256 | 46/46 identical to pinned upstream |
| MSVC x86/x64 shared CTest | 14/14 each on final test-build configuration |
| GCC x86/x64 shared CTest | 14/14 each |
| MSVC and GCC cross-width matrix | 13 cases each pass: two image-access cases, four direct pairs, four staging/helper pairs, two rejections, low-address exhaustion |
| Cached ten-component build | x86/x64 successful |
| Service fixtures | 42/42 each architecture |
| Worker/frontend RPC unit assertions | 126 and 90 checks, zero failures, handle delta zero |
| Frontend/VM/real guest/Full gates | All selected gates pass; Full 16/16 in 376192 ms |
| Coherent publication | Ten images and hook license match at both destinations; this P records delivery |

The tests prove the exercised compiler/API/lifecycle cases, not universal
compatibility with every host, security policy or concurrent process change.
New reusable APIs' fixture results do not imply new product integration.
Guest-bridge work remains in its distinct S2--S5 workline.

## Final review and publication

Full runner PID 6368 started 2026-11-06T01:04:22.6550420Z and exited zero.
The same package f86fa099-e0ba-4d7f-bbd3-721fbee9b824 is published to
assets/release and O:/winnt/system32, with the release manifest committed.
DOS4GW/NESticle remains the previously excluded external-fixture case; it is
not a pass. Real generic-thunk normal/mismatch cases also passed. No guest
bytes changed. Exact retained run results are in the [verification summary](m0-t479-s2-shared-verification.json).

Documentation governance and its self-test pass. The continuation check now
recognizes an explicit open-T sentence within a paragraph; a regression test
covers that formatting without weakening the exact next-S rule. Current
links the actual T479 S1 closure and original candidate proposal.

Git whitespace checking reports upstream trailing blank lines in the two new
process-image .c files. Preserve them for byte-exact import; all other changed
files pass. The source manifest remains authoritative for those original bytes.

Owner acceptance on 2026-11-05 closes S2/shared-library-import at 967ef3d31.
The published package is retained; no rebuild is required for this closure.
