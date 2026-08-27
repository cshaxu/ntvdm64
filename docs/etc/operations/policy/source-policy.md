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
`mvdm-tools`, shared support carriers/libraries belong to `mvdm-support`, and
selected `softpc.new` BIOS/ROM/data inputs belong to
`mvdm-softpc-firmware`. Exact declarations required from outside MVDM belong
to `mvdm-platform-abi`.
An original OpenNT package outside MVDM may enter the separate `opennt-host`
mirror only after complete-package audit proves that it has a clear original
host-service boundary and is actually required by a selected MVDM caller.
Every such file, dependency interface, build profile and final disposition is
added to the same host tracker that covers the 1,689 MVDM paths. A package-
specific `adapter-opennt-host` may supply a same-shaped modern binding only
after the imported source identifies a private host interface that cannot be
directly composed; it must not become a generic compatibility layer. The
shared declaration-only `mvdm-platform-abi` remains separate unless a tracked
declaration is proved private to one admitted `opennt-host` package.
Bochs 2.6, from `O:\repos.external\bochs-2.6-compat\bochs-2.6`, is the
approved third-party guest-machine backend, subject to the pinned-import and
source-identity requirements in `design/CODING.md` and the adoption record.
This is internal research; distribution/license review is deferred until a
release is considered. Later
source trees, `ntvdm64`, PCjs, and similar projects remain
comparative evidence only and cannot become runtime/acceptance dependencies.

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
