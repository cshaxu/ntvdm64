# T484 CMD-only build and release plan

## Authority

Owner admission on 2026-11-22 requires CMD-only build scripting, removing
Python, PowerShell and JavaScript. [Current](../../states/CURRENT.md) owns
the only active packet. Existing scripted generators cannot remain unchanged
behind a new CMD entry. Product baseline is 9e7eb19c0/P85, package
100b16d9-992d-4e2c-8d99-66a4e891dd26; T483 closure is e74afba09.
The workspace was clean at admission; this delivery changes documents only.

## Sequential work

| Stage | Scope | Completion witness |
| --- | --- | --- |
| S1 (research complete) | DIR-ordered build/generation/publication inventory, actual callers/outputs, architectures, toolchain bootstrap and migration design. | [Inventory/design](../evidence/t484-s1-build-inventory.md);355 migration candidates are not355 proved live calls. |
| S2 (complete) | CMD entry and host configuration/build/code-generation chain. | All eleven host images; preserved ABI/source selections/cache invalidation; no hidden forbidden interpreter. |
| S3 (complete) | Authored addon producers and dependent test-binary compilation; shared setup consolidation. | Every addon output accounted for; no original guest rebuild or test binary in release. |
| S4 (complete) | CMD staging, manifests, hashes and publication; retire migrated wrappers and repair callers. | Complete host/addon sets, nonzero failures and no mixed package reported successful. |
| S5 (complete) | Consolidate native preparation tools: reuse PIF.EXE, unify INI editing, remove superseded producers and build targets. | Fewer duplicate implementations/executables; callers and basic coverage preserved; post-cleanup CMD-only build/publication evidence and reviewed delivery. |
| S6 (complete) | Clean/cached end-to-end validation and migration acceptance. | Clean bootstrap, no-op/changed-input rebuilds, outside-repo invocation, prerequisite/component/publication failure cases, relevant regression, verified publication and pushed delivery. |
| S7 (active) | Release captured mouse before a Window-mode fault decision dialog. | Ignore/Terminate dialogs are mouse-accessible; capture is released before display; focused lifecycle verification and normal product delivery complete. |

Later stages are planned, not simultaneously active. Move complete producers
and callers together. Preserve existing source-owner inventories and actual
dependency edges; do not create a parallel build implementation or substitute
unrelated historical output for a required current producer.

## Rules and acceptance

- CMD is the sole project build-script interpreter. Trace indirect calls,
  configure hooks, generated commands and post-link/publication actions.
  No Python/pwsh/powershell/node/cscript/wscript escape hatch.
- Native compiler/linker/assembler/resource/IDL tools and build engines remain
  legitimate tools. Preserve declarative dependency metadata; do not hide
  arbitrary orchestration in another tool's script mode.
- Preserve code generators' real input/output contract. Remove unnecessary
  generation or use explicit templates only with evidence. A need for a new
  compiled helper is a design decision, not a preapproved interpreter substitute.
- Cover all eleven host images and every independently authored addon output.
  Reconcile actual sources/producers/consumers/manifests, not README guesses.
- Standalone test/audit/governance runners are outside automatic language
  conversion. Compilation steps they invoke are in scope; they cannot remain
  mandatory interpreters for final configuration, generation, staging or publication.
- Keep stable caches and input invalidation, original mirror bytes, LF for
  authored text, current architectures/exports and immutable guest/media.
- Default publication is assets/release. Installed O:/winnt deployment stays
  an explicit selected operation; changed build batches follow existing
  product verification/publication rules.
- Final proof includes a clean environment without forbidden interpreters,
  actual child-command evidence and complete verified host/addon manifests.
  An old cached graph passing is insufficient.

## Retention review including delivered migrations

Owner clarification applies retrospectively to every already migrated test
builder, diagnostic tool and probe as well as remaining candidates. Migration
is not a reason to retain an obsolete tool. Review current callers and unique
maintained behavior in DIR order; remove expired unused tools and their native
targets/references, consolidate duplicate setup and assertions into an existing
owner, and retain only justified current coverage. Historical receipts remain
evidence, not active build dependencies. Lack of a suite caller alone does not
prove a focused regression obsolete; record its current contract and provider.
Use the existing disposition ledger for decisions and evidence, not a second
migration inventory. Track review separately from delivered migration counts.

## Initial evidence and limits

At e74afba09, tools/build.cmd does not exist. CMakeLists.txt:41-42 requires
pwsh/node; ntvdm_generate invokes PowerShell. Worker, WOW32, native generators
and GenerateCvidcCpuBinding.mjs are directly referenced. This is preliminary
evidence, not the complete S1 inventory, implementation or a clean-build pass.

S1 records each file's direct/transitive callers, real outputs/architecture,
generation owner, cache inputs, replacement, retirement prerequisites and
validation. No product source or published asset changes at admission.

## S5: consolidate native preparation tools

Owner direction on 2026-11-24: prioritize reuse of the existing PIF utility,
merge INI handling, and do not replace scattered scripts with scattered C
programs. Owner reordered the stages: complete this cleanup as S5 before
S6 final migration acceptance. The mouse-dialog work remains S7.
S4 closed with [P46](../evidence/t484-s4-matrix-preparation.md).
S5 closed with the [shared INI editor](../evidence/t484-s5-shared-ini.md), PIF consolidation and final package verification. S6 closed with the [clean build and live acceptance record](../evidence/t484-s6-clean-cmd-acceptance.md). S7 is active.

