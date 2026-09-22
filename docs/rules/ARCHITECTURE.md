# Architecture Rules

For these rules, `mvdm-host` names the manifest-selected executable host slice
of the canonical physical `src/mvdm/` OpenNT `base/mvdm` tree. It is not a
separate filesystem root; explicit paths use `mvdm/`. This preserves existing
logical dependency vocabulary without splitting original source for builds or
visual comparison.

1. The production source owners are the canonical physical `mvdm` tree (with
   manifest-declared host, guest, tool and firmware slices), `opennt-host`,
   `opennt-abi/host-compat`, `product-abi`, `product-package`, and the
   executable-owned `run16`, `basesrv`, `ntvdm` and `dtaskmgr` roots.  T418 has retired
   generic `app`, `session`, `broker` and `adapter-*` production roots;
   retained README-only directories are historical move markers. No generic
   `common`, `compat` or `win32api` component may be introduced.
2. `mvdm` is the sole complete selected OpenNT `base/mvdm` physical mirror;
   its manifest-selected executable slice is the MVDM host-runtime mirror.
   `opennt-host` is the sole original non-MVDM OpenNT host-service mirror and
   location for separately admitted load-only OpenNT inputs. It contains every
   separately accepted original owner package, not only the
   first Base VDM slice, and accepts only a required original slice after a complete package-boundary
   audit proves: a
   direct selected-MVDM caller; a retained original algorithm/state-machine
   benefit; a finite outgoing closure through public modern APIs, existing
   bounded adapters or a specifically admitted small adapter; and no need to
   import an NT4 system/product shell. CSR/CSRSS, NTDLL CSR transport, the
   full BaseClient/Kernel32 product, Win32k and USER/GDI server are stopping
   boundaries, not recursive import candidates. Kernel VDM may enter only as
   an individually admitted `opennt-host` semantic carrier when its original
   owner is outside MVDM; original MVDM owners stay in `mvdm-host`. For a
   direct MVDM consumer: it preserves original shape and has a finite
   standalone ABI, while an adapter owns only that ABI binding. Kernel
   scheduling, trap/fault installation, ring-0 V86 switching, private CSR
   transport and `CPU_30_STYLE` remain stopping boundaries. Every accepted and
   rejected candidate has a complete source/ABI/final-disposition record in
   the shared host tracker. Original MVDM support carriers/libraries remain in
   their original-relative `mvdm-host` paths; independent historical tools
   belong to `mvdm-tools`. A tool is never a host
   runtime dependency merely because it is independently buildable.
   `mvdm-softpc-firmware` owns only selected original immutable firmware/ROM/
   data inputs; it is neither a host-runtime library nor a machine executor.
   `ntvdm` stages an admitted immutable input through the selected backend
   binding. Executable `softpc.new/base/*` packages belong to `mvdm-host`.
   The narrow `opennt-abi/host-compat` island contains only named historical
   host ABI bindings; it owns no product policy or MVDM state.
   For source-function BFS, zero-degree is all original definitions in
   `mvdm-host` plus their transitive resolved call closure while each physical
   original definition remains selected OpenNT `mvdm` source already mirrored
   under a `mvdm-*` component. Existing but unreachable support, tool, firmware
   and guest definitions are not zero-degree. The rule creates no runtime link
   edge and matches source identity rather than function spelling.
3. The selected executable `mvdm/softpc.new` source composition is the sole
   production machine implementation and owns its original CPU, memory,
   firmware and PC-device semantics. `ntvdm` owns only the reached
   same-shaped SoftPC/CCPU, monitor, Win32, Redirector, WOW, VDD and debugger
   bindings. They preserve original name, ABI, ordering and failure shape, do
   not become an alternate provider, and never include Bochs or CPU30.
4. `ntvdm/monitor` owns the bounded user-mode `NtVdmControl`, `VDM_TIB`,
   V86-event and interrupt/fault binding. It binds worker-local state around
   imported MVDM code, does not recreate kernel/CSRSS internals, and fails
   unavailable operations deterministically.
5. `ntvdm` owns one active imported MVDM host context per worker process:
   its local session, leases, resources, completions/events and teardown. No
   raw local pointer enters guest state or a cross-executable ABI. Multiple
   worker processes are permitted; multiple imported MVDM contexts in one
   worker require a separate reentrancy/global-state audit.
6. `run16` owns CLI admission/parent behavior. `dtaskmgr` owns only native
   Console management presentation and client selection state. `basesrv` owns the versioned
   IPC endpoint, cooperative registration, identities, command queue and
   original BaseSrv-record assembly, including copied task snapshot and
   broker-authorized registered-task termination; it cannot acquire
   machine/BOP/guest semantics or recreate CSRSS. `dtaskmgr` never enumerates
   or controls unrelated Windows processes.
7. DOS child programs, COMMAND re-entry and multiple WOW16 tasks inside one
   machine are intra-session guest/task lifecycles, not separate VDM sessions.
8. `basesrv` is the per-user process boundary for cooperative VDM
    registration, stable identity, command queues, notifications, leases and
    cleanup. It may recover source-proven observable BaseSrv coordination
    contracts but must not recreate CSRSS, scan or control unrelated processes,
    or acquire machine/BOP/guest semantics.
