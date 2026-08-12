# T153 S1 x86 fast-I/O provider contract 001

## Decision

The missing historical x86 fast-I/O composition is recoverable as one
contained, source-derived **fast-I/O provider family**, not as a Bochs change,
a `50:16` alias, a MIPS-state workaround, or an ambient host filesystem.

The visible OpenNT tree provides the guest-side caller ABI but no original
fast worker. The provider must therefore preserve original call/continuation
shape and use the already-admitted finite read-only namespace only as its CLI
capability backend.

## Complete family disposition

| Service | Original visible disposition | CLI profile disposition | Rationale |
| --- | --- | --- | --- |
| `50:42` `SVC_DEMFASTREAD` | `demNotYetImplemented`, CF clear/no I/O | Source-derived fast-read provider for declared read-only namespace assets only | The reached x86 path treats CF clear as completion; a real load needs bytes and an actual byte count. |
| `50:43` `SVC_DEMFASTWRITE` | `demNotYetImplemented`, CF clear/no I/O | Explicitly unavailable; retain original-unavailable provider | The finite profile owns no writable host capability. Enabling it would expand the namespace boundary. |

This is a whole fast-I/O family decision. It neither silently implements write
nor leaves a second special-case handler outside the BOP plane.

## Source-derived fast-read ABI

The call site in `handle.asm:448-474` establishes the BOP input state before
`50:42`:

- `AX:BP`: historical file-handle identity;
- `BX:SI`: current file position, used only if ZF is clear;
- `CX`: requested byte count;
- `DS:DX`: guest destination;
- ZF: `sf_nt_seek` contract (set means no preceding seek is needed).

The provider validates all values from the copied exception snapshot and
instruction window; it owns no guest pointer or host handle. On success it:

1. resolves only the profile-declared readonly namespace token;
2. honours the ZF/`BX:SI` seek convention used by `demRead`;
3. copies at most `CX` bytes through the existing checked bulk-result
   transaction to `DS:DX`;
4. resumes at the four-byte BOP continuation with `AX = actual bytes read`
   and CF clear.

The original DOS caller then, unchanged, advances the SFT position by AX and
returns the same actual count. A short read (including EOF zero) is successful
with its actual AX value, matching the documented normal `demRead` result
shape. This is the source-derived component's explicit compatibility rule,
not an assertion that the unavailable historical stub performed it.

For an unknown token, invalid span, unsupported profile file or failed
transaction preparation, it returns an explicit CF-set normal-read-style
error with no payload/write. That is preferable to the historical stub's
false CF-clear completion because the service is being deliberately selected
as a CLI capability provider; it prevents an uninitialized guest image from
being represented as a successful read.

## Ownership and integration

- The provider is adapter-owned and must be selected through existing common
  BOP ingress, registry and DEM-plane classification.
- It reuses the current `readonly_namespace`, session and generic bulk-result
  machinery. No new Bochs ABI, RAM primitive, CPU branch, device, firmware,
  guest structure or host path is required.
- The existing original-unavailable provider remains the default disposition.
  A declared read-only profile selects the fast-read provider *before* that
  fallback only for `50:42`; `50:43` remains in the fallback.
- The provider must be named as a source-derived fast-I/O component, not as
  `demRead`, even if it shares an internal generic readonly read primitive.

## Required implementation/test package

T154 may implement only this contract:

1. add a dedicated fast-I/O provider/module and register it through the
   existing plane/catalog path;
2. route `50:42` only for an initialized declared boot namespace;
3. prove full read, short read/EOF, copied payload, AX/CF, seek/ZF handling,
   invalid handle/span rejection and no-write-on-failure;
4. prove `50:43` remains original-unavailable and no unapproved service is
   captured;
5. rebuild only adapter/CLI objects using the existing engine derivative;
6. make one separately admitted bounded runtime observation only after static
   and focused tests close.

No runtime execution, source code, ABI or build change is made by T153.
