# Proposal — NtvdmSystemRoot and application search isolation

## Objective and admission

Owner-requested candidate dated 2026-09-27, promoted to the head of the
[queue](../states/QUEUE.md) by owner direction on 2026-09-28. It precedes the
CCPU40/V86 contract audit and the split WOW32 packages. This is planning only:
no numeric T allocation,
active-packet change, production repair or runtime acceptance is claimed.

Introduce the product-scoped environment variable `NtvdmSystemRoot`, derived
from the directory containing the actual `run16.exe` image. Internal product
components and guest system resources resolve through this root and their
declared package-relative locations. User applications use explicit paths or
the ordinary application search contract based on PATH, never an implicit
NtvdmSystemRoot-first search.

## Observed defect and source evidence

The owner reports that `edit` in host CMD selects the host editor, whereas
after `run16 command` and changing to C:\, it selects the package EDIT.COM.
Read-only inspection found an explicit package-first policy in
[resolve_image_path](../../src/run16-exe/main.c): for bare names, each extension
is tried beside run16 before the default SearchPath search. This does not add
an entry to PATH, making the priority invisible to the user.

[COMMAND launch adaptation](../../src/ntvdm-exe/win32/command_process_compat.c)
invokes the sibling run16 and also constructs an internal bare COMMAND.COM /c
invocation. Both consumers must be addressed together. This is source evidence
and an owner-reported reproducer, not a completed runtime diagnosis/test.

The original [BaseClient VDM configuration](../../src/opennt-host/base/win32/client/vdm.c)
uses a configured system-root-based NTVDM command. Recover that separation of
known system components from user command lookup, without importing a new
loader or rewriting original DOS search semantics.

## Required contract

- Derive `NtvdmSystemRoot` from the loaded run16 image, not argv[0], current
  directory, PATH or an unvalidated inherited override. Do not hard-code
  O:/winnt. Propagate the same package identity to its internal consumers;
  the environment string is a locator, never IPC authentication authority.
- Use the root with declared relative paths, not a blanket flat-directory
  search. Preserve the established root/system32 resource layout. A missing
  required internal component fails explicitly rather than selecting a
  same-named program from the current directory or PATH.
- Keep the host `SystemRoot`, Windows/System directory APIs and host DLL
  services distinct. NtvdmSystemRoot replaces the product/guest-root role,
  not the real Windows installation root for native applications.
- Audit existing GetNtvdmWindowsDirectory/GetNtvdmSystemDirectory, worker
  roots, environment construction, config expansion and nested launches.
  Reuse the existing narrow package/owner bindings; remove redundant root
  discovery rather than creating parallel policy.
- Immutable guest code/config references to SystemRoot need a bounded,
  documented guest-facing environment or expansion binding. Do not patch
  guest binaries, globally substitute variable text, or leak a guest root as
  the host SystemRoot into Win32 children. Preserve explicit user config paths.
- Explicit application paths remain authoritative; missing paths must not
  fall back to another same-named application. Bare user commands follow PATH
  and the audited ordinary current-directory/extension rules. Neither an
  explicit package probe nor SearchPath's implicit application-directory search
  may silently reintroduce NtvdmSystemRoot priority.
- The package directory participates in user search only when the user has
  explicitly put it in PATH, selected it as the ordinary current directory,
  or supplied it in the application path. No name-based exception for edit,
  mem or command. Document that bare run16 command/mem from elsewhere no
  longer discovers package applications when these conditions do not hold.
- Direct, inner and nested run16 must agree on application lookup without
  replacing original COMMAND parsing, guest EXEC or task completion policy.
  Handle an existing broker/worker from a different package explicitly; do not
  silently mix roots or retarget a live session by changing an environment value.

## Dependency classification to complete at admission

Classification is by invocation role, not filename extension or package presence.

| Role | Selected examples | Policy |
| --- | --- | --- |
| Internal executable | ntkvm.exe, ntvdm.exe, ntsrv.exe; internally invoked run16.exe | Accurate package-relative path from the established root; use the actual selected package at admission, without reviving a retired helper. |
| Internal DOS interpreter | Product-generated COMMAND.COM /c and startup interpreter selection | Explicit package interpreter path; distinguish from a user's command request. |
| Host product modules | WOW32.DLL, VDMREDIR.DLL | Root-derived module identity through their existing loader boundaries. |
| Guest system media/config | NTIO.SYS, NTDOS.SYS, KRNL386.EXE, DOSX, HIMEM, REDIR, MSCDEXNT, CONFIG.NT, AUTOEXEC.NT, SYSTEM.INI and configured guest dependencies | Declared package-relative layout and original configuration/loader semantics, not generic application PATH injection. |
| User applications/tools | EDIT, MEM, WINMINE, SOL, WRITE, MONITOR, user-requested COMMAND/CMD and arbitrary applications | Explicit path or ordinary user search; bundling gives no priority. |
| Real host services | Native Windows system directories, host DLLs and shell selected through the applicable host contract | Preserve host identity; do not redirect to guest media by filename. |

Inventory every selected caller, root source, relative location and failure
behavior, including optional drivers, fonts, profile/registry files and module
loads. An internal role is not a claim that every optional capability is usable.

## Proposed S tasks

| S | Work | Exit condition |
| --- | --- | --- |
| S1 | Audit root/environment/search callers and original owners; reproduce same-name shadowing; freeze the role and search-order ledger. | Every affected selected caller has one root/search policy and exact positive/negative test assertions. |
| S2 | Implement root derivation/propagation and internal component/media/config bindings with minimal source-shaped changes. | Relocated packages and nested launches select the correct internal files; missing dependencies and wrong-root joins fail explicitly; host SystemRoot and guest immutability are preserved. |
| S3 | Remove package-first user discovery and fix internally generated COMMAND invocations; cover direct and nested application search. | No implicit package priority; explicit paths and current-directory/PATH/extension order pass production-path tests. |
| S4 | Similar-issue sweep, full regression, diff/code reduction accounting and coherent publication. | Formal x86 build and all applicable production-P gates pass; docs, tested package, commit/push and owner handoff are complete. |

## Acceptance and delivery

Check in tests that assert the actual selected image path as well as output and
exit result. Cover same-name applications in the package, current directory and
multiple PATH directories; explicit suffixes, absent files, spaces/Unicode,
empty/unset PATH, inherited incorrect NtvdmSystemRoot and package relocation.
Include the reported host EDIT versus package EDIT.COM case where available,
plus controlled test executables so coverage is not tied to one host editor.

Exercise direct run16, interactive COMMAND, multiple nested COMMAND and
DOS-to-native-to-DOS return. Required internal lookup must work when the package
is absent from PATH, while a user command must not gain that exception. Verify
native children retain the real host SystemRoot, guest startup still finds its
media, and no host settings or immutable guest files are changed.

Each production P follows [execution rules](../rules/EXECUTION.md): incremental
x86 build, existing DOS17 and applicable nested/fault/display tests, retained
WOW frontiers, coherent runtime publication to O:/winnt, then reviewed
commit/push. New test/staging directories stay under repo build; runtime logs
use the established logs location. Planning-only changes require governance,
links and diff checks, not a build or redeployment.

## Non-goals and handoff

No ConPTY migration, helper executable split, guest patch, global environment
or system registry mutation, new scheduler, broad DLL-loader replacement or
unrelated frontend work. Existing product-experience/minimization evidence may be reused; those
candidates now follow this package in Queue; this candidate owns this specific root/search
repair and must not duplicate a repair already delivered before admission.
