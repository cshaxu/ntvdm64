# T484 S1 build migration inventory

## Question and inputs

At admission revision ec5b074859bc091ec685d81a7a8603ed416c3cea, identify
the actual host/addon producer chain and a finite caller worklist before
replacing its interpreters. This is build-system research, not a product
build or runtime acceptance result. Original guest inputs remain immutable.

The retained [inventory](../../../artifacts/build-migration/t484/inventory.json),
[DIR-ordered files](../../../artifacts/build-migration/t484/files.csv),
[literal references](../../../artifacts/build-migration/t484/references.csv),
[command candidates](../../../artifacts/build-migration/t484/commands.csv) and
[existing graph observations](../../../artifacts/build-migration/t484/existing-graphs.json)
separate lexical discovery from actual generator review. Ordering is full
repository-relative path, case-insensitive; duplicate basenames are not identities.

## Results and limits

- Scanned817 tracked script/metadata files under tools, test, src/addon and
  the root CMake entry/presets. Selected355 candidate files,1116 literal
  reference occurrences and1252 command-related lines.
- The355 comprise2 root configuration files,113 files under tools/build,
  11 addon producers and229 test/audit compilation candidates. A producer
  may emit several products; these counts are not binary counts.
- Assigned14 candidates to host configuration/generation,327 to addon/test
  producer migration and14 to publication/cleanup. Every row has a proposed
  disposition. These are migration assignments, not355 verified live callers.
- Inspected9 existing host Ninja graphs;21 rules still invoke forbidden
  interpreters. This excludes configure-time calls, in-process hashing and
  standalone callers;21 is not the total interpreter-dependency count.
- Test/audit hits may be comments, optional diagnostics or actual compilation.
  Each must be resolved when its producer/callers migrate. Absence of a literal
  caller does not authorize deletion. No claim all355 files were manually
  execution-traced, and no candidate has been marked migrated.

The task-local scan ran in owned background pwsh PID28644, from
2026-11-22T15:50:43Z to15:51:19Z, exit0. It performs static inventory only;
its Python implementation is not a build input. Existing graph inspection
does not regenerate or compile anything and is not clean-build evidence.

## Reviewed host chain

1. CMakeLists.txt requires pwsh and node, calls
   New-T310OriginalSoftpcNinja.ps1 at configure time and requires its generated
   source files before compiling. The generator selects original SOURCES
   manifests, explicit authored groups, per-source flags, exports, firmware
   resources, MIDL and product/test library edges.
2. New-NativeWorkerNinja.ps1 parses that x86 graph to derive five x64 closures.
   It filters original source participation, substitutes HDE64 and applies
   native per-component flags. Those selections must survive migration.
   New-NativeHook64Ninja.ps1 owns the separate Hook64 closure.
3. The x86 configure currently copies five released AMD64 images from
   assets/release into native-inputs. Those copies are not clean-build producer
   evidence. Native images have no runtime dependency on a newly linked x86
   worker; shared declarative source metadata can remove this bootstrap cycle.
4. WOW32 generation waits for ntvdm.lib. Its source selection is72 original
   bodies plus current support groups. Preserve the named NTVDM import ABI,
   ordinals, x86 calling conventions and strict A/B compilation boundaries.
5. The worker post-link rule calls Verify-VdmTibStorage.mjs. Removing Node
   must preserve that storage assertion, not silently drop the check.
6. P85 publication uses Stage-System32ProductPackage.ps1 plus a task-local
   publication script. Set-PackageIdentity.ps1 compiles C# to call Win32
   resource APIs. All eleven images must retain one nonempty package GUID,
   machine checks and post-copy hash verification.

## Output coverage

| Architecture | Current host outputs |
| --- | --- |
| x86 | ntvdm.exe, WOW32.DLL, VDMREDIR.DLL, VCDEX.DLL, nthook32.dll |
| x64 | run16.exe, ntsrv.exe, ntcon.exe, ntvwm.exe, ntmon.exe, nthook64.dll |

The addon manifest and actual source tree agree on14 independently authored
outputs:9 AMD64 utilities (HASH, GRP, LNK, SZDD, patch31, path31, patch95,
path95, PIF), MOUSE101.DRV, MOUSE31.DRV, compat95.sys, insignia.386 and
ntvdm64.386. common is headers, not a fifteenth output. Existing nine utility
BUILD.CMD files already invoke MSVC directly; assembly producers still use
PowerShell, and the mouse producers unnecessarily combine production assembly
with test assembly and Python assertions. Split production from tests without
removing those assertions from their applicable test suites.

## Implementation design

CMD owns entry, toolchain setup, selected architecture builds, failure
propagation, staging, manifest generation and explicit publication destinations.
CMake/Ninja retain native declarative dependency scheduling. Replace the
PowerShell graph generators with canonical source groups and target properties;
do not freeze a generated graph or maintain duplicate handwritten selections.
Original SOURCES lists remain input authorities; authored selections move once
from old generators to the new metadata. Compare expanded source/flag/link
inventories before retiring each old generator. Retain current stable caches.

Native source generation is necessary for binary font extraction and the
selected CPU/SAS metadata projections. Use narrowly scoped compiled data
transformers if native build/resource facilities cannot express the existing
contract. They may not launch processes, interpret recipes or replace CMD
orchestration. Preserve count/marker checks and write-only-if-changed behavior;
do not check in frozen output as a substitute for regenerating from its owners.
The concrete implementation and equivalence results remain S2 work.

Package resource mutation similarly requires a Win32 resource operation, not
an arbitrary scripting interpreter. A bounded compiled resource helper can
replace the existing embedded C#; hashing should reuse the existing authored
HASH utility or native build tool facilities. No new product runtime component
or installed helper is needed. Preserve manifest acceptance descriptions:
rebuilding an addon does not promote its candidate status to verified.

Staging must start from explicit source/approved immutable assets, not a prior
task's disposable candidate directory. Publish only after all required outputs
validate; retain exact rollback ownership and never report a mixed package as
successful. assets/winnt.zip is not modified.

## Required successor evidence

S2 proves host configuration from source without forbidden interpreters and
compares actual compile/link identities. S3 resolves every producer/caller
candidate and all14 addons, separating assertions from compilation. S4 proves
coherent stage, manifests and rollback. S5 proves clean/cached builds,
changed-input invalidation, outside-repository invocation, missing tools,
component failure and publication failure, followed by applicable regression,
dual publication and clean committed/pushed closure. Current records actual
stage admission; this document does not itself admit a later stage.
