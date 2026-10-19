# Td build hygiene

## Admission and finite brief

Owner admission: standalone M0 Td, ordinary single-person implementer/reviewer.
The owner requests unified Ninja/optional ccache/build disposal, a read-only
inventory and estimate before implementation, exact-target safe cleanup and
reviewed commit/push. T447 S4 is held at delivered P13, not closed or displaced.

Objective: two reusable cache roots only, build/.ninja and build/.ccache;
ccache temporary files under build/.ccache/tmp. Configure, generated compile
commands and direct cmake --build must agree. Cache-on/off share a tree.
One default-preview cleanup tool owns deletion and final residue inspection;
the document gate remains document-only. No preset parser, background service,
cache framework or recovery state machine. No global user cache deletion.

Baseline: main/origin 3d1ece5b319df505ba471b34d7feab755487e944, clean;
13 published images and immutable guest unchanged. The repository currently
has no supported CMake entrypoint or presets. Actual source-owner Ninja
generators select MSVC /MT x86 CCPU40 and the approved AMD64 components.
Introducing GCC product profiles would not preserve this toolchain contract.

Expected files: real CMake entry/configuration and presets, existing active
Ninja generators, one PowerShell cleanup tool and its real-tool tests,
execution/output policy and linked contributor/build guidance. No production
source, ABI, guest, release binary or documentation-gate behavior changes.
Initial estimate: 15--25 files; 500--900 net script/config/test lines, plus
compact policy/evidence. Final accounting must replace this estimate.

Verification: background owned configure/build/test with actual deadlines;
both native architectures, unchanged incremental object timestamps/no-work,
explicit generated cache environment and hostile caller environment,
cache-off launcher reset, rejected outside-Ninja and accepted inside-Ninja
paths; preview/Apply, preflight-all-before-delete, traversal/link/Git/config/
media/unknown ownership rejection. No runtime/device regression or release
refresh without a runtime change. Actual results, hashes and unproved limits
are retained here before disposable logs disappear.

Temporary owner root: build/td-build-audit, this Td only; 2-hour phase budget,
1 GiB scratch budget and 15-minute no-progress stop. Inventory PID40732 starts
2026-10-19T02:15:16Z with a 15-minute deadline; it exits after read-only scan.
The first restricted launch PID47576 produced empty logs and no results;
it is not a successful inventory. Subsequent jobs record PID/start/command/
deadline/stdout/stderr before execution; join before cleanup. Stop on unknown
ownership, Git history, real/user media/configuration, active target use or
new scope/semantic concessions. Do not kill unrelated processes.

Exit: requirement-to-file/test review, compact evidence and hash summaries,
safe cleanup with exact residual report, documentation/link/diff checks,
commit/push and clean synchronized worktree; wait for owner acceptance.

## Read-only inventory and cleanup plan

2026-10-19 read-only scan uses an explicit stack, never traverses reparse
points, and records direct-child bytes/files/config/media/Git/link findings.
No Git markers, disk images or links were found. All 318 flagged entries
are SYSTEM.INI, WIN.INI or NTVDM.REG and require ownership verification, not
an automatic safety waiver. Process inspection was unavailable in the
restricted context; an elevated read-only CIM snapshot found no process
using this checkout's build. The separate ntvdm64-wow32-audit process is
unrelated and will not be stopped or touched.

| Exact build child | Approximate GiB | Initial disposition |
| --- | ---: | --- |
| M0-T445 | 21.64 | Retired stages; inspect configuration and evidence before deletion. |
| M0-T447 | 14.91 | Saved S1--S4; record P13 receipts and preserve reproducible tools before deletion. |
| M0-T446 | 0.04 | Closed task; inspect copied configuration. |
| compiler-cache | <0.01 | Old repository cache, not user-global; inspect before retiring. |
| output | 0.04 | Historical generated package, verify source media/provenance. |
| performance-research-20261006 | 0.03 | Retired experiment; configuration check. |
| research-win31-filemanager-20261014 | 0.05 | Research outputs; inspect purpose. |
| side-palette | 0.03 | Retired experiment; configuration check. |
| lprobe, queue-wow-priority, queue-wow-residue, research-link16, tools | <0.01 each | Verify tracked producers or duplicate inputs; unknown contents block deletion. |
| ntcon-tree-7c6406575.txt, ntcon-tree-head.txt | <0.01 | Read-only generated tree dumps. |
| td-build-audit | <0.01 | Current Td; remove only after jobs exit and evidence is recorded. |

