# M0 T291 S1 — WOW owner-package recovery order

## Fixed package facts

- The current selected mirror has 222 files: 201 in `wow32`, 21 in `fax`.
- WOW32 has 77 active original provider bodies in eight source subdomains;
  eight source bodies are condition-unselected, 105 paths are declaration or
  table carriers, and eleven are original build/resource forms.
- FAX has four user-mode UI provider bodies, one private GDI driver body, four
  declarations and twelve build/resource forms.
- Comparing the current selected paths to the canonical OpenNT-4.5 source
  gives one changed mirror file: `wow32/wkfileio.c`, whose only changes are
  already registered under `MVDM-HOST-DIV-007`. The other 221 paths are
  byte-identical to their selected baseline.

## Required recovery order

1. **WOW32 thunk/frame/structure foundation** — 12 original active bodies.
   It consumes the existing `session.guest_memory`,
   `session.host_resource`, `adapter-mvdm-host-out/softpc` and
   `adapter-mvdm-host-out/monitor` contracts. It must first prove each
   CallBack16/VDMFRAME and bounded-address interface before a source body is
   enabled.
2. **WOW32 kernel/task/DOS session group** — 9 bodies. It depends on the
   frame foundation plus source-shaped session/broker records and existing DEM
   SFT/JFT boundary semantics. `wkfileio.c` remains the only registered mirror
   modification.
3. **WOW32 message/window, GDI/window-object and USER groups** — 40 bodies.
   Their private USER/GDI window-word and alias contracts require the named
   `adapter-mvdm-host-out/wow` owner assessment; no public Win32 lookalike is
   presumed equivalent.
4. **WOW32 shell/OLE/printing, multimedia and network groups** — 16 bodies.
   These retain their original algorithms and use public Win32 only through
   the already recorded adapter contract, after the common frame/session
   foundation closes.
5. **FAX UI group** — four original `wowfaxui` bodies. It depends on the WOW
   callback/frame group, current session/broker copied-record contract and
   per-call Winspool/registry assessment.
6. **FAX GDI driver** — `wowfax/wowfax.c` remains an explicit
   `not-host-runtime` historical graphics-engine/CSRSS driver unless a future
   product-owner decision admits an equivalent host-driver scope. It is not a
   candidate for silent user-mode replacement.

## Deferred product forms

Condition-unselected sources, declarations/tables and original build/resource
forms remain source evidence with their recorded `binding-only` or
`not-host-runtime` disposition. They cannot enter a host link graph merely
because their package is being audited.
