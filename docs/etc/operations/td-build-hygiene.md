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

Owner supplement: continue this Td with all13 source-component builds and a
second unchanged build, without clearing valid caches or running Git prune.
Use tests/build/Test-FullComponentBuild.ps1 in an owned hidden background job;
build/td-component-build belongs only to this supplement (40-minute total,
25-minute per command,15-minute no-output and64MiB log budgets). Preserve
component PE identities/hashes and intermediate timestamps before disposing
the logs through the existing cleanup tool. Runtime publication/device tests
remain excluded unless the build uncovers a runtime-affecting change.
The previous partial builds alone do not close this supplementary requirement.

The initial delivery did not exercise all13 source-component targets; the
supplement below supersedes that limited build denominator. No cache-cleared
cold-build, real device/guest sweep, user-desktop interaction or new runtime
publication is claimed. Retired historical generators are reference tooling,
not another supported task-copy cache policy.
The cleanup attestation is not an automatic process detector or a global
filesystem lock; callers must really quiesce target owners. Safe preflight
does not promise race-proof deletion against a hostile concurrent writer.

Final metadata verification, own-root disposal and the containing reviewed
main/origin commit form this standalone Td delivery; owner acceptance remains
required. S4 does not resume automatically and receives no new P or closure.

## Owner-requested full-component supplement

Source baseline f2af83fae156cd9ec9a3db8f71ade74ddf8ba2ea. Reproducible entry:
tests/build/Test-FullComponentBuild.ps1, launched hidden with pwsh -NoProfile
-ExecutionPolicy Bypass -File and -OutputRoot build/td-component-build/complete.
The entry configures both existing Release/ccache presets, builds W16Style and
wow32 (including worker), then the original x86 nthook32.dll/VDMREDIR.dll
targets and all six native x64 worker/frontend/monitor/launcher/service/Hook
targets. No guest target or imported published worker substitutes for these
source builds. All products remain inside the two existing stable trees.

The first restricted PID5072 exits without steps/results: not a pass. Actual
owned background PID11136 starts03:49:49.874Z and fails03:53:20.619Z: CMake
requested WOW32.DLL but the generated Ninja target is case-sensitive wow32.dll.
NTVDM had linked; WOW32 compilation had not begun. Fix only the CMake target
spelling. Failed-result hash
C5875D1D3C24CB7146D7A5F0A5F611E868CB6A70032A70A47E99228DCB8FE80C.

PID35600 starts03:53:58.303Z and fails03:56:44.385Z after successful WOW32
compilation/link. VDMREDIR's existing thread_start_compat object now consumes
session_thread_finish, added by the earlier S2 terminal-intent repair, but the
worker's generated private export list still names only current/bind/unbind.
LINK reports LNK2019/LNK1120. Failed-result hash
962C78CD4DF5A70802764711BEC53F44D156A907A487DFB53D43EECF21D30A0F.
Add the existing finish symbol to that same private export list and make the
generated definition an implicit worker-link dependency. Do not link another
session/TLS implementation into the DLL, alias finish to ordinary unbind,
invent a stub or weaken the terminal contract. This is one extra generated
private export, not a new function/signature, broker ABI or runtime algorithm.
It changes future build wiring; the installed/released P13 images stay intact.
Original host-mirror/guest differences added or removed by this supplement:0.

The final unchanged-input run is PID38496, started03:58:52.463Z; its actual
test starts03:58:53.267Z and passes04:03:19.922Z (UTC2026-10-19). All nine
subcommands exit0, with their exact command/stdout/stderr in the saved step
receipt before disposal:

| Step | PID | Seconds | Exit |
| --- | ---: | ---: | ---: |
| configure-x86 | 46340 | 35.173 | 0 |
| configure-x64 | 34092 | 44.225 | 0 |
| first-x86 | 3656 | 15.041 | 0 |
| first-x86-hook-redir | 28020 | 11.019 | 0 |
| first-x64 | 11320 | 97.087 | 0 |
| redirector-worker-import | 4108 | 5.800 | 0 |
| second-x86 | 46564 | 11.518 | 0 |
| second-x86-hook-redir | 31832 | 9.903 | 0 |
| second-x64 | 36368 | 34.720 | 0 |

The dumpbin /imports:ntvdm.exe check proves VDMREDIR imports the existing
session_thread_finish from its worker. All13 final PEs have their required
I386/AMD64 machine. The second whole build produces respectively2/1/6 nested
Ninja no-work results. All1197 objects/libraries/resources retain exact path,
length and UTC write timestamp; all13 final images retain SHA-256 and timestamp.
No compilation, resource build or relink occurs. The timings include repeated
developer-environment/runner startup; no-work does not mean zero wall time.
Existing valid objects and ccache entries survive every run and both fixes.

Final build identities (build-only candidates, not a new runtime release):