No deletion is authorized merely by this table. Final exact preflight and
preservation decisions precede Apply. Old CMake trees, if encountered, are
reconfigured, never moved. No old Ninja object tree is renamed into a cache
to evade disposal. Cache roots contain only compiler/build products, not
logs, test media, probes or task records.

## Implementation and pre-deletion evidence

The root CMake configuration and presets now select the real MSVC x86/x64
Release islands, not a copied GCC/other-product recipe. BuildTree.cmake alone
checks directories and selects the optional per-command environment; its
small PowerShell entry connects existing manifest generators to that same
configuration. Cache-on/off presets share each binaryDir and reset launchers.
CMake builds the existing NTW16S/W16Style source owner and exposes the original
worker/WOW/native/Hook manifest targets without duplicating MVDM source lists.
The component's physical C sources replace a second handwritten style list.
Native toolchain initialization and explicit source-owner targets remain.

Five existing style-test consumers now compile into stable configuration
subtrees, copying only selected executable test media into their disposable
run roots. Nine adjacent diagnostics/fixtures lose retired absolute cache/
observer defaults; caller-owned observers/logon helpers are explicit. This
adjacent sweep expands the initial file-count estimate; it does not resume
performance work or change their runtime algorithms.

Clear-BuildOutput.ps1 owns inventory, default preview, Apply and residual
inspection. Ownership includes a sorted path/length/timestamp digest, not
just an extension or a directory label: a new personal.txt cannot inherit
approval from an old generated directory. Sensitive files additionally need
an individually reviewed SHA-256 and reason. Persistent roots and configured
Ninja trees (including descendants) are protected. A never-configured helper
prefix may be inspected/disposed with the same exact ownership checks; test
configuration metadata is not hidden permanently in a cache. Inline manifest
objects allow final in-process disposal without a self-rewriting control file.
There is no new daemon, preset parser, document-gate rule or recovery system.

Background r5 completes exit0 at2026-10-19T02:47:42Z:34 assertions, actual
x86/x64 DLL/demo builds, unchanged object timestamps plus Ninja no-work,
cache mode reuse/reset, hostile caller-cache environment, explicit three-
setting compile commands, real source-manifest common-root/common-rpc library
compilation, repository cache hits and the migrated dual-width title-marker
entrypoint. This is build/fixture evidence, not whole-product guest acceptance.
Final-quality's22 cleanup assertions additionally cover exact membership,
all-target refusal, Git/media/configuration/junction/traversal/cache guards,
inline execution and successful reviewed deletion. The latest22-assertion
pre-Apply replay and positive in-memory prefix disposal complete0 at03:00:01Z.
Configured caches stay protected; the six-byte test-only prefix is removed.

Final-quality exits0 at02:55:11Z:23 changed PowerShell files parse; the
existing call-boundary graph compiles its original string fixture through
the real ccache wrapper and runs WOW_CONTROL_STRING_OK. Documentation,
relative links and diff checks pass. All13 published images remain hash-equal
in assets/release and O:/winnt/system32; assets/winnt.zip retains
7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7.
No src or release diff; zero original host-mirror or guest delta.
Measured repository stats before old-cache merge:190 direct hits,46 misses,
max_cache_size_kibibyte4882812 (5G). This is an actual observation, not an
assumed hit from the presence of ccache.exe. Global user cache was untouched.

The reviewed old repository cache contributes46 copied data entries and16
already-identical tags;33 old statistics files do not overwrite new counters.
No CMake tree or Ninja object tree was moved. No valid new cache is cleared.

