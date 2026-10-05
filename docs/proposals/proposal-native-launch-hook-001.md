# Proposal — Controlled native launch compatibility hooks

## Status and dependency

Owner subsequently accepts the delivered32-bit Hook package; its
[closure](../history/m0-t431-native-hook32-closure.md) preserves the verified
baseline. The unimplemented width expansion transfers to the separate
[dual-worker/Hook proposal](proposal-dual-width-native-workers-hooks-001.md).
Earlier planning below is retained context, not active admission or a claim
that the cross-width stages passed.

Latest owner revision separates general component x64 migration into the next
queued candidate. This hook package selects NTVWM32/64 and nthook32/64 only;
run16, NTCON, NTMON, NTSRV, NTVDM, WOW32 and VDMREDIR remain x86. Both workers
retain separate hidden Consoles and the existing NTSRV-controlled handoff.
Ordinary native children retain actual Windows identity/waits. A transient
installer-only helper is permitted if necessary after mechanism review; no
resident helper, replacement child or new lifecycle authority. These choices
supersede the earlier Hook-only/no-helper planning wording below. The current
working sequence is maintained in the
[task plan](../etc/operations/t431-native-launch-hook-plan.md); the older S
sequence below is historical candidate planning, not current admission.

Owner-approved planning, fourth unnumbered candidate in the remaining queue,
after NTMON Console-root management, root/search isolation and the bounded CCPU40/V86 contract audit; all
follow the active component-renaming package. This is not implementation
admission and does not expand T424. Finish the naming package first: NTVWM is
the native text worker and NTCON is the visible Console/Window frontend in
this proposal. The current intermediate tree may still call that frontend
NTCON; use the delivered naming baseline at admission. Reuse the preceding
root/search package's resolver and package-root contract rather than creating
a parallel lookup policy. Incorporate any relevant contract-audit handoff.

Provide transparent ordinary DOS/Win16 launch from controlled native text
programs, starting with `run16 cmd -> command.com`. Native Win32 creation
continues to use Windows semantics; only confirmed 16-bit targets route through
run16. The following [task-trace candidate](proposal-worker-task-trace-observation-001.md)
will reuse hook creation reports and NTSRV process-handle waits instead of Job
descendant observation. Monitoring is not a prerequisite for launch compatibility.

## Ownership and proposed artifacts

| Owner | Responsibility |
| --- | --- |
| Proposed src/nthook-dll | One specialist source component producing nthook32.dll and nthook64.dll; bounded API interception, target recognition, recursion exclusion and controlled native-child propagation. The same finite installation mechanism is reused by worker bootstrap and DLL propagation, not independently rewritten. |
| NTVWM | Install the correct DLL into its direct native text target before execution, at the existing suspended-create/bind/resume boundary; retain actual target wait, exit reporting, hidden Console and startup rollback. |
| run16 | Existing classification, submission and direct-result behavior; reuse its audited type-recognition code through a narrow owned classifier library, without a second resolver or generic common component. No input pump or self-redirection. |
| NTSRV | Existing admission/completion authority; authenticate hook reports in the subsequent trace package and hold observation-only process references there. No process enumeration, DLL injection owner or native scheduler. |
| interface | Copied versioned hook initialization/report declarations where needed; no injector, resolver or authentication implementation. |
| worker-base | Worker-only common mechanisms where full contracts agree; do not make an injected DLL depend on worker lifecycle, renderer or EXE-private internals. |
| NTCON / NTMON | Frontend presentation and existing management projection respectively; no hook installation or process-tree tracking. |

The two DLLs are loaded in existing controlled native processes, not new
executables, helpers or resident workers. Do not introduce rundll32, injector
EXEs, services or bootstrap helper processes. New production roots and any
third-party code require explicit architecture/provenance registration at
implementation admission; this proposal does not create those roots now.

The x64 hook is a narrowly proposed native-target toolchain island, not an
x64 NTVDM product, CCPU executor or port of run16/NTSRV/NTVWM. Current authorities
exclude general native-x64 builds. At admission, record the owner-approved
hook-only exception in the relevant architecture/build rules, with exact
toolchain, CRT, artifact manifest and cross-width copied ABI. Never link x86
objects into the x64 DLL or use this exception to change MVDM mirrors.

## Launch contract and supported boundary

- Audit the actual CMD creation entrypoints; initial supported API family is
  CreateProcessA/W. Internal CMD commands do not create processes. ShellExecute,
  alternate-token/logon launch, elevation, protected targets and direct native
  system calls are not silently claimed covered.
- Resolve/classify the target using the existing audited application-resolution
  contract. Do not infer DOS from an MZ prefix or classify only by extension.
  Preserve original launch syntax: no new required `--`, PATH precedence,
  hard-coded package root, quoting rule or System32 redirection policy.
- Confirmed DOS/Win16 targets become `run16 <original target and arguments>`.
  Run16 then follows the existing DOS completion or Win16 startup-only contract.
  The returned process object is the real launcher, not a fabricated guest
  process. Ordinary wait/exit behavior must be tested; exact guest thread,
  remote-memory, image-query and debugger equivalence are not promised.
- Native Win32 targets remain native CreateProcess results. Propagate the hook
  only to admitted native text descendants; GUI/new-Console/session boundaries
  must not acquire character-frontend authority merely through ancestry.
