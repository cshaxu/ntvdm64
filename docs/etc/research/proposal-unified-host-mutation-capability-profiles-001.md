# Proposal: Unified Host Mutation Capability Profiles

## Decision

Define and implement one shared bx-vdm/CLI capability-profile ABI for every
operation that can observe or change host-backed state.  The ABI is selected
once per VDM session and is consumed by owner packages; a BOP handler must not
invent its own direct, readonly, or shadow behavior.

This is an architectural prerequisite for direct-host DEM/COMMAND recovery,
Redirector networking, and every future device or Registry capability.  It
does not implement a particular BOP family or restore historical NTVDM host
installation mechanisms.

## Profile semantics

| Profile | Host mutation behavior | Session behavior |
| --- | --- | --- |
| `direct` | Execute the admitted existing user-mode Win32/NT operation. Preserve access-denied, privilege, sharing and device failures. | Session records only opaque capability state and source-defined DOS result data. |
| `readonly` | Reject every mutation before the host API is called. | Return the source-derived mutation failure; reads/searches use the admitted host view. |
| `volatile-overlay` | Do not call a host mutation API. | Write the corresponding namespace, metadata, time, context or other overlay state owned by this VDM; release it at teardown. |
| virtual volume | No host backing for that volume. | All state is guest/session owned; it cannot silently replace an admitted host volume. |

`direct` is the default when a capability is admitted.  It is not a license to
change Windows installation state: the product remains unpack-and-run and may
not patch/rebuild Windows, install drivers/services/VDDs, or perform
installation-style Registry mutation.

## Mutation classes

Every capability endpoint has exactly one mutation class and one state owner:

1. namespace/content: files, directories, rename, deletion and file bytes;
2. file metadata: attributes, timestamps, locks and sharing state;
3. session context: DOS current directory, environment and per-drive state;
4. host-global state: system date/time, permitted Registry values and network
   mappings;
5. device/raw-media state: volume/device control and absolute media I/O; and
6. IPC/network state: pipes, mailslots, network uses and asynchronous
   completions.

Endpoints whose original role only controls guest CPU, machine, firmware,
debugger, or engine mechanics use the explicit `none/mechanical` class. They
are not silently assigned a host mutation profile; their disposition remains
deferred until their native owner package is admitted.

For example, DEM `SetLocalTime` is host-global state: direct calls
`SetLocalTime` and preserves privilege failure; readonly rejects it; overlay
updates only a VDM virtual clock. `SetFileTime` is file metadata and must use
the same profile selection, but its overlay writes file-metadata state rather
than the virtual clock.

## Boundary and invariants

```text
CLI profile selection
  -> bx-vdm shared mutation-capability record
  -> owner package provider (DEM / COMMAND / Redirector / device)
  -> copied request/result or overlay state
  -> admitted Win32/NT operation only in direct profile
```

- Raw host handles, callback pointers, host memory and mutable Win32
  structures never cross into guest RAM.
- A readonly or overlay provider may not bypass the shared profile record to
  call a host mutation API.
- Overlay state is scoped to one VDM, has a defined teardown path, and must
  not change host persistence or host-global state.
- A host API that is unavailable, privilege-limited or unsupported remains an
  explicit source/ABI audit result; profile selection cannot fabricate
  success.

## Proposed package sequence

1. Define fixed-width profile, mutation-class, policy and opaque capability
   state records, with positive and invalid-selection tests.
2. Define owner registration and enforcement: direct/readonly/overlay/virtual
   dispatch cannot be chosen ad hoc by a BOP leaf.
3. Implement session-owned overlay primitives for namespace/content, metadata,
   virtual clock and context; prove teardown loses all changes.
4. Add policy-gated direct host adapters and readonly failure adapters by
   mutation class, retaining exact host failure data only inside bx-vdm.
5. Migrate current boot/readonly namespace, synthetic CWD and partial mutation
   fallbacks to the shared ABI; remove duplicated or bypassing paths.
6. Re-run the BOP owner-package audit and admit DEM, COMMAND, Redirector and
   device packages only against the common capability ABI.

## Acceptance evidence

- each mutation class has one owner and no direct host write bypass from
  readonly/overlay providers;
- direct, readonly, overlay and virtual-volume positive/negative tests cover
  file content, metadata, CWD, virtual clock and one privilege-sensitive host
  operation;
- overlay teardown proves no host mutation and no retained session state;
- direct mode proves ordinary permitted host mutation plus access/privilege
  failure propagation; and
- current BOP/provider ledger is reconciled to the common profile ABI without
  claiming a BOP family is thereby implemented.
