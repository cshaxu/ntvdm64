# Proposal — NtvdmSystemRoot and application search isolation

## Objective and admission

Owner-requested candidate dated 2026-09-27, promoted to the head of the
[queue](../states/QUEUE.md) by owner direction on 2026-09-28. It precedes the
CCPU40/V86 contract audit and the split WOW32 packages. This is planning only:
no numeric T allocation,
active-packet change, production repair or runtime acceptance is claimed.

Introduce one product-owned `NtvdmSystemRoot` directory contract shared by the
product's native EXEs (`run16`, `ntsrv`, `ntvdm`, `ntcon`, `ntvwm`, `ntmon`). Each
process derives its own root from the directory containing its actual loaded
EXE image. Co-located EXEs normally agree; a split or mismatched package must
not silently borrow another process's root. Internal product components and
guest system resources use this root and their declared relative locations.
User applications use explicit paths or DOS-style current-directory-then-PATH
search, never an implicit NtvdmSystemRoot-first search.

Classify directory uses into three roles before changing a caller: **user**
(working directory, PATH, explicit paths, PIF and temp configuration), **real
host OS** (the native Windows installation and loader/service APIs), and
**product/guest** (the local EXE root and original OpenNT guest layout). A
directory named `system` or `system32` is not enough to identify its role.

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

The owner clarifies that `run16`'s own directory may win a user-command lookup
only when it happens to be the caller's current working directory, appears in
PATH at its ordinary position, or was explicitly supplied. Microsoft [PATH
documentation](https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/path)
states that the current directory precedes PATH directories; the selected
original [PIF lookup](../../src/mvdm/dos/command/cmdpif.c) likewise checks
`.` first. The current run16 resolver instead probes the package directory
before `SearchPathW(NULL, ...)` for each extension. `SearchPathW(NULL, ...)` is
not a stable expression of the requested DOS-only directory order; its
default path is host-policy dependent. Freeze DOS COMMAND's actual extension
and explicit-path rules from original source/guest behavior in S1 rather than
adopting modern CMD's extension order by assumption.

## Required contract

- Provide one shared, process-independent implementation usable by every
  product native EXE, not only `worker-base`'s worker clients. Each process
  derives `NtvdmSystemRoot` from its **own actual loaded EXE image** using a
  checked full-path API, never argv[0], CWD, PATH or an inherited variable.
  Replace duplicate local derivations; do not hard-code O:/winnt. A variable,
  if exposed to a child or guest, is a projection/locator, never the authority
  for that native process's own root or for broker authentication. Verify the
  selected same-package identity at cross-process join; missing siblings and
  mismatched roots fail rather than redirecting another process or session.
- Use the root with declared relative paths, not a blanket flat-directory
  search. Follow the original OpenNT caller's `root`, `root\system` and
  `root\system32` composition where applicable; `system32` is a relative
  subdirectory, not a second independently discovered root. A missing required
  internal component fails explicitly rather than selecting a same-named
  program from the current directory or PATH. A product DLL or guest binary
  stored in `root\system32` does not redefine the native EXE's root to its own
  module directory.
- Keep the host `SystemRoot`, Windows/System directory APIs and host DLL
  services distinct. NtvdmSystemRoot replaces the product/guest-root role,
  not the real Windows installation root for native applications. Audit each
  real-host caller individually; do not globally shadow SYSTEMROOT or replace
  native GetWindowsDirectory/GetSystemDirectory.
- Audit existing GetNtvdmWindowsDirectory/GetNtvdmSystemDirectory, worker
  roots, environment construction, config expansion and nested launches.
  Preserve the original Win16 distinction: the loaded KRNL386 location gives
  its module-loading `system32`; `SYSTEMROOT` supplies the real-Windows base
  for `system`, while `WIN16DIR` may select the Win16 Windows directory.
  Win16 GetSystemDirectory returns the `system` path. A guest-only projection
  may be required to make these point into the selected product package; it
  must not change the native host SystemRoot or leak into a Win32 child.
- Immutable guest code/config references to SystemRoot need a bounded,
  documented guest-facing environment or expansion binding. Do not patch
  guest binaries, globally substitute variable text, or leak a guest root as
  the host SystemRoot into Win32 children. Preserve explicit user config paths.
- Explicit application paths remain authoritative; missing paths must not
  fall back to another same-named application. Bare user commands search the
  caller's current working directory first, then each PATH directory in
  order, with the audited DOS extension rules inside that directory search.
  Neither an explicit package probe nor SearchPath's implicit application-directory search
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
- ROMS_REZ_ID firmware is embedded in the current EXE build. Do not require a
  deployed `root\softpc` directory merely because a legacy `firmware_root`
  field and `host_find_file` adapter remain. Audit reachability of non-ROM
  reads/writes through those interfaces before retaining or removing them.
  Preserve user/PIF paths and host TMP/TEMP/GetTempPath behavior. The current
  COMMAND redirection temp-file fallback to the product Windows directory is
  an explicit exception to classify and repair or justify; it is not evidence
  that temp files generally belong at product root.

