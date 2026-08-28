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
- x86 has 81 physical unresolved records / 79 unique symbol forms after the
  MSVC `/MT` CRT imports are included.  Its source-first ledger is
  [`m0-t310-s2-x86-forced-link-ledger.tsv`](../operations/m0-t310-s2-x86-forced-link-ledger.tsv).
- x64 has 83 physical unresolved records / 81 unique symbol forms.  Its
  source-first ledger is
  [`m0-t310-s2-x64-forced-link-ledger.tsv`](../operations/m0-t310-s2-x64-forced-link-ledger.tsv).
- Every physical form in both ledgers has one disposition: original SoftPC
  machine source, original SoftPC host-control source, original MVDM provider
  source, a directly selectable original MVDM definition, or a named modern
  Win32/OpenNT-host boundary.

## NTVDMx64 patch visibility

- On x86, the byte-identical `fmstubs.c` is included in
  `ntvdmx64-softpc-patch-evidence.lib`.  Its
  `host_q_write_timestamp` contributes a visible physical dependency on the
  original `softpc.new/host/src/nt_timer.c:GetPerfCounterUsecs`; it is not
  silently stubbed or omitted.
- On x64, the same source is retained but cannot compile due to its original
  x86 inline assembly.  The four patch-hook forms it supplies on x86
  (`EDL_fast_bop`, `c_VirtualiseInstruction`, `c_sas_touch`, and
  `VdmSetPhysRecStructs`) remain visible as four explicit
  `x86-patch-evidence-not-x64-composable` records.  They require a later
  source-shaped binding decision; none is treated as an implemented x64 path.

## Interpretation

This closes the candidate's compile-and-physical-link discovery step.  It is
not a runnable SoftPC backend: the remaining links select original device,
firmware, timer, console, provider and monitor packages in their original
owner order.  The only approved pointer/VDM-alias recovery path remains the
session mapping manager; no raw host address conversion was enabled here.