### Scope and execution order

1. Inventory current preparation producers and their real callers in DIR
   order, including already migrated and newly added tools. Separate format
   generation/configuration from runtime probes and compiler generators.
   Reuse the current disposition ledger; record tool, output format, unique
   semantics, callers and final owner. Retire obsolete inputs before migrating.
2. Make released PIF.EXE the normal PIF construction/update owner. It already
   exposes create/update, memory limits, W386 and NT31 flags, paths and
   arguments. Audit ems_pif_builder, pure_dos_pif_builder, mixed_batch_pif,
   xms_profile_builder and s45_keyboard_profile_builder against those fields.
   Express test scenarios as CMD arguments or simple configuration data, not
   separate C executables or test-name switches in the product utility.
   Add only demonstrated missing format/field operations to that same owner.
   Existing create emits standard/W386/NT31 extensions: do not silently replace
   a deliberately standard-only or malformed-format fixture with this layout.
   Such retained negative-fixture construction needs explicit unique-coverage
   evidence and should remain test-local, not another general PIF builder.
3. Merge wow_network_profile and guest_driver_profile INI parsing/writing
   into one byte-preserving INI owner, including any equivalents discovered
   during S4. Prefer one reusable INI utility (INI.EXE if no existing suitable
   owner), with one parser/writer and literal section/key/value operations.
   Keep Win31/Win95/test scenario selection and driver copying in CMD; do not
   add a separate executable per profile or extend PATH95 into an unrelated
   general configuration engine. Preserve unrelated encoding, comments, blank
   lines and line endings; explicitly handle duplicate device entries, unique
   sections, enable/disable behavior and validation-before-replacement.
   Do not edit SYSTEM.INI through an API path that writes host registry
   mappings. No embedded scripting, arbitrary command execution or generic
   workflow/configuration language behind the CMD entry.
4. CONFIG.NT/AUTOEXEC.NT and other simple fixed text use shared CMD templates
   or existing preparation entries; they do not justify another C generator.
   Keep PIF, GRP and PATH95 responsible for their existing formats. Preserve
   distinct binary-binding/compiler generators only where actual input/output
   contracts justify them; do not merge unrelated algorithms merely to reduce
   executable count. win31_probe_paths requires its own binding-contract review.
5. Migrate all live callers together, then delete superseded C files, executable
   targets, wrappers, stale references and redundant tests. Preserve independent
   assertions of output semantics so using the same writer in production and
   tests does not turn verification into a self-comparison. Keep basic positive,
   negative and cleanup coverage without multiplying test frameworks.

### Acceptance and reporting

- Report before/after executable count, parser/writer implementations, source
  lines, build targets and removed callers/wrappers. Do not equate moving lines
  or changing language with lower maintenance cost. Every retained dedicated
  preparation tool needs a concrete irreducible contract.
- Compare generated PIF fields/layout and INI changes against established
  callers, including repeated device entries, missing/ambiguous sections,
  encoding/line endings, stale candidates, failure rollback and owned cleanup.
  Use representative real preparation and relevant runtime tests; no original
  guest modification/recompilation and no unapproved archive changes.
- Deliver under current rules: cached full build, coherent publication,
  relevant tests, commit and push. Addon tools publish through assets/release;
  test-only binaries remain outside production releases.
- S6 final acceptance follows this cleanup and validates clean/no-op/changed-
  input builds, actual interpreter-free commands and publication against the
  S5 final code. Do not use pre-cleanup receipts to prove removed or changed
  producers. S7/T closure requires current evidence for the final build chain.

## S7: release captured mouse before fault dialogs

Owner addition on 2026-11-24: when the user is in Window mode with the mouse
captured, release that capture before displaying a fault prompt requiring
user action, such as Ignore/Terminate. This remains the final stage after S5 cleanup and S6 acceptance, not an
admission to run alongside active S4. Overall T closure follows S7 delivery.

Scope and implementation requirements:

- Trace the actual fault-dialog and Window mouse-capture owners before editing.
  Use the existing release path, covering both capture and cursor confinement
  where they are part of that path. Do not duplicate mouse state or add a
  parallel state machine, polling loop or same-thread queue.
- Complete release before the dialog is shown, so the user can reach and click
  its controls. Do not rely on the dialog eventually taking focus to release it.
- Keep capture released while the prompt is active. Ignore returns through the
  existing input/capture policy; Terminate must not recapture during teardown.
- Include equivalent actionable fault prompts sharing this mechanism. Preserve
  Console behavior and Window behavior when capture is already inactive.
- Preserve error reporting, button decisions and guest execution semantics;
  do not modify or rebuild original guest code.

Acceptance: reproduce Window mode with capture active, trigger a controlled
fault prompt and verify release precedes display and mouse selection works.
Exercise Ignore, Terminate, repeated prompts, and the already-uncaptured case;
verify normal input after continuing and cleanup after termination. Record the
actual provider/thread boundary and reuse existing probes rather than creating
an overlapping test framework. Deliver under the then-current project rules:
cached build, coherent publication, relevant tests, commit and push, with
publication identities and results recorded before S7/T closure.
