# M0 T474 S6 — Public Component Test Ownership

## Scope and result

The S6 test-only change organizes the existing common and lib-win32 tests by
the component under test. `test/lib-win32` mirrors all ten library component
directories; `test/common` mirrors the covered common owners and subdirectories.
No claim is made that every implementation has an independent unit test. Existing
non-public test trees have not all been reorganized by this packet.

A unit test may use the inner dependencies of its owner. A library unit may not
depend on an outer product component. Actual product execution tests and their
fixtures belong to `test/integration`; dependency count alone is not a test
classification. Static ownership and build-graph audits are under `tools/audit`.

The process-hook payload unit was separated from the product native-hook packet
test; the former no longer builds product policy. Its MSVC test compatibility
header is test-local. The packet test runs independently for x86 and x64. The
thread library gained a focused test of accepted and rejected thread entries.
Unneeded outer broker and unrelated library links were removed from the VDM
protocol and pipe-pair unit targets respectively.

## Verification

- Cached x86 and x64 builds passed; no guest or mirror source was modified.
- The moved component tests cover application search, environment, system root,
  RPC management and control, pipe transfer and pair, handles, file identity,
  process observation and failure, process attributes and launch, process image
  x86/x64 classification, thread hooks, and VDM protocol.
- Process-hook upstream identity, MSVC x86/x64, all four cross-width payload
  pairs, direct payload pairs and x86/x64 late-guard tests passed. The x86/x64
  native-hook packet tests passed. CRT ownership and negative link controls
  passed. Original mouse coordinate and text cursor unit scripts and the moved
  frontend-window unit script also passed. The Console client unit target
  required its existing handle-provider dependency; after linking it explicitly,
  its isolated Console probe passed with the expected exit and assertions.
- The full selected product integration gate returned zero, including real
  DOS/native transitions, frontend loss, WOW launch observations and DOS4GW
  startup; those observations are not gameplay or pixel acceptance.
- Documentation governance and `git diff --check` passed.

The staged package was compared with the S5 candidate: five host PE images
differ only in COFF and PE debug-directory timestamps, and all other EXE/DLL
bytes match. No product binary was republished for this test-only change.
The old `tests` areas outside the public-component scope are not represented
as fully migrated. The task preserves unrelated worktree changes.