## Dependency classification to complete at admission

Classification is by invocation role, not filename extension or package presence.

| Role | Selected examples | Policy |
| --- | --- | --- |
| Internal executable | ntcon.exe, ntvdm.exe, ntsrv.exe; internally invoked run16.exe | Accurate package-relative path from the established root; use the actual selected package at admission, without reviving a retired helper. |
| Internal DOS interpreter | Product-generated COMMAND.COM /c and startup interpreter selection | Explicit package interpreter path; distinguish from a user's command request. |
| Host product modules | WOW32.DLL, VDMREDIR.DLL | Root-derived module identity through their existing loader boundaries. |
| Guest system media/config | NTIO.SYS, NTDOS.SYS, KRNL386.EXE, DOSX, HIMEM, REDIR, MSCDEXNT, CONFIG.NT, AUTOEXEC.NT, SYSTEM.INI and configured guest dependencies | Declared package-relative layout and original configuration/loader semantics, not generic application PATH injection. |
| User applications/tools | EDIT, MEM, WINMINE, SOL, WRITE, MONITOR, user-requested COMMAND/CMD and arbitrary applications | Explicit path or ordinary user search; bundling gives no priority. |
| Real host services | Native Windows system directories, host DLLs and shell selected through the applicable host contract | Preserve host identity; do not redirect to guest media by filename. |
| User/host configuration | CWD, PATH, explicit PIF and application paths, TMP/TEMP/GetTempPath | Keep their supplied/search semantics; do not substitute package paths except a separately justified original guest default. |

Inventory every selected caller, root source, relative location and failure
behavior, including optional drivers, fonts, profile/registry files and module
loads. An internal role is not a claim that every optional capability is usable.

### Read-only caller map for the implementation handoff

The paths below identify observed source locations, not passing runtime tests.
S1 must resolve reachability and the exact original-owner contract before
changing any imported file.

| Current source | Observed use and category | Implementation check |
| --- | --- | --- |
| [run16 main](../../src/run16-exe/main.c), [frontend scope](../../src/run16-exe/frontend_scope.c), [ntsrv main](../../src/ntsrv-exe/main.c) | Independent EXE-relative sibling discovery; **product**. run16's `resolve_image_path` also makes the product directory first in **user** lookup. | Move only shared self-root mechanics to a native EXE-wide component; keep internal sibling lookup distinct from DOS application lookup. |
| [ntvdm package layout](../../src/ntvdm-exe/package_layout.c), [session](../../src/ntvdm-exe/session/session.c), [firmware/directory adapter](../../src/ntvdm-exe/softpc/mvdm_softpc_firmware.c), [shadow registry](../../src/ntvdm-exe/softpc/mvdm_shadow_registry.c), [native COMMAND launcher](../../src/ntvdm-exe/win32/command_process_compat.c) | EXE-relative root, `root\system32`, `root\softpc`, `NTVDM.REG` and sibling run16 are **product**; internal bare COMMAND invocation needs an explicit selected interpreter. | Reuse one self-root implementation, retain original relative layouts, and verify non-ROM `softpc` reachability. Existing ANSI/short-root limits still need explicit validation; root unification alone does not remove them. |
| [WOW32 initialization](../../src/mvdm/wow32/wow32.c), [OEMUNI directory facade](../../src/mvdm/oemuni/process.c), [COMMAND configuration](../../src/mvdm/dos/command/cmdconf.c), [CONFIG.NT](../../src/mvdm/bin86/config.nt), [AUTOEXEC.NT](../../src/mvdm/bin86/autoexec.nt) | Guest Windows directory and system32/media are **product/guest**. Immutable config references to `%SystemRoot%` require a bounded guest expansion. | Preserve original filename/layout and caller ordering; do not replace all occurrences of `SystemRoot` in native process environment. |
| [Win16 kernel boot](../../src/mvdm/wow16/kernel31/ldboot.asm), [Win16 directory API](../../src/mvdm/wow16/kernel31/userpro.asm), [Win16 loader](../../src/mvdm/wow16/kernel31/ldopen.asm), [run16 environment](../../src/run16-exe/main.c) | Original `WIN16DIR` selects guest Windows dir; guest-visible `SYSTEMROOT` supplies `root\system`, while loaded KRNL386 supplies `root\system32`. Current child environment preserves real host SYSTEMROOT, so the two guest paths may diverge. | Prove the actual guest environment block and choose a bounded guest-only projection; keep native SYSTEMROOT real and restore it when DOS launches a native child. Never edit immutable KRNL386. |
| [DOS environment transfer](../../src/mvdm/dos/command/cmdmisc.c), [DOS/native environment transforms](../../src/mvdm/dos/command/cmdenv.c), [BaseCreateVDMEnvironment](../../src/opennt-host/base/win32/client/vdm.c) | These are the **host-to-guest and guest-to-host boundaries**, not another independently discovered root. | Test inherited/incorrect root, real host SYSTEMROOT, guest startup and DOS-to-Win32 child return in both directions. |
| [SoftPC ROM reader](../../src/mvdm/softpc.new/host/src/nt_rez.c), [legacy file lookup](../../src/mvdm/softpc.new/host/src/nt_unix.c), [formal ROM build](../../tools/build/New-T310OriginalSoftpcNinja.ps1) | Active ROMs are embedded **product** resources; legacy `root\softpc` file lookup remains in source. | Do not require or manufacture an external ROM tree. Classify reachable non-ROM file reads/writes before retiring the old field/adapter. |
| [COMMAND temporary file](../../src/mvdm/dos/command/cmdredir.c), [PIF lookup](../../src/mvdm/dos/command/cmdpif.c) | Temp and PIF selections are normally **user/host**; COMMAND currently falls back from failed GetTempFileName to product Windows directory. | Preserve explicit PIF/start directory and TMP/TEMP; separately dispose of this temp fallback without treating product root as generic scratch space. |
| [WOW32 host font facade](../../src/wow32-dll/source/wow_public_user_facade.c), [XACTSRV workstation API](../../src/opennt-host/netapi/xactsrv/apiwksta.c) | Native GetWindowsDirectory/GetSystemDirectory calls with **real host** meaning. | Keep host identity; audit selected production reachability before changing any apparent system-directory call. |

