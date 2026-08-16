# T221 S3 P35: Virtual directory-rename transaction

## Question

Can the explicitly admitted, default-off Virtual namespace provide the full
private file-or-directory rename primitive required by the original DEM owner,
without falling through to Overlay or touching a host namespace?

## Inputs

- `src/opennt/overlay/base/mvdm/dos/dem/demfile.c`, `demRename`: same-drive
  requirement, same-name refusal, then one `MoveFileOem` operation.
- The existing Overlay mutation contract, which establishes that a directory
  move must include its tree rather than pretending it is a file operation.
- The bounded Virtual store, node view and create/delete/rmdir foundations.

## Procedure

Added a selector-blind store primitive that relocates every private entry below
the selected source prefix, including file and directory tombstones.  It
preflights path capacity and rejects self/descendant targets.  The Virtual
mutation backend applies the original same-drive and same-name checks, checks
the source, target and target parent through the private view, then calls that
one primitive.  The operation accepts no host-root, host path or host handle.

## Observations

The focused MSVC x64 `/MT` probe at `build/M0-T221-S3/109` passed with
`hostIo=false`.  It covers a directory tree move, moved file visibility,
moved tombstone invisibility, cross-drive refusal and a destination-inside-
source refusal.

## Interpretation

Virtual now has the requisite complete private directory-move shape; it does
not borrow the Overlay lazy host-tree relocation or materialize an admitted
host file.  This remains a selector-blind provider primitive.  No DEM BOP,
CLI option, engine admission or native trace is bound by this record.

## Follow-up

Integrate the completed Virtual namespace/mutation view through the one DEM
provider partition, then test the selected namespace, FCB wildcard and search
families together under all four profiles.
