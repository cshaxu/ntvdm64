# Proposal — OpenNT MVDM owner expansion and original-component recovery

## Objective

Recover the remaining relevant OpenNT MVDM source/input packages into the
project's named original-code components without creating an omnibus `mvdm`
component, changing runtime semantics, or treating historical material as a
default build dependency.

## Architectural decision

The product has twelve source components:

- Original-code mirrors: `bx-core`, `opennt-guest`, `opennt-bop`,
  `opennt-host`, `opennt-softpc`, `opennt-utils`.
- Mechanical adaptation: `bx-mantle`, `adapter-bop`, `adapter-softpc`,
  `adapter-win32`.
- Project composition: `app`, `session`.

`tools/opennt` is an original tool closure, not a component. It may support
reproducible guest builds but never enters the modern product link graph.

`opennt-softpc` holds original BIOS/ROM/machine-contract source inputs; it is
not a new emulator and cannot impose OpenNT semantics on `bx-mantle` or
`bx-core`. `opennt-utils` accepts a source package only after an original
caller proves it is a utility rather than guest, BOP, host-product or machine
owner.

## Packets

1. **S1 — ownership/provenance census and component roots.** Complete the
   24-package MVDM census, ratify the 12-component model, and create
   production-only roots/READMEs. No body import or build-graph expansion.
2. **S2 — guest-owned `bin86` admission.** Classify, copy source-identically,
   hash and register every admitted `bin86` input beneath `opennt-guest`.
3. **S3 — SoftPC firmware/ROM initial mirror.** Admit `softpc.new/bios` and
   `softpc.new/roms` only after a per-file source identity/consumer list;
   retain opaque loading through app-to-mantle mechanics.
4. **S4 — utility-package decision.** Audit `suballoc` and the separated
   `vdmutils` products. Import only a source-identical package with a declared
   caller; put non-production reference material under `docs/etc/legacy_code`.
5. **S5 — optional host package preparation.** Audit VDD, debugger, fax and
   WOW host packages for their future `opennt-host` owner. Preserve their
   external-source provenance and package boundaries, but do **not** import
   them into `src/opennt-host` until a reached provider is admitted for actual
   composition.
6. **S6 — original tool closure.** Admit needed `tools16` inputs below
   `tools/opennt` with source hashes and a declared guest-build consumer.
7. **S7 — closure.** Reconcile owner manifest, README exception registers,
   formal Ninja graph and absence of obsolete/misowned roots.
8. **S8 — reachable host-source similarity repair.** Re-open T263 for the
   four currently composed host subsets: `nt_error.c`, `vrinit.c`,
   `vrnmpipe.c` and `vrputil.c`. For each, restore every currently admitted
   feature block to original OpenNT ordering, data layout and failure flow;
   move unavailable historical platform calls behind same-shaped
   `adapter-win32` or `adapter-softpc` facades; and mark an unconnected whole
   feature block as an explicit source-cited crop. This is a logical-block
   standard, not a byte-count target: retain original module/algorithm comments
   with every restored block where practical, but do not import diagnostics or
   otherwise unreachable product blocks merely to increase similarity. This packet neither imports
   dormant VDD/debug/FAX packages nor claims their product capabilities.

## Non-goals

- No Bochs CPU/device semantic change and no new Bochs intrusion.
- No BOP/provider feature implementation, DPMI/WOW/VDD/Redirector admission,
  guest behavior change, or default expansion of the formal link graph.
- No wholesale copy of `base/mvdm`; every import remains a source-policy,
  owner and provenance decision.

## Verification

Each import packet must prove upstream-relative path, filename and byte hash
or document every local `DIVERGENCE:` in the importing component README.
Closure repeats the inventory/governance gates and a formal Ninja graph check
that proves no new root is linked merely because it exists.
