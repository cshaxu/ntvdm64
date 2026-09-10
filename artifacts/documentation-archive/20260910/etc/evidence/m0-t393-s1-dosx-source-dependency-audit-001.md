# M0 T393 S1 — DOSX/DPMI32 source and dependency audit

## Question

Can the approved DOSX/DPMI32 package restore original source and
reduce project-specific divergence? This audit identifies the smallest
source-owned S2 cohort without confusing guest media, a retained mirror, or a
historical disposition with an enabled runtime provider.

## Inputs

The audit used the selected OpenNT/OpenNT-4.5 MVDM union, current source and
formal-disposition ledgers, and the published package. It did not change
source, build graph, or executable. Guest media is fixed project input and is
intentionally not enumerated by this task record.

## Procedure

1. Read `nt_pif.c`, `cmdconf.c`, `dpmi/dxboot.asm`, `nt_bop.c`,
   `dpmi32/dpmi32.c`, and `wow32/wow32.c` with the prior reached trace.
2. Run the no-write DOSX/DPMI disposition verifiers; inspect default build
   generator references instead of treating source-tree presence as composition.
3. Compare reached owner and adapter roots with registered divergences.

## Observations

### Reached owner chain

The reached DOSX path enters `MS_bop_3 → DpmiDispatch`; the original table
selects `DpmiInitDosx` and owns `53:00`–`53:18`. The later `MS_bop_1`
transition is a separate Win16/WOW32 owner handoff.

The DOSX guest product is not host source to import. The first unresolved host
runtime cohort is complete DPMI32, not one `53:xx` BOP leaf. WOW32 begins only
at the separate `MS_bop_1` successor boundary.

### Current original source and formal disposition

| Owner slice | Original source form | Current disposition | S2 rule |
| --- | --- | --- | --- |
| DOSX `dpmi` | 59 recorded paths, 22 selected guest bodies plus `486/dosx.exe` | verifier: `guest-only`; never host link input | retain whole guest product; media only after separate approval |
| DPMI32 x86 | 25 paths, 16 selected original x86 provider bodies | verifier: `adapter-backed`; `New-T310OriginalSoftpcNinja.ps1` already compiles the complete original source list into `original-mvdm-dpmi32.lib`, but no DOSX-to-WOW runtime closure is proved | retain that complete host cohort and audit its actual CCPU40/session mapping bindings |
| WOW32 | 77 active original bodies in eight subdomains, 105 declaration carriers | separate Win16/WOW successor owner package | next Queue candidate admits the full source/build cohort and proves runtime boundaries by subdomain |
| WOW16 kernel | immutable `krnl386.exe` media | absent from stage; not host link input | stage identity under a media manifest change, never rewrite it |

The old one-file WOW callback generator is focused evidence, not a default
product graph. Historical ledgers naming `adapter-bochs` are retired binding
descriptions, not authority to restore Bochs. Before graph admission, T393
must reconcile them to the selected CCPU40/session mapping seam.

### Difference ledger

There is no uncommitted source change in the reached owner or adapter roots;
the mirror is not awaiting a local merge. It contains registered compatibility
divergences. This audit found governance drift: `Test-T289S7DpmiMirrorAudit`
allows only six modified DPMI32 files, but committed `MVDM-HOST-DIV-140` also
changes `buffer.c` and related host-flat-address files. The audit fails at
`buffer.c` even though the divergence is registered in
`src/mvdm-host/README.md`.

That is an audit-script/register mismatch, not evidence that `buffer.c` can be
deleted or replaced. S2 must derive its allowlist from the divergence register
or update it precisely before using the audit as a gate. It must distinguish
these required x64 safety adaptations from any later reduction in
project-authored boundary code.

## Recovery ladder

| Rung | Decision | Limit |
| --- | --- | --- |
| 1. Original composable source | selected for complete DPMI32 | mirror presence does not enable runtime proof |
| 2. Smallest same-shaped seam | required | bounded mapping lease and CCPU40 binding for DPMI32 |
| 3. Exceptional external intrusion | not admitted | no missing mechanism yet needs NTVDMx64; Bochs remains prohibited |
| 4. New behavior | prohibited | no synthetic DPMI, parser, replacement bootstrap, BOP success leaf, or CPU workaround |

## Interpretation and confidence

**Yes, conditionally.** T393 can recover substantial original-provider flow:
the original DPMI32 material is already retained. The likely benefit is
replacing or avoiding small project-specific boundary behavior; it is not a
promise that copying files alone reduces diff. DPMI32 enters as one complete
host owner cohort through bounded current seams.

Confidence is high for DPMI owner classification and counts: the DPMI
disposition verifiers passed. No runtime completion claim is made.

## S2 recommendation and stop condition

Admit S2 only after it freezes one default formal product graph. It must
reconcile the DPMI audit allowlist and verify the complete 16-body DPMI32 x86
cohort through the CCPU40/session mapping seam. Its exit boundary is
`MS_bop_1`; the next Queue candidate owns the complete WOW32 source/build
admission and all subsequent Win16 runtime work.

## Governance revision

The owner clarified after S1 that WOW32 is a Win16 runtime package. T393 is
therefore deliberately DPMI32-only. The earlier WOW32 classification remains
successor evidence, but does not admit a WOW32 source body, carrier, provider
image, or runtime path into this T.