- Preserve application/command-line interpretation, environment, current
  directory, standard streams, handle-list inheritance, error reporting and
  caller flags. Audit CREATE_SUSPENDED, debugging, startup attributes and
  security boundaries explicitly. Unsupported interception combinations must
  have a tested documented native passthrough/refusal contract, never a silent
  approximation that changes a successful native launch.
- Temporary suspension for installation must not undo caller-requested
  suspension. Cover concurrent creation and failures before/after successful
  handoff; never terminate an already handed-off target due to report failure.
- Exclude run16, internal product roles and hook-internal creation from
  redirection. A hook must not turn `run16 target` into another run16 forever.
- Keep loader-lock initialization bounded; do not perform blocking broker
  traffic or complex launch work from DllMain. Hook discovery information is
  not authorization; reuse authenticated generation-safe session capabilities.

32-bit CMD is the initial runtime target. A 32-bit launcher can also create
a 64-bit target, but a 32-bit DLL cannot be loaded into it. Test 32->32,
32->64, 64->64 and 64->32 installation/propagation. No-helper cross-width
installation is an unresolved engineering gate, not an established result.
If it cannot be proved, stop that S and report the specific boundary; do not
add a helper, weaken coverage or claim both DLL builds prove injection.

## Observation handoff; no Job requirement

This package establishes a reusable authenticated creation-report boundary
for the subsequent trace package, not a second task registry. Successful
creation reports identify the actual parent and child process instances,
worker/session generation and known image/type. A raw PID or parent claim
is not evidence. Define minimal recipient-owned process capabilities and
close/rollback contracts; broker messages never trust sender-local handles.

The trace package will register created processes and wait on their process
handles for exit, including forced termination. DLL unload/Detach notifications
are not exit authority. Short-lived children require a retained creation-time
reference so exit before report handling cannot become a PID-reuse guess.
Reports may miss uninjected or bypassed launch paths; expose coverage/gaps,
never invent ancestry or recover it with periodic enumeration. Report loss
does not modify native execution, direct completion or worker READY/BUSY.

No Windows Job observer, process-tree kill, kill-on-close pairing, completion
authority, new scheduler, guest patch, host binary-on-disk modification,
system Registry write or global hook installation is admitted.

## Proposed S sequence

| S | Complete deliverable and gate |
| --- | --- |
| S1 | Source/API/bitness audit and finite contract ledger; select one reusable classifier and installation mechanism, document provenance/license and failed earlier recovery rungs, freeze initialization/report ABI and supported/unsupported flags. Define reproducible test sources/cases, cross-width feasibility and architecture exceptions before production implementation. |
| S2 | Production nthook32.dll and NTVWM initial installation plus controlled 32-bit child propagation; actual SysWOW64 CMD -> COMMAND -> return, nested CMD and DOS/native crossings, preserved streams/arguments/environment/exit behavior, recursion and failure tests. Publish only the passing coherent package. |
| S3 | nthook64.dll and no-helper cross-width installation/propagation closure; actual 64-bit CMD launch and all four width combinations, safe unsupported boundaries and process/handle cleanup. Building the DLLs alone cannot close S3. |
| S4 | Whole-package concurrency, session isolation, fault, native passthrough and compatibility audit; sealed hook artifacts plus existing runtime set, verified ordinary launch gates and trace-package handoff. Stop for owner acceptance; do not silently close T. |

Each S maintains a capability ledger with source owner, exact test entrypoint,
assertions, prerequisites, production wiring, passed/failed/unsupported status
and remaining ownership. Implemented capabilities connect immediately; do not
leave propagation or registration as research-only wrappers. Hook launch
compatibility and the subsequent monitoring package have separate exit gates.

## Acceptance and delivery

Exercise ordinary 32-bit and 64-bit CMD chains; DOS COMMAND/MEM/EDIT directly
and nested; DOS -> native CMD -> DOS and native CMD -> DOS -> native; Win16
with its existing startup contract; native full-screen EDIT; independent
Console roots; Console/Window display; pipes/redirection and explicit handle
lists; Unicode paths/quoting; missing/malformed images; unavailable DLL;
bitness mismatch; caller-suspended targets; unsupported debug/security flags;
parent/worker/broker failure; and repeated resident reuse. Prove successful
native launches still return their real process/thread objects and errors.

Every production P retains affected unit/integration tests, all established
17 Console + 17 Window routes, retained WOW frontiers, source/build-input
identity, coherent publication to O:/winnt and commit/push gates. Extend the
package manifest with the actually verified hook DLLs; do not silently keep
the old eight-file assumption for hook-enabled release/recovery. All build,
generated and disposable results remain under build/. No production artifact
or runtime success is claimed by this planning change.

## Research anchors to revalidate at admission

- [CreateProcessW contract](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw).
- [Microsoft Detours process/DLL installation](https://github.com/microsoft/Detours/wiki/DetourCreateProcessWithDllEx): reference, not an import decision; its cross-width helper route is excluded by this proposal.
- [Windows process waits](https://learn.microsoft.com/en-us/windows/win32/procthread/waiting-for-processes).
- Current run16 native classification/launch code, NTVWM suspended direct
  creation and bind/resume sequence, existing interface resource attachments
  and the delivered naming package. Audit current paths at admission.

Any adoption of Detours or other implementation requires its exact revision,
license/notices, finite dependencies and no-helper route to be recorded under
the source policy. Neither these references nor the side-conversation design
constitute a proved injection implementation.
