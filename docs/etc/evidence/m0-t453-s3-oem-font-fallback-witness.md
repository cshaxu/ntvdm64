# M0 T453 S3 OEM consumers and font fallback witness

Status: S3 closure candidate. This record covers the finite non-ASCII and
font-removal paths selected by the T453 owner matrix. It makes no claim that
all historical font-installation environments can be reproduced on a modern
host.

## Ownership and path

The selected WOW delete path remains `WK32WowDelFile` in `wow32/wkman.c`.
Its original sequence is authoritative: direct OEM delete, visibility probe,
temporary-name/rename/delete/rollback when the name remains visible, then
font-resource removal, `WM_FONTCHANGE` broadcast and final OEM delete on
failure. The direct retained-file branch was exercised from the
production-built `wkman.obj` in S2; this S3 record closes its distinct font
fallback and non-ASCII provider inputs.

`oemuni/file.c::RemoveFontResourceOem` remains the sole OEM-to-Unicode
adapter for the fallback. It maps conversion overflow to
`ERROR_FILENAME_EXCED_RANGE`, other conversion failures through the original
NT-status mapper, and delegates the real resource state to the host
`RemoveFontResourceW`. Neither a filename-specific workaround nor a second
font-state owner was introduced. DEM's separate create retry retains its
original `.TTF`/`.FOT`, broadcast, retry and error branches.

## Current-source fixture closure

`tools/build/Generate-T420S17OemUniNinja.mjs` now supplies the current
firmware-root header and one test-only root provider when a fixture links the
original `process.c`. This reflects the already-existing production boundary:
`process.c` asks the session-aware firmware provider for Unicode system and
Windows roots before its original OEM narrowing. The DBCS fixture now injects
at that provider boundary rather than intercepting obsolete direct Win32 root
calls. The PIF caller fixture supplies only its unused console-title import.
These are fixture link closures, not production implementations or new
runtime state.

The declared S3 fixture group was generated under
`build/t453-s3/oemuni-fixtures` with the established x86 formal compile
flags and all seven images exited zero:

| Image | Witness |
| --- | --- |
| `oemuni-font-fixture.exe` | OEM Unicode conversion, provider result/failure, conversion overflow/no-memory mapping, TEB static-string reuse. |
| `oemuni-font-retry-fixture.exe` | Five original DEM create-failure cases: direct font removal, `.TTF` to `.FOT` fallback, broadcast/retry, and final error ownership. |
| `oemuni-failure-fixture.exe` | Non-ASCII full-path, environment expansion/delete-missing and short-buffer behavior. |
| `oemuni-family-fixture.exe` | OEM enumeration, search, rename/delete/directory cleanup and missing-font negative path. |
| `oemuni-dbcs-fixture.exe` | CP932 multibyte conversion, exact capacities, root-provider narrowing, growth/failure cleanup and environment capacity matrix. |
| `oemuni-pif-caller-fixture.exe` | Original `cmdpif.obj` PIF path/error/cleanup caller using the current OEM providers. |
| `oemuni-pif-environment-fixture.exe` | Guest PIF environment copy/release failure restoration. |

The durable run summary is `build/t453-s3/oemuni-fixtures/run-results.json`.
Its DBCS witness includes all thirty size-matrix cases and its font-retry
witness reports five passing original retry scenarios. Compiler warnings are
legacy warning-level diagnostics in the exercised original bodies; the group
has no compile or link failure.

## Boundary left to S4

S3 adds no product source or binary. S4 remains responsible for coherent W04
package acceptance: repeated real operations, cleanup, non-ASCII DOS/native
return and the ten-image release decision. The approved S2 product package
remains the current published baseline.
