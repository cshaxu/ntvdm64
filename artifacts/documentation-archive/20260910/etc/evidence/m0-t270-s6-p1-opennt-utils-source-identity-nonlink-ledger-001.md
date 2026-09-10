# M0 T270 S6 P1 — Opennt-utils source-identity and non-link ledger

## Question

Does every retained utility input match its selected OpenNT original, and has
the retained package remained outside the current formal runtime link graph?

## Per-file identity

| Target | Selected OpenNT path | Disposition |
| --- | --- | --- |
| `src/opennt-utils/suballoc/debug.c` | `base/mvdm/suballoc/debug.c` | exact SHA-256 match |
| `src/opennt-utils/suballoc/foo.bar` | `base/mvdm/suballoc/foo.bar` | exact SHA-256 match |
| `src/opennt-utils/suballoc/makefile` | `base/mvdm/suballoc/makefile` | exact SHA-256 match |
| `src/opennt-utils/suballoc/sources` | `base/mvdm/suballoc/sources` | exact SHA-256 match |
| `src/opennt-utils/suballoc/suballcp.h` | `base/mvdm/suballoc/suballcp.h` | exact SHA-256 match |
| `src/opennt-utils/suballoc/suballoc.c` | `base/mvdm/suballoc/suballoc.c` | exact SHA-256 match |
| `src/opennt-utils/suballoc/suballoc.txt` | `base/mvdm/suballoc/suballoc.txt` | exact SHA-256 match |
| `src/opennt-utils/suballoc/tsa.c` | `base/mvdm/suballoc/tsa.c` | exact SHA-256 match |

## Current-link disposition

A whole-repository source/build-manifest scan finds no current compiled,
linked, included or runtime consumer of `src/opennt-utils/suballoc`. The only
current non-document reference is the production-tree governance allowlist.
The component intentionally defines no library target. This confirms the
admission remains an exact retained utility package, not an implicitly revived
utility runtime dependency.

There is no `opennt-utils-overlay`; no exception register entry is required.

