# Proposal — Rename the native-text worker and Console frontend

## Status and objective

Owner closed the predecessor and admitted this former queue-head candidate on
2026-10-02. The active numeric packet belongs only to
[CURRENT](../states/CURRENT.md); the ordered execution stages are in the
[working plan](../etc/operations/t424-worker-frontend-renaming-plan.md).
This document retains the admitted migration scope and acceptance contract.
S1 is a read-only referent audit; admission alone does not authorize skipping
that audit or replacing the published package with renamed candidates.

Give each product component a name that describes its actual owner:

| Current component and role | Final component and product | Boundary |
| --- | --- | --- |
| `src/ntcon-exe` / `ntcon.exe`, resident Win32 text worker | `src/ntw32-exe` / `ntw32.exe` | Owns the hidden Windows Console and direct native text targets; peer of NTVDM. |
| `src/ntkvm-exe` / `ntkvm.exe`, visible frontend | `src/ntcon-exe` / `ntcon.exe` | Owns the visible Console, Window, display state, input and presentation routing; not a worker. |

The final eight-file package contains `ntmon.exe`, `run16.exe`, `ntsrv.exe`,
`ntvdm.exe`, `ntw32.exe`, `ntcon.exe`, `WOW32.DLL` and `VDMREDIR.DLL`. Neither
the name `NTCON` nor a `ntcon_*` product-owned symbol may continue to denote
the Win32 text worker. Generic imported `kvm-*` library names remain their
library identity unless a separate source/provenance review establishes that
they are product component names; this task does not rename an external API
merely because its spelling contains KVM.

The worker rename also includes its **project-owned NTSRV task-record name**:

| Current project-owned spelling | Final spelling and meaning |
| --- | --- |
| `OPENNT_BASE_CONRECORD` / `ConRecord` | `OPENNT_BASE_WIN32RECORD` / `Win32Record`: one admitted Direct Win32 text request, peer in naming to the original `DOSRECORD`. |
| `conrecords`, `pending_conrecord`, `next_conrecord`, `service_delete_conrecord` and analogous members/helpers | Corresponding `win32records`/`pending_win32record`/`next_win32record`/`service_delete_win32record` spellings, consistently through NTSRV, tests and documentation. |

The current `OPENNT_BASE_CONRECORD` in `src/ntsrv-exe/opennt/source/base_service.c`
is explicitly project-owned, so this is not an excuse to rename or alter the
original OpenNT `DOSRECORD`, `WOWRECORD` or Console identity/association
structures. Keep the record's request, target PID/handle, receipt, image,
list order, Direct completion and management projection unchanged. Do not
turn the rename into a new observed-task stack or execution policy.

## Ordered migration and collision prevention

1. Freeze a reviewed inventory of the **old** identities and their referents:
   executable/component directories, C symbols and types, macros, build targets
   and object paths, test names, package manifests, launch paths, service/RPC
   records (including the project-owned `ConRecord` family), log/diagnostic
   names, and current design/rule/proposal text. Mark
   each occurrence as native-worker, frontend, generic library, historical
   evidence, or external compatibility surface. Record the baseline eight-file
   hashes and active protocol revision before editing.
2. First rename the **old worker** `NTCON` to `NTW32`, including its directory,
   executable, every product-owned symbol including `ConRecord` →
   `Win32Record`, service identity, caller, generated
   reference, test and documentation reference. Historical documentation is
   included: rewrite each old-worker `NTCON` reference as `NTW32`, even in
   retained evidence, archived documentation and proposal text, and rename
   documentary paths where necessary. Preserve the original pre-migration
   documents in Git history
   for exact as-recorded reproduction; record the migration commit and avoid
   changing recorded test outcomes, hashes or dates. Keep the frontend named
   `NTKVM` during this local stage. Compile and run focused
   worker/launcher/broker/protocol checks. **Hard gate:** an exhaustive,
   case-insensitive scan of tracked text and path names throughout the repository,
   plus untracked current source/documentation files, must find no `NTCON`
   spelling at all (including this proposal) before frontend rename is
   admitted. Include `docs/` and the documentation archive under `artifacts/`;
   do not let a narrow `src/` scan claim success. Classify generated/build
   outputs separately and regenerate
   them before the final acceptance scan. Do not satisfy the gate by hiding a
   reference in an alias, comment or unscanned path. Verify and publish a
   coherent intermediate eight-file package with `ntw32.exe` and `ntkvm.exe`,
   without the old `ntcon.exe`, before calling this production S delivered;
   never publish an untested mixture of old and new files.