Closeout source review catches the existing native-image importer's deliberate
build-owned input restriction. CMake now makes identity-preserving generated
prerequisites from the five verified release inputs instead of weakening that
guard or reading an installed Windows tree. The x64 reference-only graph
needs no such input copies. Focused import verification completes0 at03:07:02Z:
both configurations build, all five actual imports pass the original AMD64/
subsystem and SHA checks, and a second import is Ninja no-work. Receipt hash
8468BA4AAEC89361B49C0B38E7B824E69D2F3D899362B95BC8C0B997270604BF.
The formal build-hygiene test retains these assertions for future execution.

### Exact cleanup admission

The15 exact targets in the inventory table total247437 files and39452492718
logical bytes (36.742GiB). All18094 sensitive/unknown-extension entries have
individual hashes/reasons.10203 are exact canonical guest ZIP duplicates;
all318 configurations equal its SYSTEM.INI, hash
F20A94235712C26B0499D61E7CC080B612BBFE230DE0C65A6832494FDBB1ACAA.
The7 source archives contain51 files matching retained Git revisions after
CRLF checkout normalization only; no unique source/Git data is discarded.
The old guest ZIP is reachable blob7aef7060cc3cde2bf5976472ea9b4c00d59e9400.
Both LINK copies match retained tools/historical/msdos400/LINK.EXE, hash
124A3C800EDC16B60F696D35A9DFEC798B68B78FE2CA90C5988EA76BDEAB1A8A.
The manual's URL/hash remain in indexed T445 S1 research. The terminal draft
is already traced in S3's audit and superseded by the delivered terminal API.

The single tool's complete preview passes all15 with Applied=false and exact
membership digests. Its ownership manifest SHA-256 is
E82F632384A5FD0E6E34EFCBBF71F6C3EFBA8E0AC00C7D319A2DBE40F553386F;
preview3FB1CD202AB5399F86E667298D182AA8F35C992CEDE3B5C122A4D91745039CC5;
sensitive inventoryD45C2013CF803FCF2B051B1B1FE6E0A64170645F99A15929F97678AD908294BA;
provenance65A0D09434B5911EC97B056824F74901646F6A7289B40B10963C6C6041383E6F;
full build result4409B8EA2D56C6074850E7B5DBA68A85AF84CA59ADFEA59A9984FA26CFDCEA6A;
final quality54943F08E976F68B4BA0E8F7A510949D836FB6F8F709FDF6F0D24E3E7D8C1245.
These hashes and conclusions are retained before deletion, not reconstructed
after it. Actual process quiescence must be rechecked immediately before Apply.

### T447 checkpoint preservation

P13 remains3d1ece5b319df505ba471b34d7feab755487e944, main/origin synchronized
at admission. Its saved Git receipt hash is
84B4A0F524BFE9676591FD9168E1086FBCE4E23844DCC168ABB81CE906E22E9B;
final runtime manifest3C906AC1F4A9F2D92F9E67F0E35FBEAA7C68DF154ADE92E2EE7F114ADEC1049C.
The indexed S4 source audit retains source/test identities, success/failure
interpretations and delivery limits; raw build snapshots/logs are disposable
under this Td. Historical failures are not erased as conclusions or renamed
passes. M1--M6 obligations,0102/WRITE and earlier S3 risks remain open exactly
as recorded. Accepted runtime binaries stay in release assets/Git and the
installation. Future work reconfigures stable trees from tracked sources;
retired M/T/S run paths are no longer live build dependencies.

### Applied cleanup

At03:01:06Z, a fresh CIM snapshot inspects387 processes, finds no executable/
command-line owner of the15 targets and no active ccache ambiguity, and joins
the completed owned build/test receipts. No process is terminated for cleanup.
Snapshot hashDAA8130147EBA95F4553DE2895574606E64D35A7E27208CB89A7EA3DEC8148C4.
All targets then pass the single tool's preflight and final file/directory
metadata recheck before the first removal. Apply completes exit0 at03:06:02Z.
The15 listed targets,247437 files and39452492718 logical bytes are removed.
Volume free space changes261433110528->301213118464 bytes, an observed
39780007936-byte delta, not exclusive attribution against other disk activity.
Receipt hash7B961B02CF5183B26E047FB1127F50964ADE79D57BBB2CE06A11ABECD94AFB72.