Product DLLs such as WOW32/VDMREDIR and guest binaries such as KRNL386 may
live below `root\system32`; their module directory is not the `NtvdmSystemRoot`
of the containing native EXE. Optional VDD, user-selected PIF/application and
host DLL loading remain separate cases. S1 must also inventory `ntvwm`,
`ntcon`, and `ntmon` even if an EXE currently needs no package file: the shared
root contract must be available consistently without inventing a resource.

## Proposed S tasks

| S | Work | Exit condition |
| --- | --- | --- |
| S1 | Complete the three-role caller ledger, original DOS search/extension and Win16 directory-source audit, selected-host reachability, non-ROM SoftPC use and same-name shadowing reproducer. | Every selected caller has a user/host/product disposition, original-source basis, failure behavior and positive/negative assertion; no modern CMD/DOS equivalence is assumed. |
| S2 | Add the single native EXE-wide self-root implementation; replace duplicate derivations and bind internal component/media/config paths with minimal source-shaped changes, including guest-only projection if source evidence requires it. | Every product EXE derives its own root; relocated and nested packages select correct internal files; missing dependencies and wrong-root joins fail explicitly; real host SystemRoot and guest immutability are preserved. |
| S3 | Remove run16's package-first user discovery and fix internally generated COMMAND invocations; cover direct, interactive and nested application search. | No implicit package priority; explicit paths and CWD-then-PATH plus audited DOS extension order pass production-path tests. |
| S4 | Similar-issue sweep, full regression, diff/code reduction accounting and coherent publication. | Formal x86 build and all applicable production-P gates pass; docs, tested package, commit/push and owner handoff are complete. |

## Acceptance and delivery

Check in tests that assert the actual selected image path as well as output and
exit result. Cover same-name applications in the package, current directory and
multiple PATH directories; explicit suffixes, absent files, spaces/Unicode,
empty/unset PATH, inherited incorrect NtvdmSystemRoot and package relocation.
Include a split-EXE-directory/wrong-root-join negative case and prove that
each product native EXE uses its own image location rather than inherited root
text. Put controlled same-name applications in (1) the current directory,
(2) two ordered PATH directories, and (3) the package directory omitted from
both; record the selected full path for each result. A case where CWD equals
the package directory must pass through the ordinary CWD rule, not a special
package exception.
Include the reported host EDIT versus package EDIT.COM case where available,
plus controlled test executables so coverage is not tied to one host editor.

Exercise direct run16, interactive COMMAND, multiple nested COMMAND and
DOS-to-native-to-DOS return. Required internal lookup must work when the package
is absent from PATH, while a user command must not gain that exception. Verify
native children retain the real host SystemRoot, guest startup still finds its
media, and Win16's guest Windows, `system` and `system32` paths each match the
original loader/API rule under a relocated package. Verify temp/PIF paths and
ROM startup independently; no host settings or immutable guest files change.

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
