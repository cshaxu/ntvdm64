# Proposal: Repository and neutral runtime foundation

## Purpose

Establish the governance and project-owned foundations needed by every later
rebootstrap package without importing Bochs or OpenNT provider code. This is
the first admissible rebootstrap candidate.

## Boundary

The scope is the thirteen-root owner/provenance/build manifest, neutral
`session` lifecycle, the one mapping-manager implementation, its three typed
per-session instances, bounded thread binding, and the versioned `broker`
wire contract. It contains no BOP selector, DOS, WOW, SoftPC, Bochs, Win32
provider, guest-image or imported MVDM service semantics.

## Proposed subtasks

1. **S1 — Owner and source manifest.** Record every production root, pinned
   baseline, package union decision, future library target, source admission
   state, `src.old` quarantine and x86/x64 build identity.
2. **S2 — Session lifecycle.** Create dependency-neutral instance creation,
   cancellation, teardown, resource ownership and bounded worker-context
   binding with no hidden current-session singleton.
3. **S3 — Compatibility object space.** Implement one mapping-manager type
   and instantiate separate `guest_memory`, `host_resource` and
   `completion_callback` tables per session. Prove monotonic allocation,
   sentinel handling, reverse lookup, stale tombstones and no reuse.
4. **S4 — Guest-memory lease contract.** Specify and test the checked
   synchronous address/span/access/epoch lease API. No native pointer may be
   retained, serialized or used asynchronously.
5. **S5 — Broker contract and host-width matrix.** Define versioned copied
   messages, stable broker IDs, disconnect cleanup and per-user access rules;
   compile the neutral graph under MSVC Win32/x86 `/MT` and x64 `/MT` and prove
   identical surrogate behavior.

## Exit criteria

- Every eventual production input has one owner/provenance/build disposition.
- Session and mapping tests run without Bochs or imported MVDM code.
- Both host architectures use the same surrogate32 allocation and lookup path;
  x86 has no identity pass-through.
- Broker messages reject native pointers/HANDLEs, local surrogates, guest
  pointers and CRT-owned objects.
- No source, include, build or runtime manifest uses `src.old/`.

## Non-goals

No broker service implementation, machine, guest mirror, original MVDM source
import, BOP dispatch or host-provider behavior is admitted here.
