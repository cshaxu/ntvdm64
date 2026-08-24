# Architecture Rules

1. Bochs 2.6 owns guest CPU, RAM/ROM, firmware and PC-device mechanics; guest OpenNT owns DOS/WOW/COMMAND behavior; `opennt-bop` owns routing to the original OpenNT host provider where it can be independently composed, and the source-derived replacement only where that historical host composition is unavailable. Neither Bochs nor guest code may absorb the other's responsibility.
2. The Bochs/OpenNT bridge is the only integration point. It uses versioned fixed-width records, validated guest-memory ranges and explicit stop/result dispositions; no host pointer, C++ object or cross-architecture function pointer crosses it.
3. A local OpenNT recovery requires a reached OpenNT caller, owner analysis, data-layout and failure-behavior evidence, and a bounded fixture. Its implementation decision is made in this mandatory order: (a) reuse independently composable original OpenNT source; (b) reuse that source through the smallest adapter, compatibility seam, or build shim needed to replace unavailable host composition; (c) make a registered, exceptional intrusion into adopted external code only when neither preceding route can provide an essential mechanical boundary; (d) author new behavior only when the required historical source or usable behavior is absent. This is an implementation hierarchy, not a choice among equivalent designs: a convenient transcription, trace observation, available modern API, or temporary capacity boundary never justifies skipping an earlier rung. At step (b), the preferred form is an **OpenNT-shaped compatibility facade**: original source files retain their original function/macro/structure shape while a named compatibility header maps the unavailable historical dependency to a bounded, session-owned implementation. The internal implementation may be safely isolated, but it must preserve the reached call site's calling convention, layout, ordering and failure rule. Where session mapping is required, isolation is by separately owned mapping-manager instances, not a shared numeric namespace. A source-derived rehost is permitted only when that facade cannot compose the original translation unit; it must record why, and must not become a permanent substitute merely because it is functional. The owner may explicitly override this order for a named product requirement, including the CLI, Bochs-for-SoftPC substitution, and unpack-and-run host non-intrusion.
4. An OpenNT mirror or host seam cannot become a CPU, firmware, device, DOS-kernel/filesystem or WOW implementation. `opennt-bop` may dispatch its declared source-derived host-service catalogue, but must not become a Bochs CPU/device substitute or an unbounded DOS product.
5. `ntvdm64` and other comparison repositories remain read-only evidence. Bochs 2.6 is the sole approved third-party internal-research runtime backend once its pinned import record passes T95.
6. Default product targets cannot depend on unreviewed artifacts, fixtures, or an unpinned third-party source snapshot.
7. Bochs is deny-by-default. Each enabled feature needs a reached first-profile OpenNT caller, named Bochs owner, boundary rationale, explicit admission record, and focused negative test. Upstream availability is not a reason to enable it.
8. Preserve original OpenNT service ownership, calling conventions, layouts, dispatch order, and failure behavior. Bochs and both adapters may carry documented bounded contracts but may not recreate that behavior.
9. The product has one host-integration contract: the applicable original OpenNT service behavior, recovered through the mandatory source-first ladder. Project-authored mutation profiles, including Direct, Readonly, Overlay and Virtual, are not product behavior. Superseded v1 implementations and their tests are retired from the working tree when their current route is code complete; version history is the comparison record. They may neither be selected nor extended.
10. Historical recovery uses a mandatory source-recovery precedence: reusable
   OpenNT source first; its smallest contract-preserving adapter/shim second;
   registered external-code intrusion third; newly authored behavior only when
   no earlier route is usable.  CLI integration may use supported host APIs as
   capabilities of the selected OpenNT-owned service, without modifying the
   host merely to install or run this product.
11. A compatibility facade may not recreate an unbounded historical product
    shell behind a familiar name. An `adapter-softpc`-owned, bounded mapping lease may
    return a native pointer for the exact historical `GetVDMAddr` or
    `Sim32GetVDMPointer` call shape, but only while the synchronous admitted
    call is active and only for validated guest RAM. That pointer never crosses
    the bx↔machine ABI, enters guest state, survives its mapping epoch, or
    reaches an asynchronous worker. A facade never exposes a Bochs object or
    silently treats `NtVdmControl`, CCPU/SAS, CSR/BaseSrv, or emulator-hook
    calls as equivalent successes. Each facade covers only the reached,
    source-proven session contract; the remainder is explicit unavailable or
    deferred behavior.
12. A project-defined interface, data layout or algorithm for OpenNT recovery
    is an explicit exception, not an alternate normal route. Before it is
    authored, its record must prove both direct source reuse and the
    same-shaped compatibility facade infeasible, define the smallest
    replacement contract and failure behavior, and state whether it will be
    migrated, retained only as generic mechanics, or deleted. Otherwise an
    implementation must retain the original interface form and reproduce the
    reached original control/data/failure logic as closely as the platform
    permits.
13. Replacement boundaries are minimized by interface count as well as code
    size. Public Win32 APIs are used directly where they retain the original
    contract. For unavailable Win32, CCPU or SoftPC calls, retain the original
    signature/call shape in the compatibility facade whenever possible; a new
    shaped boundary requires rule 12's exception and may expose only the
    state required by the reached call.
14. Intrusion is minimized independently for adopted Bochs `bx-core` and
    imported OpenNT mirrors (`opennt-guest`, `opennt-bop`, `opennt-host`, and
    tracked BOP-dependencies). Prefer an `adapter-softpc`, `adapter-win32`, or
    `opennt-host` same-shaped facade that preserves the original call shape.
    An edit to `bx-core` or imported OpenNT source requires a source-proven
    necessity, the smallest feasible diff, and the component's divergence or
    exception record; convenience or a local trace result is never sufficient.
15. The target source owners are exactly `bx-core`, `bx-mantle`,
    `opennt-guest`, `opennt-host`, `opennt-bop`, `adapter-bop`,
    `adapter-softpc`, `adapter-win32`, and `app`. Dependencies flow from `app`
    through `adapter-bop`'s typed, selector-blind ingress and declared OpenNT/adapter
    contracts to `bx-mantle` and then `bx-core`; `opennt-guest` is a guest-image
    input and does not become a host-provider library. `opennt-bop` alone owns
    BOP source meaning. `adapter-bop` is selector-blind copied-frame transport,
    while `adapter-softpc` is selector-blind, same-shaped
    SoftPC/CCPU mechanical recovery over Bochs; `adapter-win32` is an
    unavailable-Win32 facade, not an OpenNT service owner.
16. The local-intrusion registers are component-owned. `bx-core/README.md`,
    `opennt-guest/README.md`, `opennt-bop/README.md`, and `opennt-host/README.md`
    record every edit to their imported source and every source-derived component.
    `opennt-bop` mirror edits additionally retain the original call form and mark
    each changed expression with `DIVERGENCE:`. `bx-mantle` may crop and compose
    native Bochs mechanisms but may not acquire OpenNT, DOS, VDM, WOW or Win32
    meaning.
