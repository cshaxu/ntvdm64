# T423 S10 component minimization audit

## Question and baseline

Owner requested simplification of authored architecture, executable/DLL dependency
review, and consolidation of opennt-abi, product-abi and product-package.
Baseline: main c2242d1c3; S9 production delivery 3b40345f8. This is a source,
build-graph, link-map and PE-import audit, not a production cleanup or new runtime
acceptance. S10 and T423 remain open.

## Procedure and evidence

- `tools/audit/Audit-ProductDependencyClosure.ps1 -OutputRoot build/M0-T423/S10/dependency-r2`
  reads the formal x86 and WOW Ninja graphs, walks explicit object/archive inputs,
  and records owner counts and available map objects. r1 mistakenly attributed
  extracted libvterm sources to repository owners; r2 corrects that classification.
- Source callers were checked using `rg` across src, tests and build generators.
- Formal EXE maps were inspected separately from archive membership. Published PE
  imports were inspected using x86 Visual Studio `dumpbin /dependents /imports`.
- Header comparisons used corresponding relative paths in local OpenNT and
  OpenNT-4.5 references. Comparison is not authorization to import or overwrite.

Archive closure includes eligible members, not proof that all members entered a
PE. Header counts are not runtime component counts. File relocation is not code
deletion. The graph reader targets the current generator's simple Ninja syntax;
it is not a general Ninja parser or a runtime dynamic-import resolver.

## Component dispositions

| Component | Observed contents and consumers | Recommended disposition |
| --- | --- | --- |
| product-package | One C and one H, 165 physical lines; header includes worker session; only production caller is mvdm_standalone_worker.c. Other consumers are fixtures. | Move unchanged to ntvdm-exe's startup/package binding. Remove the artificial top-level component. Do not change path/search policy. |
| product-abi | Four headers, 155 physical lines; shared version plus worker/frontend Console I/O, mouse and video contracts; no runtime library. | Keep the small shared contract for now. Do not mix session-dependent package implementation into it. Its README must describe the actual contracts, not three-EXE identity only. |
| opennt-abi/source | 209 declaration/media-of-declaration files, about 2.72 MB; original topology plus selected subsets. | Original declarations may ultimately join opennt-host at original paths, but reconcile duplicate paths and subset provenance first. No blind tree merge. |
| opennt-abi/host-compat | 17 headers, two C files and README; includes RTL/TLS/PEB binding and Console grid code. | Keep truly shared compatibility separate from original mirrors; return exclusively worker-owned adapters to ntvdm-exe. No invented file may enter a mirror as an original. |

The minimum-risk recommendation is therefore **remove product-package as a
component, retain product-abi as a header-only contract, and decompose opennt-abi
by provenance and actual consumers**. Merging all three would reduce directory
count but increase coupling. If eliminating product-abi is later desired, move
I/O declarations to ntkvm's public interface and keep product identity in one
explicit shared header; that is optional organization, not runtime simplification.

Of 209 source files, 197 match a same-path reference byte-for-byte. The other 12
need source-selection/subset reconciliation, not blanket deletion. Three paths
also exist with different contents in opennt-host: `public/sdk/inc/ntexapi.h`,
`ntrtl.h`, `nturtl.h`. The ABI versions are full originals while the other mirror
has selected subsets. Current include order prefers opennt-host, so overwriting
either side silently changes the selected declarations. Invented `_wow32.h`
subset carriers cannot simply be moved into an original mirror.

## Proven authored-code cleanup candidates

| Candidate | Evidence | Action and retained contract |
| --- | --- | --- |
| native_console_capture.c old reader | Lines 75-164, 90 lines; production has no reader callers; tests still call it; symbols remain in ntkvm map. | Remove old hidden-Console snapshot reader from production. Preserve live screen_apply/cells_write and adapt their tests; do not delete the whole file indiscriminately. |
| native_console_view.c old wait | Lines 277-310, 34 lines; no src/tests caller. | Remove obsolete second presentation/wait loop and declaration. Keep current frontend wait/completion implementation. |
| session_service.c launch/wait forwarding | Last two functions, eight lines; no src/tests caller, present in map. | Remove both wrappers and declarations. |
| native_conpty.c ReleasePseudoConsole | Release path is test-only, yet open_events requires the API at production initialization. | Remove the test-only production requirement and separate diagnostic lifecycle probing from the persistent production backend. Never release on direct-target completion. |
| ntmon USER32 dependency | Published import contains only wsprintfW; main.c formats endpoint with it. | Replace owned formatting with bounded CRT formatting, then prove USER32 import disappears. No change to endpoint bytes. |

The first three identify **132 C lines** of removable obsolete runtime code,
excluding declarations and test changes. This is a candidate count, not achieved
deletion. Moving package_layout's 165 lines deletes zero implementation lines.
Do not promise a total reduction before compiling and measuring each actual diff.
Other test convenience/inspection APIs are not automatically dead functionality.

## Dependency findings

The explicit graph contains four ntkvm client translation units in run16, zero
ntkvm units in worker, and 38 ntkvm units in frontend plus eight separately
classified imported libvterm sources. These are source selections, not PE size.
run16's four units are bootstrap, native request client/I/O and process-launch
binding; they do not embed the renderer or terminal parser.

Broad Base archives do not imply every executable embeds the broker server.
Maps show run16 extracting client/classifier/capture and finite startup bindings;
ntsrv extracts dispatch/record resources/streams/waits; ntkvm extracts client-side
command/payload/process/startup values; ntmon extracts RPC security. No evidence
supports deleting these dependencies merely because archives share a name.
WOW32 and VDMREDIR importing ntvdm.exe is their intentional parent-provider ABI,
not proof that those DLLs contain the whole worker.

Retain one ConPTY, one terminal state and one presentation owner. Do not unify
task completion with frontend lifetime, or duplicate input into DOS and ConPTY.
There remains a bounded Console-membership query subprocess in ntsrv/console_query.c
and run16/console_probe.c. It is authentication/discovery, **not** the removed
native hidden-Console I/O helper, nor a ConPTY descendant observer. Removing it
requires an equivalent authenticated membership proof, not trusting an environment
PID. S9's helper-removal claim is scoped to the native I/O backend.

## Implementation order and acceptance

1. Delete proven unused frontend branches and production-only diagnostic API
   requirements; keep focused regression tests on the retained real paths.
2. Move worker package binding without semantic changes; update graph and tests.
3. Audit each compatibility header's formal and WOW consumers, migrate private
   adapters, reconcile original declaration collisions before any source-tree merge.
4. Re-measure mirror diff, authored C/H lines, archive extraction and PE imports.
   Production P deliveries still require x86 builds, inherited DOS/nesting/fault/
   WOW gates and coherent seven-file publication. Audit-only changes do not publish.

Confirmed decisions above have source evidence. A complete opennt-abi import map
is a migration prerequisite not established by this audit's 197 byte matches;
no unsafe full merge is claimed ready. Accepted S9 horizontal-wheel and physical
desktop limitations remain explicit, and SOL/WRITE are not usability passes.

## Audit delivery verification

The generator was rerun to `build/M0-T423/S10/dependency-r3` with the same counts.
`tests/component-integration/verify-frontend-link-ownership.ps1` passed, including
seven leakage negative controls and the launcher rebuild control. Documentation
governance and `git diff --check` passed. This P contains audit tooling and
documentation only: no production compilation, new runtime claim or publication.
