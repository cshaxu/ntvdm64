# Architecture Rules

1. The production source owners are exactly `mvdm-host`, `opennt-host`,
   `mvdm-tools`, `mvdm-softpc-firmware`, `mvdm-softpc-patch`,
   `mvdm-platform-abi`, `mvdm-guest/dos/v86`, `mvdm-guest/bin86`,
   `mvdm-guest/wow16`, `mvdm-guest/font16`,
   `adapter-mvdm-host-in`, `adapter-mvdm-host-out`, `session`, `broker`,
   `adapter-opennt-host`, and `app`.
2. `mvdm-host` is the sole complete selected MVDM host-runtime mirror.
   `opennt-host` is the sole original non-MVDM OpenNT host-service mirror. It
   contains every separately accepted original owner package, not only the
   first Base VDM slice, and accepts only a required original slice after a complete package-boundary
   audit proves: a
   direct selected-MVDM caller; a retained original algorithm/state-machine
   benefit; a finite outgoing closure through public modern APIs, existing
   bounded adapters or a specifically admitted small adapter; and no need to
   import an NT4 system/product shell. CSR/CSRSS, NTDLL CSR transport, kernel
   VDM, the full BaseClient/Kernel32 product, Win32k and USER/GDI server are
   stopping boundaries, not recursive import candidates. Every accepted and
   rejected candidate has a complete source/ABI/final-disposition record in
   the shared host tracker. Original MVDM support carriers/libraries remain in
   their original-relative `mvdm-host` paths; independent historical tools
   belong to `mvdm-tools`. A tool is never a host
   runtime dependency merely because it is independently buildable.
   `mvdm-softpc-firmware` owns only selected original immutable firmware/ROM/
   data inputs; it is neither a host-runtime library nor a machine executor.
   `app` stages an admitted immutable input through the selected backend
   binding. Executable `softpc.new/base/*` packages belong to `mvdm-host`.
   `mvdm-platform-abi` contains declarations only.
   For source-function BFS, zero-degree is all original definitions in
   `mvdm-host` plus their transitive resolved call closure while each physical
   original definition remains selected OpenNT `mvdm` source already mirrored
   under a `mvdm-*` component. Existing but unreachable support, tool, firmware
   and guest definitions are not zero-degree. The rule creates no runtime link
   edge and matches source identity rather than function spelling.
3. The selected executable `mvdm-host/softpc.new` source composition is the
   sole production machine implementation and owns its original CPU, memory,
   firmware and PC-device semantics. The `softpc` family of
   `adapter-mvdm-host-out` preserves only reached historical SoftPC/CCPU
   interface shapes and binds the original SoftPC call graph. It never includes
   a Bochs type, object or global and never provides a fallback executor.
4. `adapter-mvdm-host-in` transports fixed-width copied machine events and typed
   completion only. Selector, service, dispatch and provider meaning remain in
   imported `mvdm-host` source.
5. `adapter-mvdm-host-out` contains the explicit `win32`, `softpc`, `monitor`,
   `redir`, `wow`, `vdd` and `debugger` interface families. Each preserves
   the reached original name, parameters, calling convention, layout, ordering
   and failure contract wherever the platform permits. No family becomes an
   alternate OpenNT provider or absorbs another family's semantics.
   `adapter-opennt-host` is the distinct, package-private family for reached
   `opennt-host` private-host interfaces. Its subfamilies remain named by their
   accepted OpenNT owner packages; it cannot absorb MVDM caller semantics,
   reconstruct CSR/CSRSS, or become a generic adapter.
6. The `monitor` family owns the complete same-shaped user-mode
   `NtVdmControl`, `VDM_TIB`, V86-event and interrupt/fault-handler interface
   family. It binds bounded per-session/per-thread state and an app-installed
   opaque machine endpoint. It must not recreate NT kernel or CSRSS internals;
   unavailable operations fail deterministically.
7. The `redir`, `wow`, `vdd` and `debugger` families respectively own the
   original VDMREDIR, WOW32/WOWEXEC, VDD and debugger product-interface
   boundaries. A missing interface is assigned to one named family before a
   mirror source is modified to bypass it.
8. `session` is dependency-neutral and owns one independent VDM instance's
   lifecycle, mapping-manager instances, resources, completions/events and
   teardown. It has no BOP, DOS, WOW, VDD, Redirector or Win32 service
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
    the `softpc` family of `adapter-mvdm-host-out` with a checked synchronous mapping lease with address, span,
    access and epoch. No such pointer crosses an ABI or reaches asynchronous
    work.
18. `mvdm-guest/dos/v86`, `mvdm-guest/bin86`, `mvdm-guest/wow16` and
    `mvdm-guest/font16` are complete load-only mirrors. Their source, objects,
    libraries and products never satisfy a host symbol. App loads
    manifest-selected immutable bytes through the selected backend binding.
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
25. The current host build profile is MSVC Win32/x86 `/MT`, with the original
    SoftPC CCPU40 executor selected by `CPU_40_STYLE`. `CPU_30_STYLE` is the
    retired NT4 kernel-VDM V86-monitor contract and must not enter any
    project-owned compilation, linking, runtime, fixture or acceptance input.
    Different architectures never satisfy
    symbols in one process; any later cross-process interaction uses only an
    admitted fixed wire ABI.
26. Original SoftPC CCPU40 is the sole session machine backend. A session has
    no fallback or simultaneous executor. Every functional test that reaches
    machine, guest or MVDM-host behavior records the selected Win32/x86 CCPU40
    row. Pure source/build-audit work may state why it has no machine row.
27. `src.old/` and comparison repositories are evidence only and must not
    enter source, build, link, runtime or acceptance manifests.
29. The `mvdm-host` support paths may be linked only after the package/symbol
    tracker records their original consumer, exact interface shape, binding
    owner and x86/x64 disposition. `mvdm-tools` may never be linked into `app` or
    an MVDM host runtime. `mvdm-softpc-firmware` may not be compiled or linked
    as a host provider; `app` may stage its immutable inputs only through the
    selected machine backend. Executable `softpc.new/base/*` source packages
    belong to `mvdm-host`, not the firmware component.
30. A `*-overlay` is a private implementation partition of its matching
    original mirror, not an additional generic source component.  It compiles
    into that mirror's library only; the matching mirror is its sole caller
    and linker.  Adapters, `session`, `broker`, `app`, fixtures and every other
    mirror must use the mirror boundary, never an overlay boundary.
31. Mirror similarity is a release property.  An original package may not be
    relocated into an adapter or replaced by autonomous code merely to reach a
    build.  Retain upstream path/name/control structure in the mirror; use a
    registered local `DIVERGENCE:` hook only for a minimal binding, and move
    any material added mechanism to the paired overlay.
32. `mvdm-softpc-patch` is a narrow component for individually reviewed
    NTVDMx64-derived SoftPC patch bodies. Each body must have a register row
    identifying its original SoftPC caller and interface shape, NTVDMx64
    provenance, reason, x86/x64 disposition, mapping-manager use and removal
    condition. It is neither a generic adapter nor a second executor and may
    reach mechanics only through `adapter-mvdm-host-out/softpc`.
