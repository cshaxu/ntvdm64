# M0 T335 S1 — WOW32/Win16 source/ABI and profile rebaseline

## Current mirror identity

`ExportWowPackageBaseline.mjs` regenerated the complete current inventory of
`mvdm-host/{wow32,fax}` against the selected OpenNT-4.5 MVDM tree.

- 315 paths are retained in the current mirror inventory.
- 221 source/carrier paths are byte-exact to the selected OpenNT-4.5 source.
- The only source divergence is `wow32/wkfileio.c`, already registered as
  `MVDM-HOST-DIV-007`; it remains a source-shaped VDD-handle binding issue,
  not a new WOW provider.
- 93 paths have no canonical source counterpart because they are imported
  original intermediate/object/resource products beneath `obj/`. They are
  preserved evidence and `not-host-runtime`, never a host link input.
- The original manifests select 82 source translation units (77 WOW32 and
  five FAX) and three resources. The remaining 230 paths are declarations,
  build carriers, artifacts, or unselected product forms.

The generated [315-row current ledger](../operations/m0-t335-s1-wow-source-profile-ledger.tsv)
retains a SHA-256 identity/disposition row for every path. Its historic
222-path source-level predecessor remains [T291 S1's file ledger](../operations/m0-t291-s1-wow-file-subdomain-disposition-ledger.tsv);
the added 93 paths are artifacts rather than omitted current source bodies.

## Current interface boundary

The earlier fifteen-family interface walk remains source-valid because the
222 source/carrier paths are unchanged except for the registered `wkfileio.c`
binding. It is reinterpreted under the current CCPU40-only architecture:

- guest ranges are bounded `session.guest_memory` leases; host resources and
  persisted identity are `session.host_resource` mappings;
- original CCPU/SAS and `host_simulate` call forms go through the named
  SoftPC and monitor adapters, not a retired Bochs route;
- Base VDM command request, task/broker record and cross-process notification
  remain separate broker capabilities; the current one-session command source
  is not misrepresented as BaseSrv/WOWEXEC;
- private USER/GDI words, GDI drivers, CSRSS/kernel-VDM calls, VDD file
  association and cross-process shared-pointer layouts remain original
  boundary facts rather than candidates for an ad-hoc public lookalike.

The authoritative original caller/layout details remain in the [fifteen-family
ledger](../operations/m0-t291-s1-wow-interface-family-ledger.tsv). No family
has acquired a second mapping manager or raw native pointer/HANDLE transport.

## Selected next owner cohort

The first admissible code cohort is not a Win16 API or a trace-selected BOP.
It is the complete twelve-body original WOW32 thunk/frame/structure foundation:
`mapembed`, `walias`, `walloc16`, `wcall16`, `wcall32`, `wstruc` and their
same-subdomain peers. Its required precondition is a source-shaped
`CallBack16` / `VDMFRAME` transaction and bounded memory contract. The
existing `adapter-mvdm-host-out/wow` root is intentionally empty of provider
logic; S2 must prove its narrowly typed binding surface before any source body
is selected for a formal graph.

## Result and limit

S1 closes the current source, identity, profile and ownership baseline. It
does not enable `W32DllInitialize`, `CallBack16`, `GetNextVDMCommand`, WOWEXEC,
GUI/USER/GDI, FAX, a BOP route, or a Win16 guest load. S2 owns only the
complete callback/frame ABI and failure-contract design needed to decide
whether the original first cohort can be compiled unchanged.
