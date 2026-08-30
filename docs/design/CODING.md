# Source Layout

## Production roots

```text
src/
  mvdm-host/
  opennt-host/
  mvdm-tools/
  mvdm-softpc-firmware/
  mvdm-softpc-patch/
  mvdm-platform-abi/
  mvdm-guest/dos/v86/
  mvdm-guest/bin86/
  mvdm-guest/wow16/
  mvdm-guest/font16/
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

- `mvdm-host` contains only canonical selected MVDM host-runtime
  packages, retaining package-internal paths and filenames.
- `opennt-host` contains every complete selected original OpenNT host-service
  package outside MVDM, each retaining upstream-relative paths and filenames.
  Base VDM is its first accepted slice, not a limit on future admitted owner
  packages. Each package is admitted only with rows in the shared
  file/interface/dependency/build trackers and the external package-boundary
  ledger. The audit must show a direct `mvdm-host` caller, retained original
  service value and a finite outward modern-binding closure; a standalone
  convenience helper, a symbol hit or a recursively required NT4 product-shell
  package never qualifies.
- `mvdm-host` retains selected shared MVDM build/header carriers and original
  support libraries at their original relative paths (`inc`, `dirs`,
  `makefil0`, `oemuni`, and `suballoc`); no separate support component exists.
- `mvdm-tools` contains selected standalone MVDM tools and their
  resources (`vdmutils`). It is a production mirror with optional independent
  tool builds, never a main-program library.
- `mvdm-softpc-firmware` contains only selected immutable original MVDM
  firmware and machine-input paths (`softpc.new/bios`, `roms`, and `data`).
  It is never a host-runtime library or a second machine implementation. The
  executable `softpc.new/base/bios` and `base/keymouse` packages stay in
  `mvdm-host`; `app` selects immutable inputs for the chosen backend through
  its admitted source-shaped composition binding.
- `mvdm-platform-abi` contains exact declarations outside MVDM required by
  those packages; it has no implementation.
- `mvdm-softpc-patch` contains only individually provenance-registered
  NTVDMx64-derived SoftPC patch bodies. Each register row names the original
  caller, source identity, interface shape, mapping-manager use and removal
  condition. It reaches mechanics only through the same-shaped `softpc` family
  of `adapter-mvdm-host-out`.
- `mvdm-guest/dos/v86`, `mvdm-guest/bin86`, `mvdm-guest/wow16` and
  `mvdm-guest/font16` are the complete selected local DOS/V86, Bin86, WOW16
  and 16-bit-font carries. They remain data/load-only input roots and do not
  imply an unselected external source-universe import.
- `adapter-mvdm-host-in` contains only its declared selector-blind typed
  machine-event boundary. `adapter-mvdm-host-out` contains the named same-shaped historical
  interface families `win32`, `softpc`, `monitor`, `redir`, `wow`, `vdd` and
  `debugger`; a missing historical product interface is assigned to one of
  those families before a mirror source is changed to avoid it.
- `adapter-opennt-host` contains only the same-shaped package-private host
  interface family required by each owning `opennt-host` package. Its source
  subdirectories identify that original owner package. It cannot be used by
  `mvdm-host` directly or become a generic compatibility root.
- `session` contains neutral per-instance lifetime, mappings, resources,
  events and teardown.
- `broker` contains the versioned IPC client/server contract and per-user
  cross-process coordination state.
- `app` contains CLI admission and final wiring.

Historical MVDM build tools such as `tools16`, `bin86`, `convert` and
`dat2obj` belong under `tools/opennt`, not `src/`.

Before authoring or reimplementing a project-owned mechanic, audit existing
current and quarantined same-owner `adapter-*`, `app`, `session`
and related component code as a recovery/reference source. Reuse only the
individually selected, provenance-recorded portion that already satisfies final
ownership, dependency and mirror rules; never bulk-import a component tree or
revive a rejected semantic path.

For the source-function BFS, zero-degree consists of all original definitions
in `mvdm-host` and their transitive resolved call closure while each callee's
physical original definition remains in the selected OpenNT `mvdm` tree and is
already mirrored under a project `mvdm-*` component. Do not sweep every
tool, firmware, or guest definition into zero merely because it
exists locally: it must be reachable from that closure. This classification
does not change final build ownership. Resolve by selected physical definition
identity, never by a bare same-spelled function name.

## Host-width coding model

The current recovery build uses MSVC Win32/x86 `/MT`, with the original CCPU40
configuration as its sole selected profile. x64 is a later compatibility profile, not a current
formal-build gate. Cross-component and broker wire records use fixed-width
integer fields.
Native process-local implementation uses `uintptr_t`, `size_t`, `HANDLE` and
other pointer-sized platform types.

Opaque native resources never enter imported MVDM state. They are registered
in a session-owned `host_resource` or `completion_callback` mapping-manager
instance and represented to MVDM by a 32-bit surrogate. The allocator begins
with candidate zero, skips source-proven reserved sentinels, advances
monotonically, keeps reverse lookup and stale tombstones, and does not reuse an
ID during the session lifetime. The x86 build follows the same mapping path
and does not identity-map native 32-bit values.

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


## Build layout

Ninja is generated from the source-owner and package-selection manifests.
Disposable objects, libraries, executables and logs belong under
`build/<task-id>/<run-id>/`. Guest objects and libraries are packaging/loading
inputs only and never enter the host link. Formal verification currently
covers selected x86 CCPU40 compilation plus architecture-neutral token behavior;
x64 compatibility verification is deferred until the SoftPC/MVDM execution
path is connected.
