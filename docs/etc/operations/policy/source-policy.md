# Source And Research Policy

OpenNT NT4 MVDM/NTDOS is the primary historical guest/service source family.
The pinned local OpenNT and OpenNT-4.5 MVDM trees are merged into one canonical
component superset: identical target paths collapse with dual provenance,
one-sided paths are included, and content conflicts are resolved once at
complete-package scope using source/build/resource/artifact lineage. Product
source never retains parallel edition roots or applies an undocumented
file-level precedence. The selected file's source path/hash and the rejected
alternative are recorded. DOS/WOW16 guest components also retain every
selected original source, resource, intermediate and binary product, but none
of their objects is a host link input.
Selected host-runtime packages remain together in the canonical `mvdm-host`
mirror; conceptual BOP, host and SoftPC-control categories
do not create parallel source roots. Independent historical tools belong to
`mvdm-tools`; shared support carriers/libraries remain at their
original-relative paths beneath `mvdm-host`; and selected immutable
`softpc.new` BIOS/ROM/data inputs belong to
`mvdm-softpc-firmware`. Executable `softpc.new/base/*` packages, including
`base/bios` and `base/keymouse`, remain in `mvdm-host` even when they
initialize firmware state. Exact declarations required from outside MVDM
belong to `mvdm-platform-abi`.
`mvdm-host` is recovered as a complete selected original host-package union,
not as a collection of trace-reached files. `opennt-host` is the single mirror
root for **all** separately accepted original OpenNT host packages outside
MVDM; BaseSrv/BaseClient VDM is only its first accepted slice. An original
OpenNT package outside MVDM may enter that mirror only after complete-package
audit proves that its **required original slice** is directly required by a
selected MVDM package, retains
substantial original state/algorithm value, and has a finite outgoing closure
through public modern APIs, an existing bounded adapter or a specifically
admitted small adapter. Every accepted **and rejected** candidate records its
source package, direct MVDM caller, complete outgoing boundary, file/interface
and build disposition, reason for acceptance/rejection, and superseded current
project implementation in the same host tracker that covers the 1,689 MVDM
paths. The audit never authorizes importing unrelated files from an accepted
source directory. CSR/CSRSS, NTDLL CSR transport, kernel VDM, full BaseClient/Kernel32,
Win32k and USER/GDI server are explicit stopping boundaries: their source may
be cited as evidence but is not recursively imported for host runtime.
A package-specific subfamily below `adapter-opennt-host` supplies a
same-shaped modern binding only for private host interfaces identified in its
accepted imported package; it must not become a generic compatibility layer.
The
shared declaration-only `mvdm-platform-abi` remains separate unless a tracked
declaration is proved private to one admitted `opennt-host` package.

For source-function dependency audit, zero-degree starts with every original
definition in `mvdm-host` and expands through its transitive resolved calls
only while each physical original definition remains within the selected OpenNT
`mvdm` source tree and is already mirrored under a project `mvdm-*` component.
The first resolved call outside that universe is one-degree. Existing but
unreachable support, tool, firmware and guest functions remain outside zero.
This is only a source-graph classification: it cannot make tools, firmware or
guest inputs host-runtime providers. The match is selected source
path/provenance/signature and content identity where applicable, never a
name-only match.
The product machine backend is the original SoftPC/CCPU30 and CCPU40 source
under `mvdm-host`; one declared profile is the sole production CPU and device
route for each session. CCPU40 is currently accepted; CCPU30 must receive its
own source, compile and bounded-run acceptance before paired-profile support
is claimed. Bochs 2.6
under `O:\repos.external\bochs-2.6-compat\bochs-2.6` is retired comparison
material only: it cannot be a source, build, link, runtime or acceptance
dependency.  Later source trees, `ntvdm64`, PCjs, and similar projects remain
comparative evidence only and cannot become runtime/acceptance dependencies.

### NTVDMx64 SoftPC patch exception