| Image | SHA-256 |
| --- | --- |
| ntvdm.exe | F62686328FB25AC8CF6C84FA6D12EE3AA6042A28303A483AEC36D56E30AE0A0A |
| WOW32.DLL | 6E1F504E68308C6A09449A5B149805E6DB98B688FD50D684BA6519E895AFEC72 |
| VDMREDIR.DLL | F0E712AFFFDBA75CC9E40161FA603B25795C8882AD1F55147A23209B6A970DD8 |
| nthook32.dll | 3EBE9D17FC585632FEAC7A1D837FC3B37995855440C4EDDFBAAC079F1F505000 |
| ntw16s32.dll | A7BDB9724224764F82EC512659FD48E9883B2950007F2EA3E27E1A0E42695218 |
| ntw16s64.dll | 3D0B171D8E064FD3120B4183C7FD65AF294276A4A8084F8C9131D625E83BA847 |
| W16Style.exe | F41F787BEC8E355514A64F84C9365AE3169313536AD4DD590D0DE19CD9DEF0BD |
| ntvwm.exe | 46BD6CD50B776C3AC8F3CD9FB93CB820BB3E298C5F8AACEB743752A243FF24F3 |
| ntcon.exe | 107A195D5F7A4B30A213CA72815636BADC15F567A9BC4038B47A5294DCFC5181 |
| ntmon.exe | D496BCA37FD0F13A55A6706FBEBFCAA71A2CF53A1F095DCB62B9342F7267EF8E |
| run16.exe | DBEA1548702A464BCEC0F24AC3EEDF6A75C0CCFE5058A85B2966C4806FDAF589 |
| ntsrv.exe | DEAF2EB9D62AC8044B5260A184E31C808FC495F58A84C3022FE4BDB29C1E829C |
| nthook64.dll | BE3C8100035990A465A0BADD0249392D85B0F1C3B493817548417BA56ECEEDAE |

Result hash4CD10ABA844FEDB603B6813DD1642D781D1EDDE379ED3DBA3BFEC7854AC174DD;
steps13F700E35BAA19A7ABBEFD4AA864D9DEE5CD042F379A7571B04AEB7AB22A29CA;
import proofEC4ADBEE7465756DB0A305718C7083B3585D3F1FF446FE7195C4D892B988C48C.
Second-build log hashes in the table's order:
3E26677CD854ABDAC19F9F37E5F6FE1C1517DD9F3FE49FB5A5897291F5A55F69,
A9DB993D5E263BDCAC0CCC0EE952EAA438E3B0DD366BBAF508A1085FCD529960,
5A0EF8C51730CFDE72363624B95BA9643803856B7DEA778B07D4644808E6F04C.
Test-source hash3AEEF313ED5891916DF2540E0A1354066BDF459C8C32B646F25BF514EDFBF41B;
CMakeA482C4B9268B1F7B1CBB01226D2F14A4A789D2D59AABBE2BA32194EEC9E64A2B;
generator292D93C2FC7ADA3CA3DE2388FA797000E9CEEC1C55F6243DCC92EE7A7AD377A0.
The earlier failed logs are retained as failures until disposal, not overwritten
or counted as successful iterations. No Git prune or cache-clearing command ran.

Background final-review PID25680 starts04:05:09.271Z with a180-second deadline
and completes04:05:40.523Z exit0: documentation governance/relative links,
diff whitespace, both changed PowerShell parses, the13 installed/release
manifest hashes and immutable guest ZIP all pass. Explicit repository-cache
statistics show1261 cacheable calls,234 direct hits and1027 misses, limit5G;
these cumulative counts include initial compilation, not a promised hit ratio.

The completed supplement owns66 disposable files/6716413 bytes (6.41MiB),
all generated command/log/JSON evidence, with no media, configuration, Git or
user-data exception. The real cleanup tool's default preview passes with
Applied=false; exact membership digest before disposal is
EB1E4C618BC9BB71F305906E0A32BA3C4557DFA717462DD1F0A5BEB22D7DFB9F.
The above result, failure, import and incremental summaries precede disposal.
Recheck actual process quiescence before Apply; do not infer it from a switch.

At04:06:17.852Z, all owned supervisors/commands have exited and a fresh CIM
inspection finds no active build executable or disposable-root user (excluding
the read-only inspection command itself). Apply passes with the same digest,
removes exactly those66 files/6716413 bytes, and final CheckResidual reports
only build/.ccache and build/.ninja. No process is killed for cleanup and no
valid cache is deleted. Only generated disposable evidence is removed; its
hashes/conclusions above and reusable117-line test remain tracked.

Supplement change accounting: scripts/config/test120 added,2 removed,net118
(one new test, one CMake target correction and the private-export/dependency
wiring). Documentation138 added,6 removed,net132; five changed/added files.
No production-source, guest or release-asset file changes. No device/runtime
acceptance is claimed for the new build outputs, which are not deployed.
This completes the requested all-component/incremental build scope; reviewed
commit/push and final clean-state verification deliver the Td supplement for
owner acceptance. T447 S4 stays held at P13 with its unchanged obligations.

## Owner acceptance and closure

Owner accepts the supplement and explicitly closes this standalone Td after
aadde21e8f774068caee64663101823faa07f615 (main/origin synchronized, clean).
Both the initial f2af83fae and supplementary all13/1197 build evidence remain
above, with failed attempts and limitations preserved. No cache-clearing,
Git prune, runtime re-publication or guest change follows from closure.
T447 S4 resumes at its delivered P13 source/runtime frontier under the same
fixed M1--M6 obligations. S5/S6 and Queue are unchanged; performance stays
frozen. This acceptance closes tooling work, not those runtime obligations.
