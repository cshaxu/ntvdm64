# M0 T284 S1 — historical-interface adapter convergence inventory

## Result

The pre-convergence OpenNT-facing adapter surface is now recorded at file and
public-callable-declaration granularity. The generated inventories are the
sole source of truth for later T284 source moves:

- `docs/etc/operations/m0-t284-s1-adapter-convergence-file-inventory-001.tsv`
  — 33 unique final ownership records: 22 current production files, seven
  superseded root-boundary documents, and four source-proven, currently empty
  specialist families.
- `docs/etc/operations/m0-t284-s1-adapter-convergence-public-symbol-inventory-001.tsv`
  — 71 exported/reached callable declarations or declaration carriers.
- `docs/etc/operations/m0-t284-s1-adapter-convergence-consumer-inventory-001.tsv`
  — all seven selected MVDM host consumer families and their required future
  dependency direction.

`tools/governance/Export-T284AdapterConvergenceInventory.ps1` regenerates the
three inventories from the current roots. It records a SHA-256 identity for
every currently live production input; it never reads, writes or selects an
MVDM body.

## Findings

- `adapter-win32` has nine production inputs. All are owned by the future
  `adapter-mvdm-host-out/win32` family. This includes its declaration-only
  `vdmapi.h`, `winbasep.h`, and `winconp.h` carriers: their forwarding role is
  retained, not mistaken for a monitor or console implementation.
- `adapter-softpc` has nine production inputs. They move as one
  `adapter-mvdm-host-out/softpc` family, with the existing constraint that
  source-shaped calls use only session-bounded services and typed
  `adapter-bochs` mechanics. No Bochs type becomes visible to `mvdm-host`.
- `adapter-vdm-monitor` has four production inputs. They move as one
  `adapter-mvdm-host-out/monitor` family. Their available subset stays
  synchronous; unimplemented NT kernel/CSR behavior remains a deterministic
  unavailable result.
- Redirector, WOW, VDD, and debugger currently have no production source or
  public header. Each is nevertheless a source-proven final owner package,
  recorded as `create-named-unavailable-family`. Their current placeholder
  root documents are separately recorded as `replace-with-final-family-readme`;
  S3 must create a small, explicitly unavailable family rather than treating
  a README-only root as a provider.
- `adapter-bop` is deliberately absent from these inventories. Its future
  name is `adapter-mvdm-host-in`, and it remains an inbound copied-frame /
  machine-event bridge rather than an outgoing historical MVDM interface
  facade.

## Dependency proof

Every final family follows only this direction:

```text
mvdm-host -> adapter-mvdm-host-out/{win32,softpc,monitor,redir,wow,vdd,debugger}
adapter-mvdm-host-out/softpc -> adapter-bochs -> bochs-core
```

The outgoing component has no direct `bochs-core` dependency. `adapter-bochs`
remains the only direct caller of `bochs-core`; source-shaped MVDM callers
remain in `mvdm-host`.

## S1 closure boundary

This evidence only admits the exact future ownership and dependency map. It
does not rename a directory, create `adapter-mvdm-host-out`, move source,
change an include, add an export, select a build input, or enable a historical
product capability. Those are separately ordered T284 work.
