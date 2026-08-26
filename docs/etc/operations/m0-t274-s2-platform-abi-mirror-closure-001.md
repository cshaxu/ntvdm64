# M0 T274 S2 platform ABI mirror closure

## Inputs

- [S1 non-guest union ledger](m0-t274-s1-non-guest-mvdm-union-ledger.tsv),
  generated from `O:\repos.external\OpenNT\base\mvdm` and
  `O:\repos.external\OpenNT-4.5\nt\private\mvdm`;
- the same two pinned source trees, searched only for declarations outside the
  selected MVDM union;
- [source policy](policy/source-policy.md) and the admitted T274/S2 Status
  brief.

## Procedure

1. `Export-T274S2PlatformAbiLedger.ps1` scanned the 1,467 selected MVDM
   code/header/assembly inputs for direct `#include` forms.
2. It classified each unique include as a selected MVDM-local include,
   CRT/public-SDK input, uniquely resolved external OpenNT declaration,
   ambiguous external candidate, or unresolved historical/generated input.
3. `Import-T274S2PlatformAbiHeaders.ps1` imported only the uniquely resolved
   original declaration headers, preserving their source-relative paths and
   refusing a non-identical overwrite.
4. The second importer run required all 22 destination hashes to match their
   recorded source hashes, proving the import is idempotent.

## Observations

- 1,467 selected host code inputs contained 5,537 direct include occurrences
  and 637 distinct include forms.
- 292 forms resolve directly beside selected MVDM sources; 183 map to another
  selected MVDM union path.
- 27 are CRT/public-SDK inputs and therefore do not belong in this mirror.
- 22 have one exact original OpenNT declaration source and are now imported;
  their combined size is 653,680 bytes and the import manifest reports zero
  SHA-256 mismatches.
- 70 same-name external candidates are deliberately unresolved pending
  complete owning-package include/build context. 43 forms are historical
  generated/segment or otherwise unresolved inputs. Neither category was
  guessed, copied or given replacement behavior.

## Interpretation

`opennt-platform-abi` now has the smallest source-proven direct declaration
mirror permitted by the current selected host universe. This is declaration
supply only: it does not claim that an original MVDM package compiles, links or
runs. The ledger makes every remaining direct external include explicit, so
the future host-topology/package task can resolve it at the owner package
boundary rather than growing an unclassified compatibility layer.

## Confidence and follow-up

Confidence is high for source identity and direct-include coverage because the
scan covers every selected host code input and every imported file is
hash-verified. Transitive declaration closure, historical generated segment
inputs, and same-basename candidates require the S5/S6 package build topology;
they are not implied by this S2 result.
