# T109 S1 selector-family conformance 001

## Correction applied

The adapter no longer treats `C4 C4 02` as an explicit VdmRedir-unavailable
operation. It now classifies selector `02` as unknown, so the adapter cannot
manufacture a result for it. The dormant source-derived redirector terminal
provider has no runtime caller.

The focused C11 target
`bx-ntvdm-redir-unavailable-provider-v1-test` passes after the correction. It
proves all of the following without an engine build or guest trace:

| Encoded selector | Original identity | Current adapter disposition |
| --- | --- | --- |
| `02h` | Not `BOP_XMS` or `BOP_REDIR`; owner not yet established | Unknown selector, no provider result |
| `52h` plus service byte | `MS_bop_2 -> XMSDispatch` | Mapped/deferred XMS plane |
| `57h` plus service byte | `MS_bop_7` VdmRedir ingress | Mapped/deferred redirector plane |

The source evidence is `base/mvdm/inc/bop.h` (`BOP_XMS=52h`,
`BOP_REDIR=57h`) and `base/mvdm/softpc.new/base/bios/bios.c` (the matching
BIOS dispatch table entries). `host/src/nt_bop.c` supplies `MS_bop_2` and
`MS_bop_7` semantics. No test or document claims XMS or redirector support.

## Static debt and priority decision

The same table assigns `55h` to `MS_bop_5`; the present common ingress does
not yet expose that compatibility selector. This is recorded static debt, not
an admitted provider task: it has no evidence of runtime reachability.

By contrast, selector `02` was reached repeatedly in T108 at `073B:03FF`.
T110 therefore audits that actual source/table/caller provenance first. This
prevents both another numbering assumption and an add-on service strategy.
