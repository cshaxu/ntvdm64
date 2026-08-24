# M0 Td — Ten-Component Architecture Confirmation

## Decision

The project uses exactly ten production source components, organized into
three responsibility strata:

- **Original-source mirrors:** `bx-core`, `opennt-guest`, `opennt-bop`, and
  `opennt-host`.  Each preserves source identity where the original unit is
  available.  Every necessary edit has a local `DIVERGENCE:` marker and an
  entry in that component's `README.md` exception register.
- **Mechanical adapters:** `bx-mantle`, `adapter-bop`, `adapter-softpc`, and
  `adapter-win32`.  `bx-mantle` is Bochs-only; `adapter-bop` is selector-blind
  copied-frame transport; `adapter-softpc` preserves reached SoftPC/CCPU/SAS
  interface shape over bounded Bochs mechanics; and `adapter-win32` preserves
  reached historical Win32 interface shape over supported public Win32 APIs.
- **Project-authored composition:** `app` is the CLI entry and sole final
  composition owner.  `session` is the dependency-free, neutral lifecycle,
  identity, resource/event and teardown foundation; it is not an upward
  product layer.

The authoritative dependency direction is:

```text
app -> opennt-guest
app -> bx-mantle -> bx-core
app -> session
app -> adapter-bop -> opennt-bop -> opennt-host
opennt-bop -> adapter-win32
opennt-bop -> adapter-softpc -> bx-mantle -> bx-core
opennt-bop -> session
opennt-host -> adapter-win32 | adapter-softpc | session
adapter-bop -> session
adapter-win32 -> session
```

No dependency may reverse this diagram.  In particular, Bochs and its mantle
remain OpenNT/DOS/VDM/WOW/Win32-blind; `adapter-softpc` and `adapter-bop` do
not interpret BOP meaning; and `session` does not acquire component-specific
vocabulary or depend on a product component.

## Reconciled Authorities

- `docs/design/ARCHITECTURE.md` defines the ten-component ownership model and
  dependency graph.
- `docs/design/CODING.md` defines the production-only component roots and the
  source-identity/DIVERGENCE requirements.
- `docs/rules/ARCHITECTURE.md` and `docs/rules/CODING.md` make the boundary and
  source-first constraints enforceable.

This Td introduces no product source, build-graph behavior, numeric T
allocation, or change to active `M0 T261 S8`.  It records the owner-approved
architecture confirmation after the authorities were cross-read.

## Verification

- Cross-read the four current architecture/coding authorities above.
- Confirmed the declared source roots in `docs/design/CODING.md` enumerate
  exactly the ten production components and retire transitional `src/cli`,
  `src/bx-vdm`, and generic `src/opennt` roots.
- Run documentation-governance verification, documentation-inventory refresh,
  and `git diff --check` for this documentation-only delivery.
