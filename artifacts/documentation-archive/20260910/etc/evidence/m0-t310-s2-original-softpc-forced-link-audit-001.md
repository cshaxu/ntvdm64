# M0 T310 S2 original SoftPC forced-link audit — 001

## Question

After compiling the exact selected original `ccpu386` and host-control source
sets, what physical links still prevent an original SoftPC candidate from
becoming an executable on x86 and x64?  In particular, is the imported
NTVDMx64 `fmstubs.c` patch body visible in the audit rather than dropped?

## Method

The formal T310 generator creates an architecture-local, deliberately
non-runnable DLL target.  It uses `/WHOLEARCHIVE` for every candidate archive,
`/FORCE:UNRESOLVED`, and `/FORCE:MULTIPLE`; the generated DLL is audit output,
not a runtime image.

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86
ninja -C build/M0-T310/S2/softpc/x86 -j8 original-softpc-forced-closure.dll

powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x64
ninja -C build/M0-T310/S2/softpc/x64 -j8 original-softpc-forced-closure.dll
```

`tools/audit/Export-T310ForcedLinkLedger.mjs` then parses each physical linker
caller (`library(object) -> symbol -> caller`) and cross-checks the normalized
symbol against the existing selected-MVDM source ledger.  It does not merge
same-named objects or manufacture a provider.

## Result

- Both x86 and x64 compile all 130 selected original CCPU bodies and ten
  selected original SoftPC host roots, archive them, and emit a deliberate
  forced-link DLL.
- x86 has 84 physical unresolved records after the
  MSVC `/MT` CRT imports are included.  Its source-first ledger is
  [`m0-t310-s2-x86-forced-link-ledger.tsv`](../operations/m0-t310-s2-x86-forced-link-ledger.tsv).
- x64 has 83 physical unresolved records.  Its
  source-first ledger is
  [`m0-t310-s2-x64-forced-link-ledger.tsv`](../operations/m0-t310-s2-x64-forced-link-ledger.tsv).
- Every physical form in both ledgers has one disposition: original SoftPC
  machine source, original SoftPC host-control source, original MVDM provider
  source, a directly selectable original MVDM definition, or a named modern
  Win32/OpenNT-host boundary.

## NTVDMx64 patch visibility

- `fmstubs.c` is compiled and archived as evidence on **both** x86 and x64.
  Its active contract remains immediate debugging break through uniform MSVC
  `__debugbreak()` syntax. This is a syntax divergence, not a runtime
  substitute.
- The evidence archive is deliberately excluded from both forced-link
  candidates. Consequently the four forms (`EDL_fast_bop`,
  `c_VirtualiseInstruction`, `c_sas_touch`, and `VdmSetPhysRecStructs`) remain
  visible on both architectures with `debugbreak-patch-default` disposition.
  No mapping manager, cancellation result, zero result or implicit continuation
  is currently enabled for them.
- A discarded mapping/lease experiment is retained under `#if 0`, outside the
  formal build graph, as design material only. It is not evidence of runtime
  recovery.

## Interpretation

This closes the candidate's compile-and-physical-link discovery step.  It is
not a runnable SoftPC backend: the remaining links select original device,
firmware, timer, console, provider and monitor packages in their original
owner order.  The only approved pointer/VDM-alias recovery path remains the
session mapping manager; no raw host address conversion was enabled here.
