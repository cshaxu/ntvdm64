# M0 T262 S4 — OpenNT host subset marker closure

## Result

The OpenNT-host mirror meets the file and exception-index rule:

- Seven exact files hash-match their named OpenNT source paths.
- Four retained reachable subsets/minimal edits are `nt_error.c`, `vrinit.c`,
  `vrnmpipe.c` and `vrputil.c`.
- The code marker set and README index set are exactly identical:
  `HOST-DIV-012` through `HOST-DIV-024`.
- A production-tree sweep finds no other host source/header and no remaining
  project-authored compatibility header.

The audit changes no host code: all four subset bodies already carry their
statement-local markers and the README records their purpose, reason and
replacement seam.