The owner has expressly approved a narrow exception for
`O:\repos.external\ntvdmx64`: an individually audited SoftPC patch body or
generated SoftPC build carrier may be copied only into
`src/mvdm-softpc-patch`. Each imported file must have a register row naming
the exact NTVDMx64 path and hash, the original SoftPC caller/interface it
serves, the original-source alternative, the reason that alternative lacks a
usable x86/x64 build carrier, mapping-manager handling if applicable, and
focused x86/x64 evidence. This exception does not admit NTVDMx64's injector,
loader, HAXM path, driver, CSRSS stubs, registry policy, host mutation, or its
product composition. `src.old` remains evidence only. Original SoftPC callers
remain in the `mvdm-host` mirror and may use only a minimal registered
`DIVERGENCE:` hook to enter the imported patch body.

Every imported file, copied fragment, ABI conclusion, or historical-source
change records its source tree/path/revision, role, notices present, rationale,
alternatives, and validation. The mandatory recovery ladder is: reuse
independently composable OpenNT source; reuse it through the smallest adapter,
compatibility seam or build shim; use a registered exceptional intrusion into
adopted external code only when essential mechanics cannot otherwise cross the
boundary; and author new behavior only when no usable source remains. The
record must demonstrate the preceding rung was attempted or name its specific
unavailable dependency; a trace hit, a convenient Win32 API, a temporary
buffer limit, or an easier rewrite is not such a demonstration. A
source-derived rehost belongs to the adapter/shim step, not to free-standing
new behavior: it must identify the original owner, source path, retained
layout/order/failure rules and the reason the original translation unit cannot
be composed. Prefer an OpenNT-shaped compatibility facade: keep the original
function/macro/structure form in the mirror, then map unavailable historical
dependencies through a named bounded session seam. Internal implementation may
be isolated, but same naming alone never proves behavioral equivalence. Where
mapping is required, the session owns separate fixed mapping-manager instances
rather than one shared numeric namespace. The
sole permitted historical guest-pointer compatibility route is the `softpc`
family of `adapter-mvdm-host-out` using an applicable `session`-owned mapping-manager
instance: it may return a native pointer for an
exact `GetVDMAddr`/`Sim32GetVDMPointer` call only under a bounded synchronous
mapping lease, and that pointer may not enter guest state, cross a component
ABI, outlive its call epoch, or reach asynchronous work. A facade must not
recreate an unbounded NT4 product shell. Minimize new replacement interfaces: use a public Win32 API directly
when it retains the reached contract, otherwise preserve the unavailable
Win32/CCPU/SoftPC call's signature and call shape in the facade where possible.
Every imported-source divergence records the exact unavailable
dependency and why the facade cannot express it. Owner-approved product
requirements may explicitly constrain this order. Prefer isolated adapters and
build shims to editing historical code.

Opaque host identity uses the same session-owned 32-bit surrogate path on x86
and x64; native-value identity pass-through is forbidden. Numeric data is not
tokenized. Cross-process broker messages use separate fixed-width broker IDs
and never contain a local surrogate, native resource or guest pointer.

User-approved binary research artifacts must live below `artifacts/`, include
a source/role/hash/size/redistribution manifest, and remain outside source,
default build inputs, runtime discovery, and release packaging.

### OpenNT-src-2 MVDM supplement exception

The owner has expressly approved one bounded exception for
`O:\repos.external\opennt-src-2\nt\private\mvdm`: paths absent from the
current `mvdm-*` mirrors may be copied byte-for-byte into their corresponding
original-relative component path, including historical `.obj`, `.lib`,
`.exe`, `.dll`, map, symbol, log and build-control products.  This is a
mirror-completeness exception, not a source-selection, build or runtime
admission.  It must never overwrite an existing selected/mirrored file; every
row carries source path, SHA-256, size, target path and its no-build/no-runtime
disposition in the import manifest.  The OpenNT/OpenNT-4.5 union remains the
only T301 BFS source-definition input unless separately re-admitted.
