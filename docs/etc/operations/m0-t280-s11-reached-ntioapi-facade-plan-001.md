# M0 T280 S11 — reached `ntioapi` facade implementation

S11 implements the one source-shaped, reached-subset `adapter-win32` NT I/O
facade selected by S10.  It is an ABI-and-binding packet, not DEM provider
enablement: no original DEM translation unit, BOP route or guest trace enters
the build graph.

## Scope

- Add a single `adapter-win32` declaration carrier for the eleven S10 ledger
  rows, retaining their OpenNT source spellings and call shapes.
- Bind only the directly auditable modern `ntdll` entries; dynamic availability
  and every unavailable outcome must retain an explicit `NTSTATUS` result.
- Add x86/x64 host-only fixtures for layouts, available binding and unavailable
  failures.
- Preserve the rule that query records and `HANDLE`s are host-local.  No code
  in this packet calls a session mapping manager.

## Exclusions

No directory/volume backend, no object-manager emulation, no guest DTA copy,
no DEM body link, and no substitute Win32 success behavior.  A source body
may be admitted only after this facade has proved all reached declarations and
negative contracts on both architectures.

## Exit

The adapter presents one audited source-shaped NT I/O surface on x86 and x64;
each unavailable call is deterministic and explicit; all records and handles
remain host-local; and the exact OpenNT `ntioapi.h` carrier remains unchanged.
