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
| S1 (active) | DIR-ordered build/generation/publication inventory, actual callers/outputs, architectures, toolchain bootstrap and migration design. | Finite per-file dispositions and dependency order; no deletion by extension. |
| S2 | CMD entry and host configuration/build/code-generation chain. | All eleven host images; preserved ABI/source selections/cache invalidation; no hidden forbidden interpreter. |
| S3 | Authored addon producers and dependent test-binary compilation; shared setup consolidation. | Every addon output accounted for; no original guest rebuild or test binary in release. |
| S4 | CMD staging, manifests, hashes and publication; retire migrated wrappers and repair callers. | Complete host/addon sets, nonzero failures and no mixed package reported successful. |
| S5 | Clean/cached end-to-end validation and closure. | Clean bootstrap, no-op/changed-input rebuilds, outside-repo invocation, prerequisite/component/publication failure cases, relevant regression, verified publication and pushed delivery. |

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

## Initial evidence and limits

At e74afba09, tools/build.cmd does not exist. CMakeLists.txt:41-42 requires
pwsh/node; ntvdm_generate invokes PowerShell. Worker, WOW32, native generators
and GenerateCvidcCpuBinding.mjs are directly referenced. This is preliminary
evidence, not the complete S1 inventory, implementation or a clean-build pass.

S1 records each file's direct/transitive callers, real outputs/architecture,
generation owner, cache inputs, replacement, retirement prerequisites and
validation. No product source or published asset changes at admission.