At this checkpoint only the two caches and the current Td temporary root
remain. The latter is disposed only after its supervisors exit and the final
receipt/hash summary is recorded. Raw task logs/captures/failed candidates
are not retained or promised recoverable; canonical source/media and released
images remain in Git/assets/tools. No user-file exception is currently needed.

Final Git diff review finds one historically tracked generated artifact in
that retired output tree: build/output/ntvdm64-0237.exe,3137024 bytes, originally
committed by f0554100d. No current tool/test/design/status consumer names it.
Its blob36df93fbb94a11929ed805c90423d6a078f7f0a6 remains reachable in history;
an actual streamed Git-object read verifies SHA-256
1CC9DF57CFEF1296FB3512F9C391945582DEDBAA5577E4106A6AE2E21406A95D.
The Td deliberately retires this versioned build output from the current index,
not its history. It is not a test Git checkout or one of the current13 images.
Its bytes are already included in the cleanup total, not counted twice.

Final configuration-refresh verification completes0 at03:18:49Z: both real
CMake builds regenerate metadata and report Ninja no-work, with every existing
object timestamp unchanged.23 PowerShell files parse, document/link gates
pass, and all13 installed/release images plus the guest ZIP still match.
Closure-result SHA-256 is
6759ECE63D4BE286DF16FE6DA4E2061E355EF838A3D040065D92D0374301CF78.
Verified tools are CMake4.3.3, Ninja1.13.2 and ccache4.13.6 with MSVC19.43.
Source-owner selection changes require explicit reconfiguration; no general
compiler/SDK migration or every historical generator's runtime is claimed.
Earlier failed path/module tests and the stopped superseded owned inventory
are not passes; only the retained successful groups support the verdict.

### Change accounting

Final Git line counts (generated ignored output is accounted separately):

| Scope | Added lines | Removed lines | Net |
| --- | ---: | ---: | ---: |
| Scripts/configuration/tests | 606 | 113 | +493 |
| Documentation | 375 | 73 | +302 |

Seven files are added,28 modified, and the one obsolete binary above retired;
the binary deletion has no textual line count. All textual counts above are
the final Git diff; own-root disposal is confirmed below.

Before final own-root disposal, the same tool inventories194 files/12508896
bytes and55 sensitive generated-log/configuration rows; all belong to this Td,
not another task or user data. Exact membership digest is
9639EA7298EDC49CEF39F6508DACD1F05D3AC828D06B0A0C593B1466C4C6C0CB.
The reviewed inline manifest is applied after all owned supervisors exit,
with no active root reference. This summary precedes deletion. With the old15,
the admitted removal totals247631 files/39465001614 logical bytes (36.75GiB),
plus the separate six-byte configuration-test prefix. Final residual inspection
confirms only build/.ccache and build/.ninja, with no retained user exception.

### Limits and delivery disposition

No full cold13-image rebuild, real device/guest sweep, user-desktop interaction
or new publication is claimed: product sources/guest/images do not change.
The custom worker/WOW/native/Hook targets retain original manifest composition;
the actual exercised builds are the named dual-width style and selected
libraries/fixture and five image-import targets, not every dormant target. Retired historical generators
are reference tooling, not another supported task-copy cache policy.
The cleanup attestation is not an automatic process detector or a global
filesystem lock; callers must really quiesce target owners. Safe preflight
does not promise race-proof deletion against a hostile concurrent writer.

Final metadata verification, own-root disposal and the containing reviewed
main/origin commit form this standalone Td delivery; owner acceptance remains
required. S4 does not resume automatically and receives no new P or closure.
