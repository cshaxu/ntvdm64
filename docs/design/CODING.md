# Source Layout

## Production roots

```text
src/
  bochs-core/
  mvdm-host/
  opennt-host/
  mvdm-support/
  mvdm-tools/
  mvdm-softpc-firmware/
  mvdm-platform-abi/
  mvdm-guest/dos/v86/
  mvdm-guest/bin86/
  mvdm-guest/wow16/
  mvdm-guest/font16/
  adapter-bochs/
  adapter-mvdm-host-in/
  adapter-mvdm-host-out/
  adapter-opennt-host/
  session/
  broker/
  app/
```

The eight original mirror roots preserve selected upstream-relative paths after
re-rooting and maintain component README exception registers. One source
component may generate several libraries. Production roots contain production
inputs only; tests, examples, reference copies and historical experiments stay
under `tests/` or `docs/etc/legacy_code/`. `src.old/` is quarantined comparison
material and never a source, build, link or runtime input.

## Owner placement

- `bochs-core` contains the adopted Bochs mirror only.
- `mvdm-host` contains only canonical selected MVDM host-runtime
  packages, retaining package-internal paths and filenames.
- `opennt-host` contains only a complete selected original OpenNT host-service
  package outside MVDM, retaining its upstream-relative paths and filenames.
  It is admitted only with rows in the shared file/interface/dependency/build
  trackers and the external package-boundary ledger. The audit must show a
  direct `mvdm-host` caller, retained original service value and a finite
  outward modern-binding closure; a standalone convenience helper, a symbol
  hit or a recursively required NT4 product-shell package never qualifies.
- `mvdm-support` contains selected shared MVDM build/header carriers
  and original support libraries (`inc`, `dirs`, `makefil0`, `oemuni`, and
  `suballoc`). It has no implied default host link edge.
- `mvdm-tools` contains selected standalone MVDM tools and their
  resources (`vdmutils`). It is a production mirror with optional independent
  tool builds, never a main-program library.
- `mvdm-softpc-firmware` contains selected original MVDM firmware and machine
  input paths (`softpc.new/base/bios`, `bios`, `roms`, and `data`). It is an
  immutable mirror input carrier, never a host-runtime library or a second
  machine implementation; only `adapter-bochs` may consume an admitted,
  manifest-selected input.
- `mvdm-platform-abi` contains exact declarations outside MVDM required by
  those packages; it has no implementation.
- `mvdm-guest/dos/v86`, `mvdm-guest/bin86`, `mvdm-guest/wow16` and
  `mvdm-guest/font16` are the complete selected local DOS/V86, Bin86, WOW16
  and 16-bit-font carries. They remain data/load-only input roots and do not
  imply an unselected external source-universe import.
- `adapter-bochs` contains Bochs-only composition and is the only caller of
  `bochs-core`.
- `adapter-mvdm-host-in` contains only its declared selector-blind typed
  machine-event boundary. `adapter-mvdm-host-out` contains the named same-shaped historical
  interface families `win32`, `softpc`, `monitor`, `redir`, `wow`, `vdd` and
  `debugger`; a missing historical product interface is assigned to one of
  those families before a mirror source is changed to avoid it.
- `adapter-opennt-host` contains only the same-shaped
  BaseSrv/private-host interface family required by its `opennt-host` caller.
  It cannot be used by `mvdm-host` directly or become a generic compatibility
  root.
- `session` contains neutral per-instance lifetime, mappings, resources,
  events and teardown.
- `broker` contains the versioned IPC client/server contract and per-user
  cross-process coordination state.
- `app` contains CLI admission and final wiring.

Historical MVDM build tools such as `tools16`, `bin86`, `convert` and
`dat2obj` belong under `tools/opennt`, not `src/`.

Before authoring or reimplementing a project-owned mechanic, audit existing
current and quarantined same-owner `bochs-core`, `adapter-*`, `app`, `session`
and related component code as a recovery/reference source. Reuse only the
individually selected, provenance-recorded portion that already satisfies final
ownership, dependency and mirror rules; never bulk-import a component tree or
revive a rejected semantic path.

## Host-width coding model

Both MSVC Win32/x86 and MSVC x64 builds use `/MT` and the same logical code
path. Cross-component and broker wire records use fixed-width integer fields.
Native process-local implementation uses `uintptr_t`, `size_t`, `HANDLE` and
other pointer-sized platform types.

Opaque native resources never enter imported MVDM state. They are registered
in a session-owned `host_resource` or `completion_callback` mapping-manager
instance and represented to MVDM by a 32-bit surrogate. The allocator begins
with candidate zero, skips source-proven reserved sentinels, advances
monotonically, keeps reverse lookup and stale tombstones, and does not reuse an
ID during the session lifetime. The x86 build follows the same path and does
not identity-map native 32-bit values.

Guest 16:16 and linear32 addresses use the separate `guest_memory` instance.
A synchronous historical pointer API may receive a native pointer only through
a checked address/span/access/epoch lease. The pointer is not serialized,
retained by asynchronous work or passed across a component ABI.

Only identities are tokenized. Numeric length, offset, flag, time, error,
register and guest-address fields keep their original meaning. Arithmetic is
validated in a wider temporary type before narrowing. Native structures that
contain pointers or HANDLEs are materialized and translated in the owning
adapter.

## Session and broker code

Every project-owned stateful API takes or is bound to an explicit session
instance. Thread entry to imported MVDM code binds the current monitor context
and unbinds it on exit. No project-global current machine, mapping manager or
resource table is allowed.

Broker messages contain only versioned copied fields and broker-owned IDs.
They never contain local surrogate IDs, native HANDLEs/pointers, guest
pointers, C++ objects or cross-process callbacks. Process discovery is
cooperative registration with leases, not arbitrary process enumeration.

## Mirror and overlay practice

An imported production file is exact upstream, a true subset, or a
same-shaped minimal modification. Each crop or changed expression is marked
`DIVERGENCE:` and indexed in the component README. If more than half the
retained source differs, or an inserted semantic body needs more than three
executable lines, move that implementation to the matching private
`*-overlay` and leave the smallest registered call boundary. Only the matching
mirror may call or link its overlay.

`bochs-core` is measured only against the pinned upstream Bochs 2.6 tree.
First compress any recovered historical difference into an existing upstream
call boundary; then place material retained mechanics in `bochs-core-overlay`.
A prior project copy is evidence for a possible behavior, never a substitute
mirror baseline.

## Build layout

Ninja is generated from the source-owner and package-selection manifests.
Disposable objects, libraries, executables and logs belong under
`build/<task-id>/<run-id>/`. Guest objects and libraries are packaging/loading
inputs only and never enter the host link. Formal verification covers x86 and
x64 compilation plus architecture-neutral token behavior.