9. Broker command records are versioned and fixed-width. They contain stable
    broker IDs, never native pointers, sender-local HANDLE values, local mapping
    tokens, guest pointers, CRT objects or cross-process callbacks. The
    owner-approved resource-transfer exception permits a separate authenticated
    OS-managed capability attachment for required file, pipe, event and process
    resources. Only the recipient's valid local reference may be materialized;
    it is not a cross-process identity. Preserve rights, aliasing, file position,
    lifetime and rollback. Console handles require a separate local binding.
10. Guest width and host width are orthogonal. Imported MVDM code observes
    original 16/32-bit ABI values; directly composable native x86 resources
    retain their original process-local carrier.
11. A fixed-width original ABI that cannot carry a native value may own the
    smallest source-shaped table required by that ABI. It must not become a
    shared session mapping/token service.
12. A native resource never enters guest state or broker command records.
    Broker resource attachments are limited to the rule 9 exception. Native
    x64 is not a product, build, runtime or acceptance target.
13. Numeric data is not an opaque identity. Lengths, offsets, times, flags,
    errors, registers and guest addresses retain original semantics and must
    receive explicit range and overflow validation.
14. Historical guest-pointer calls may expose a native pointer only through
    the `ntvdm/softpc` family with a checked synchronous mapping lease with address, span,
    access and epoch. No such pointer crosses an ABI or reaches asynchronous
    work.
15. `mvdm/dos/v86`, `mvdm/bin86`, `mvdm/wow16` and the separately sourced
   `opennt-host/base/win32/winnls/fontsup/system` fonts are complete load-only mirrors. Their source, objects,
    libraries and products never satisfy a host symbol. ntvdm loads
    manifest-selected immutable bytes through the selected backend binding.
16. The selected OpenNT tree is one package-scope union of the pinned OpenNT
    and OpenNT-4.5 MVDM baselines. Every target path has one selection;
    conflicts are decided at complete-package scope with provenance. Parallel
    edition roots and undocumented file-level hybrids are forbidden.
17. Recovery order is mandatory: audit an applicable project-owned current or
    quarantined owner candidate; directly composable original source; the
    smallest same-shaped adapter/build seam; a registered external-code
    intrusion; newly authored behavior only when no
    usable source remains. A candidate may be selectively copied only after
    per-file provenance, owner, dependency, behavior and test review; no whole
    component tree may be revived by default. Convenience and trace order do
    not justify skipping a rung.
18. Every mirror file is exact upstream, a registered true subset, or a
    registered same-shaped minimal modification. Every changed imported
    expression carries `DIVERGENCE:` and a component README register entry.
19. A material added body belongs in the named adapter/ABI family that owns
    its unavailable modern boundary. A new mirror path is permitted only for
    an actual original OpenNT file, at its original relative path, after the
    required provenance and package-boundary admission. It may be byte-exact,
    a registered true subset, or a registered minimal same-shaped adaptation.
    Project-invented files may not enter `mvdm` or `opennt-host`. A mirror
    change must belong to that file's original owner.
20. Do not introduce generic `compat`, `common`, `adapter-host` or
    `adapter-common` roots. A new specialist adapter requires a complete
    original owner-package and ABI audit plus explicit admission.
21. Modern host integration may use public Win32 filesystem, device, process,
    console, IPC and registry APIs under ordinary permissions. The product
    must not modify/rebuild Windows system components or require installation
    mutations to start or sustain itself.
22. The current host build profile is MSVC Win32/x86 `/MT`, with the original
    SoftPC CCPU40 executor selected by `CPU_40_STYLE`. Native x64 is not a
    current compilation, linking, runtime or acceptance profile; x64-only
    issues do not authorize a diff unless they demonstrate an
    architecture-neutral mapping-manager correctness defect. `CPU_30_STYLE` is the
    retired NT4 kernel-VDM V86-monitor contract and must not enter any
    project-owned compilation, linking, runtime, fixture or acceptance input.
    Any later cross-process interaction uses only an
    admitted fixed wire ABI.
23. Original SoftPC CCPU40 is the sole session machine backend. A session has
    no fallback or simultaneous executor. Every functional test records its
    x86 CCPU40 row. Pure source/build-audit work may state why it has no
    machine row; selected translation units compile and link on that profile.
24. `src.old/` and comparison repositories are evidence only and must not
    enter source, build, link, runtime or acceptance manifests.
25. The `mvdm-host` support paths may be linked only after the package/symbol
    tracker records their original consumer, exact interface shape, binding
    owner and x86/x64 disposition. `mvdm-tools` may never be linked into `ntvdm` or
    an MVDM host runtime. `mvdm-softpc-firmware` may not be compiled or linked
    as a host provider; `ntvdm` may stage its immutable inputs only through the
    selected machine backend. Executable `softpc.new/base/*` source packages
    belong to `mvdm-host`, not the firmware component.
26. Mirror similarity is a release property.  An original package may not be
    relocated into an adapter or replaced by autonomous code merely to reach a
    build.  Retain upstream path/name/control structure in the mirror; use a
    registered local `DIVERGENCE:` hook only for a minimal binding. A material
    added mechanism belongs in a named adapter/ABI family, never a mirror
    overlay.
27. NTVDMx64-derived declarations or missing-provider fallbacks must have a
    register row identifying their caller/interface shape, provenance, reason,
    x86/x64 disposition, mapping-manager use and removal condition. They live
    in the named adapter/ABI family, never as a mirror component or a new file
    below `mvdm` or `opennt-host`.
