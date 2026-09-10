# M0 T348 S3 — Guest artifact matrix closure

Date: 2026-09-01

## Reproducibility

The Node 24 generator `tools/audit/Export-T348GuestArtifactMatrix.mjs` was
run twice against the fixed repository inputs.  Both generated
`m0-t348-s1-guest-artifact-matrix.tsv` with SHA-256:

`29f304ed164e89a6bd780f28009987c4ecc94504b4a9de7ab6f52ad72b7338f1`

## Matrix integrity

The matrix has 2,719 rows and 2,719 unique `(root, relative_path)` identities.
Every row has exactly one allowed final disposition:

- 9 `runtime-media` rows;
- 6 `runtime-firmware-adjacent` rows;
- 1,700 `runtime-deferred-wow16` rows;
- 889 `mirror-not-current-stage` rows;
- 113 `source-built-cross-check` rows;
- 2 `firmware-not-current-stage` rows.

The 9 media rows expand to 10 destinations because original `COMMAND.COM`
has two required destinations.  With the six firmware destinations, the S2
stage proof therefore contains exactly 16 media paths.  Its manifest,
physical files and SHA-256 values matched the matrix exactly.

## Boundary result

The matrix is a load-only media contract.  It neither rebuilds guest source
nor links guest source into the host executable.  Its next queue successor is
the bounded executable-relative MVDM package-path capacity disposition; guest
bootstrap continuity remains a later, separately admitted machine/guest
owner package.
