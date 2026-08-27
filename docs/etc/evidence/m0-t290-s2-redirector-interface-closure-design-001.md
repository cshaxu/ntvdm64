# M0 T290 S2 — Redirector same-shaped external-interface closure design

S2 establishes the smallest legal boundary for every external family reached
by the original Redirector package.  The complete decision record is the
[external-interface recovery ledger](../operations/m0-t290-s2-redirector-external-interface-recovery-ledger.tsv).

The source-first decisions are:

- Keep the original `vdmredir` provider bodies and their dispatch/order.
- Bind transient DOS/protected pointers only through the existing same-shaped
  SoftPC facade and session-scoped lease/copy mechanism; no source body may
  retain an alias or write a host pointer into guest storage.
- Bind public named-pipe, mailslot and selected NetAPI operations behind the
  existing outgoing Win32/Redirector boundary, preserving original call and
  failure order rather than replacing provider policy.
- Let `session` own handles, cancellation and completion records.  COMMAND's
  existing child broker is a consumed dependency, never recreated by this
  package.
- Reserve physical IRQ/EOI delivery to selector-blind `adapter-bochs`; no
  Redirector source directly controls machine state.
- Retain original unavailable outcomes where no legal modern equivalent exists:
  VDD user hooks, private DLC/LLC `AcsLan`/`NtAcsLan`, and unadmitted
  guest-visible monitor projections.  These are explicit branch outcomes,
  not unbounded adapter rewrites.

S3 can now compose only lifecycle, COMMAND-consumer and synchronous
named-pipe bodies after it adds the declared same-shaped bindings.  S4 and S5
remain responsible for the asynchronous and network/device groups identified
above.  No `57:xx` ingress is enabled by this design record.
