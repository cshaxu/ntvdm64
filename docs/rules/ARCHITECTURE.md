# Architecture Rules

1. The production source owners are exactly `bochs-core`,
   `opennt-mvdm-host`, `opennt-platform-abi`, `opennt-guest-dos`,
   `opennt-guest-wow16`, `adapter-bochs`, `adapter-bop`, `adapter-softpc`,
   `adapter-win32`, `adapter-vdm-monitor`, `adapter-redir`, `adapter-wow`,
   `adapter-vdd`, `adapter-debugger`, `session`, `broker`, and `app`.
2. `opennt-mvdm-host` is the sole non-guest/non-tool MVDM mirror. BOP, host,
   SoftPC and utility package distinctions may become library targets but not
   separate source components. `opennt-platform-abi` contains declarations
   only.
3. Bochs owns CPU, memory, firmware and PC-device semantics. `adapter-bochs`
   is its only production caller and contains only Bochs mechanics.
   `adapter-softpc` reaches the machine only through typed `adapter-bochs`
   operations and never includes a Bochs type, object or global.
4. `adapter-bop` transports fixed-width copied machine events and typed
   completion only. Selector, service, dispatch and provider meaning remain in
   imported `opennt-mvdm-host` source.
5. `adapter-win32`, `adapter-softpc` and `adapter-vdm-monitor` preserve the
   reached original name, parameters, calling convention, layout, ordering
   and failure contract wherever the platform permits. An adapter does not
   become an alternate OpenNT provider.
6. `adapter-vdm-monitor` owns the complete same-shaped user-mode
   `NtVdmControl`, `VDM_TIB`, V86-event and interrupt/fault-handler interface
   family. It binds bounded per-session/per-thread state and an app-installed
   opaque machine endpoint. It must not recreate NT kernel or CSRSS internals;
   unavailable operations fail deterministically.
7. `adapter-redir`, `adapter-wow`, `adapter-vdd` and `adapter-debugger` are
   separate specialist adapters for the original VDMREDIR, WOW32/WOWEXEC, VDD
   and debugger product-interface families. Each preserves only its named
   historical external boundary; none is an alternate provider or a generic
   compatibility layer. A missing interface is assigned to one of these or to
   an existing adapter before any mirror source is modified to bypass it.
8. `session` is dependency-neutral and owns one independent VDM instance's
   lifecycle, mapping-manager instances, resources, completions/events and
   teardown. It has no BOP, DOS, WOW, VDD, Redirector, Win32 or Bochs service
   vocabulary.
9. `app` creates and wires session instances. The current product binds one
   active imported MVDM host context per process and permits multiple
   `ntvdm.exe` processes. Project-owned APIs must nevertheless be
   multi-instance-safe. In-process multiple imported MVDM contexts require a
   separate source-led reentrancy/global-state audit.
10. DOS child programs, COMMAND re-entry and multiple WOW16 tasks inside one
   machine are intra-session guest/task lifecycles, not separate VDM sessions.
11. `broker` is a distinct project-owned per-user process boundary for
    cooperative VDM registration, stable identity, command queues,
    notifications, leases and cleanup. It may recover source-proven observable
    BaseSrv coordination contracts but must not recreate CSRSS, scan or
    control unrelated processes, or acquire machine/BOP/guest semantics.
12. Broker IPC is versioned and fixed-width. A broker message may not contain
    a native pointer/HANDLE, local mapping token, guest pointer, Bochs object,
    CRT-owned object or cross-process callback.
13. Guest width and host width are orthogonal. Imported MVDM code observes
    original 16/32-bit ABI values and a session-owned 32-bit compatibility
    object space; native x86/x64 resources remain behind adapters.
14. One mapping-manager implementation has separate per-session typed
    instances for guest memory, host resources and completion/callback
    records. Instances do not share a numeric namespace. Candidate allocation
    begins at zero, skips source-proven reserved values, is monotonic, and does
    not reuse IDs within a session.
15. Both x86 and x64 builds map opaque native resources; x86 may not use
    identity pass-through. A surrogate is restored to a native resource only
    by its owning adapter. A native resource never enters MVDM or guest state.
16. Numeric data is not an opaque identity. Lengths, offsets, times, flags,
    errors, registers and guest addresses retain original semantics and must
    receive explicit range and overflow validation.
17. Historical guest-pointer calls may expose a native pointer only through
    an `adapter-softpc` checked synchronous mapping lease with address, span,
    access and epoch. No such pointer crosses an ABI or reaches asynchronous
    work.
18. `opennt-guest-dos` and `opennt-guest-wow16` are complete load-only
    mirrors. Their source, objects, libraries and products never satisfy a
    host symbol. App loads manifest-selected immutable bytes through
    `adapter-bochs`.
19. The selected OpenNT tree is one package-scope union of the pinned OpenNT
    and OpenNT-4.5 MVDM baselines. Every target path has one selection;
    conflicts are decided at complete-package scope with provenance. Parallel
    edition roots and undocumented file-level hybrids are forbidden.
20. Recovery order is mandatory: audit an applicable project-owned current or
    quarantined owner candidate; directly composable original source; the
    smallest same-shaped adapter/build seam; a registered mirror-private
    overlay or external-code intrusion; newly authored behavior only when no
    usable source remains. A candidate may be selectively copied only after
    per-file provenance, owner, dependency, behavior and test review; no whole
    component tree may be revived by default. Convenience and trace order do
    not justify skipping a rung.
21. Every mirror file is exact upstream, a registered true subset, or a
    registered same-shaped minimal modification. Every changed imported
    expression carries `DIVERGENCE:` and a component README register entry.
22. A material added body belongs in the matching `*-overlay`. An overlay is
    private to its mirror, exposes no public ABI and may not be called,
    included or linked by another component or fixture.
23. Do not introduce generic `compat`, `common`, `adapter-host` or
    `adapter-common` roots. A new specialist adapter requires a complete
    original owner-package and ABI audit plus explicit admission.
24. Modern host integration may use public Win32 filesystem, device, process,
    console, IPC and registry APIs under ordinary permissions. The product
    must not modify/rebuild Windows system components or require installation
    mutations to start or sustain itself.
25. The host build matrix is MSVC Win32/x86 `/MT` and MSVC x64 `/MT`.
    Different architectures never satisfy symbols in one process;
    cross-process interaction uses only an admitted fixed wire ABI.
26. `src.old/` and comparison repositories are evidence only and must not
    enter source, build, link, runtime or acceptance manifests.
27. Bochs 2.6 is the only mirror baseline for `bochs-core`. Existing project
    Bochs/adapter mechanics are recovery evidence only; every retained core
    difference is minimized, individually registered, and placed in the
    matching private overlay when it exceeds the mirror rule's local boundary.
