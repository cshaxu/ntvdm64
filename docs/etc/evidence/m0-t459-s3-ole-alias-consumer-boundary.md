# M0 T459 S3 — OLE alias consumer boundary

**Status:** closed. This is a zero-production-diff reconciliation of the
selected WOW32 OLE compatibility surface. It does not claim guest OLE server
or client interoperability.

## Reached host contract

The selected formal x86 graph compiles original `wole2.c` once under the
CCPU40 provider configuration. Its only OLE-specific execution contract is a
fixed 32-slot mapping between a foreign native thread ID and a selector-shaped
Win16 `HTASK`. Original `walias.c` creates/reuses it when a foreign thread
must be presented to WOW; `wuwind.c` resolves it for a window relationship;
`wkernel.c::WK32GetModuleFileName` consumes it for the bounded image-name
result. This is the reachable host-side consumer family; it is not a general
OLE automation or COM service.

The current exact-owner witness passed at `build/t459-s3/ole-alias-r2`:

```text
WOW_MODULE_ALIAS_BOUNDS_AND_LIFETIME_OK
```

It recompiles the selected original body from the production command and
proves the selector encoding, lookup, bounded full/truncated/one-byte image
output, 32-entry oldest-slot reuse, explicit withdrawal and stale identity
rejection by process ID and creation time. No native handle or pointer is
stored in or returned to guest state. The same source contract was previously
accepted in T452; S3 re-runs it against the current graph rather than adding a
second alias owner.

## Explicit OLE stopping boundary

`OLECLI`, `OLESVR` and `MCIOLE16` are immutable Win16 guest libraries. They
are not selected by the `wow32.dll` production graph and no current host call
to them exists beyond ordinary Win16 loading/DDE execution. The repository has
no reached public, finite COM API seam that could replace their client/server
protocol. Treating the 32-slot `HTASK` alias helper as permission to add such
a provider would create a second OLE/DDE implementation and violate the
minimum-diff authority boundary.

Consequently S3's consumer conclusion is finite: the host contract is proven;
full guest OLE client/server interoperability is an unselected immutable-guest
capability, not an unfinished host binding or a hidden S3 implementation item.
No source defect, new provider or guest change was found.

## S4 handoff

S4 reconciles the hash-identical package, the real common-dialog consumer
result and this original alias proof. It must ensure neither family is claimed
through the other and must close W18 without absorbing guest OLE server work.