3. Only after the zero-occurrence gate, rename the **old frontend** `NTKVM` to `NTCON`,
   including its directory, executable, product-owned symbols and all callers.
   Reconnect the newly named `NTCON` frontend to both `NTVDM` and `NTW32` via
   the existing interface and worker-base boundaries. Do not implement a new
   frontend or worker mechanism as a side effect of renaming.
4. Audit the final `NTCON`/`ntcon`/`NTKVM`/`ntkvm` occurrences by referent,
   not blind textual replacement. Every final `NTCON` occurrence must mean the
   frontend; every Win32 text worker occurrence must use `NTW32`. Historical
   document text is normalized to the new worker name as required above; Git
   preserves its prior literal form. Current authorities, live proposals,
   package instructions, operational scripts and indexed evidence must use
   final names. No intentionally retained alias may give the old worker the
   new frontend name; an externally fixed binary identifier requires the
   explicit stop/review decision below.
5. Review every cross-process identity separately. If a copied protocol value,
   RPC interface, product version, ACL endpoint or command-line contract
   actually changes, version both ends coherently and reject mismatched peers.
   Keep stable wire semantics when only local C names change; do not bump or
   mutate an ABI solely for spelling. No old/new mixed runtime set may register
   as a valid coherent product.

Moves use `git mv` where appropriate and preserve source ownership. Original
OpenNT/MVDM mirror code, guest media, task completion, worker lifecycle,
window/console behavior and the generic imported KVM libraries are not
reimplemented or semantically modified. No additional executable or helper
is admitted.

## Suggested S sequence after T admission

| S | Bounded deliverable |
| --- | --- |
| S1 | Read-only referent inventory, naming/ABI decision ledger, baseline hashes and ordered migration map. No production rename. |
| S2 | Rename the old worker to NTW32 and its project-owned ConRecord family to Win32Record in code and **all** documentation; run the tracked-tree case-insensitive zero-`NTCON` and old-ConRecord referent gates, full production-P regression and publish the coherent intermediate package (`ntw32.exe` + `ntkvm.exe`). Do not admit S3 until the gates and delivery pass. |
| S3 | Rename the old frontend to NTCON only after S2's gate; update build, package, tests and launch wiring, run the full product gate and publish only the final verified eight-file set. |
| S4 | Independent final referent/semantic-diff audit: every current `NTCON` means frontend, every native worker means `NTW32`; reconcile indexes and authorities, deliver governance and clean-worktree closure. |

If S1 finds an externally fixed identifier that cannot safely be renamed,
record its exact consumer and obtain owner review before S2 rather than
silently treating the old worker as the new frontend. S2 and S3 each obey the
ordinary production-code P build, full regression, coherent publication,
commit and push gate. S2's intermediate package is a complete eight-file set,
not an old/new mixture; S3 replaces it only after its own verification.
Documentation-only P commits use the documentation gate.

## Acceptance

- Build the complete MSVC Win32/x86 package from a cleanly regenerated graph;
  verify no build target, generated dependency or deployment script resolves
  the old product roles by their previous names.
- Exercise direct and nested COMMAND/MEM/EDIT and native CMD/EDIT, DOS↔native
  return, Console/Window switching, resident worker reuse, faults and
  independent sessions. Preserve the current 17 Console + 17 Window product
  matrix and the retained WINMINE/SOL/WRITE frontiers.
- Prove `run16`, `ntsrv`, `ntmon`, `ntvdm`, `NTW32` and the new `NTCON` agree
  on worker kind, process identity, frontend ownership, receipt and teardown.
  Monitor must identify the Win32 worker without mistaking the frontend for a
  task or worker.
- Prove the new `Win32Record` remains exactly the former Direct native record:
  one actual admitted request, unchanged receipt/exit-code and worker-stack
  behavior, no observed descendant as a record element. No product-owned
  `ConRecord` spelling or old-worker referent remains after S2; the original
  DOS/WOW and Console identity records are byte/semantics unchanged.
- Scan source, tests, tools, all current and historical documentation, and
  package outputs. The S2 scan has zero old-worker-name occurrences; after S3
  every occurrence of its reused name denotes only the frontend. Compare
  behavior and protocol with the pre-rename
  package; a rename is not permission to lower existing functional assertions.
- Publish one verified coherent final-name package to `O:/winnt`, retain a
  recoverable prior package, record all eight hashes, commit/push reviewed
  changes, and leave the active T open for owner acceptance if required.
