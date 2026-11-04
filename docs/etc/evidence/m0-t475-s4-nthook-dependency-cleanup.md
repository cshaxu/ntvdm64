# T475 S4 — NTHook dependency cleanup

## Admission and P1

The owner directs removal of NTHook private dependencies. The post-S3 audit
found Run16, NTVDM and NTVWM selecting actual NTHook-owned context and/or
installer objects. It also found that S3's Redir header check covered the 18
component objects but missed the private SoftPC forced header on actually
selected OpenNT archive members and on selected Hook32 shared objects. That
prior S3 header-closure claim is corrected; S4 P3 must address both closures.

P1 moves the existing context structure, payload reader and package-path
construction from NTHook into `common/native-hook`, retaining the packet GUID,
names, calling conventions and data handling. Run16 uses the common header
and archive; NTHook and both workers reuse the same source. The launcher
compiler dependency and private-header negative control passed.

The x86/x64 product build, 63 immutable guest files, 274 export ordinals and
all 12 selected Control gates passed. Actual x86 and x64 Hook fixtures each
passed 118 assertions; the x64/x86 alternating descendant run passed 168,
including payload, handle transfer, invalid inputs and lifecycle failures.
The first isolated x86 fixture lacked its required pinned Run16 image and
failed; a fresh directory with that prerequisite passed. Local package
`563bc615-556c-49e6-869e-6d7ea0004396` has ten verified release hashes.
Full was not run; DOS4GW/NESticle was absent. Logs, candidate and prior asset
backup are under `build/t475-s4-p1`. Main integration and push remain deferred
by the owner.

## P2: shared installation orchestration

The existing suspended-child installation orchestration and public contract
now belong to `common/native-hook`; NTVDM, NTVWM and NTHook consume the same
implementation. NTHook retains its interception entry and policy; generic
injection remains in `lib-win32/process-hook`. Packet construction, handle
transfer, suspended-child ownership and failure rollback are unchanged.
Production graph and compiler dependency checks passed for both workers,
including injected private-header negative controls.

The x86/x64 product build, 63 guest files, 274 export ordinals and 12 selected
Control gates passed. Actual x86 and x64 Hook fixtures each passed 118 checks;
the alternating-width fixture passed 168, covering real child launches,
payload and handle transfer, invalid inputs and cleanup. Local package
`3f1e35fd-7d71-4e0b-96be-9ff23b1257cd` has ten matching release hashes. Full
was not run and DOS4GW/NESticle was absent. Logs and previous release backup
are under `build/t475-s4-p2`.

## P3: complete selected-archive header boundary

The x86 graph now removes the SoftPC `terminate` namespace forced header
from common, compatibility, lib and non-MVDM source objects while preserving
their existing remaining flags and worker-specific uses. Both ordinary `cc`
and `cc_rtl` bindings, including the RTL default `/Gz`, are handled. The first
two attempts failed the new complete-closure check because explicit and then
default RTL bindings were missed; those failures are retained in the logs.

The production graph and compiler dependency check recursively traverses all
archive candidates for Hook32 and VDMREDIR, not just each component's own
objects. It verified 98 closure objects and rejected an injected private header;
the NTVDM import library is treated as the owner-deferred PE runtime boundary.
Launcher, both workers and WOW compiler ownership checks passed. The actual
link maps show the context and installer selected from the common archives.

The x86/x64 product build, 63 guest files, 274 export ordinals, 118 x86 and
118 x64 Hook assertions, 168 cross-width assertions, 24 redirector async
checks, 12 original NetAPI checks and 12 selected Control gates passed. Actual
DOS async-read, async-write and NetAPI probes passed. Local package
`eedc8134-05c6-449d-b799-c036d739e909` has ten matching release hashes. Full
was not run and DOS4GW/NESticle was absent. Logs and prior release backup are
under `build/t475-s4-p3`. Main worktree integration, system32 publication and
push remain deferred by the owner; NTVDM PE runtime imports remain a separate
queued task.

## P4: native classifier composition boundary

The owner reopens S4 after an audit found that Run16's selected x64 OpenNT
classifier forced an NTHook-private `legacy_classifier_arch.h`; Hook64 uses
the same classifier composition. The byte-identical project-owned header now
resides in `common/base_srv`, and both native build rules select it only for
the original classifier translation unit. The original `_X86_` conditional
and retained OS/2 NE classification branch are unchanged. The full Run16
compiler closure contains no NTHook-private header; an injected old-path
negative control was rejected. No independent OS/2 NE guest sample was run.

The x86/x64 product build, 63 immutable guest files, 274 export ordinals,
118 x86 and 118 x64 Hook checks, 168 cross-width Hook checks, and 12 selected
Control gates passed. A real Win16 WRITE startup retained its main, document
and page-info windows across four snapshots; editing and normal exit were not
tested. Local package `8adf96a8-42b3-4273-8f48-27b58b07df1d` has ten matching
release hashes. Full was not run; DOS4GW/NESticle was absent. Logs and the
prior release backup are under `build/t475-s4-p4`. Main integration, runtime
publication and push remain deferred by the owner.
