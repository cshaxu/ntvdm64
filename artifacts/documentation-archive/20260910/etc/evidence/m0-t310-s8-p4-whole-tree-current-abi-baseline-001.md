# M0 T310 S8 P4 — Whole-tree current ABI baseline

## Scope

This is the required source-first baseline for the complete original-source
trees below `src/mvdm-host` and `src/mvdm-support`.  It does not include app,
session, adapters, guest media, firmware-only inputs, or any historical tree.
It neither enables a controller nor treats a compiler warning as a repair
instruction.

## Inputs and procedure

1. Regenerated the formal original-SoftPC MSVC `/MT` Ninja graph on x64 and
   x86 from the current selected source tree.
2. Rebuilt `original-softpc-candidate` on each architecture outside the
   sandbox, writing the paired compiler logs below the formal build roots.
3. Ran the repository audit generators over both complete source roots and the
   two current compiler logs:
   `Export-T310WholeTreeAbiInventory.mjs`,
   `Export-T310WholeTreeCompilerWarningLedger.mjs`, and
   `Export-T310WholeTreeContractReviewIndex.mjs`.

## Current inventory

- The static pass reads 1,412 original-source files and records 2,019
  candidates in 33 owner/package clusters:
  - 1,274 function-pointer indirect calls;
  - 520 function-pointer declarations or slots;
  - 176 architecture conditionals;
  - 40 pointer-to-fixed-width conversions; and
  - 9 fixed-width-to-pointer conversions.
- The formal x86/x64 cross-check contains 2,126 diagnostic occurrences and
  1,740 unique source diagnostics.  It remains intentionally unsuppressed.
- 93 x64 diagnostics in the current selected graph are the immediate
  source-reading frontier: `C4311`, `C4312`, `C4047`, `C4057`, `C4113`,
  `C4133`, `C4024`, `C4028`, `C4152`, or `C4211`.  They are grouped by the
  generated review index instead of becoming independent warning fixes.

## Disposition rule

The pending review state in the ledgers is deliberate.  Every candidate must
be reviewed as an original declaration + assignment/initializer + selected
caller + original owner contract.  A repair is admitted only when that reading
proves one of these results:

- a minimal same-shaped mirror declaration/expression correction;
- a private mirror overlay for material added native-width mechanics;
- a session mapping-manager boundary for an actual external host identity or
  guest-memory lease; or
- an unselected/original-unavailable product disposition.

Private native SoftPC addresses remain native-width internal values.  They are
not mapping tokens.  Non-width diagnostics such as unused parameters,
deprecated CRT calls, source-era formatting, and style warnings stay visible
as evidence but are explicitly outside this ABI repair pass unless source
reading proves they affect x86/x64 call or data representation.

## Next work

Review continues by complete original contract clusters.  The generated
CVIDC/base-video and selected CCPU clusters remain source-owned groups, while
the current first unreviewed machine groups are BIOS/reset, comms, host input,
host platform, and EMS/UMB memory.  No controller will be enabled merely
because its source archives.
