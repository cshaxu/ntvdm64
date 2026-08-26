# Source Layout

## Production roots

```text
src/
  bochs-core/
  opennt-mvdm-host/
  opennt-platform-abi/
  opennt-guest-dos/
  opennt-guest-wow16/
  adapter-bochs/
  adapter-bop/
  adapter-softpc/
  adapter-win32/
  adapter-vdm-monitor/
  session/
  broker/
  app/
```

The five original mirror roots preserve selected upstream-relative paths after
re-rooting and maintain component README exception registers. One source
component may generate several libraries. Production roots contain production
inputs only; tests, examples, reference copies and historical experiments stay
under `tests/` or `docs/etc/legacy_code/`. `src.old/` is quarantined comparison
material and never a source, build, link or runtime input.

## Owner placement

- `bochs-core` contains the adopted Bochs mirror only.
- `opennt-mvdm-host` contains the canonical selected non-guest/non-tool MVDM
  packages, retaining package-internal paths and filenames.
- `opennt-platform-abi` contains exact declarations outside MVDM required by
  those packages; it has no implementation.
- `opennt-guest-dos` and `opennt-guest-wow16` contain complete selected guest
  source, resources, build descriptions, intermediates and original products.
- `adapter-bochs` contains Bochs-only composition and is the only caller of
  `bochs-core`.
- `adapter-bop`, `adapter-softpc`, `adapter-win32` and
  `adapter-vdm-monitor` contain only their declared same-shaped or typed
  mechanical boundaries.
- `session` contains neutral per-instance lifetime, mappings, resources,
  events and teardown.
- `broker` contains the versioned IPC client/server contract and per-user
  cross-process coordination state.
- `app` contains CLI admission and final wiring.

Historical MVDM build tools such as `tools16`, `bin86`, `convert` and
`dat2obj` belong under `tools/opennt`, not `src/`.

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

## Build layout

Ninja is generated from the source-owner and package-selection manifests.
Disposable objects, libraries, executables and logs belong under
`build/<task-id>/<run-id>/`. Guest objects and libraries are packaging/loading
inputs only and never enter the host link. Formal verification covers x86 and
x64 compilation plus architecture-neutral token behavior.
