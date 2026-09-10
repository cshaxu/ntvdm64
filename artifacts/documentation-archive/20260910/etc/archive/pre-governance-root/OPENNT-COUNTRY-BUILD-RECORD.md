# OpenNT COUNTRY.SYS Build Record

## Scope

This record covers the country/code-page database declared by
`base/mvdm/dos/v86/dev/country/makefile`. It is a compatibility-build result
only. It does not demonstrate that `NTIO.SYS`, `NLSFUNC`, or a runtime country
loader accepts the candidate.

## Corrected Source Closure

The original `country.asm` is a single assembly unit with local `nls*.inc`,
`dbcs93x.inc`, and `mkcntry.inc` inputs. It also includes `copyrigh.inc`.
That input exists in the checked OpenNT V86 include tree as Git blob
`d854d5da283a2891c403ef29866f2018ac25baa1`.

Earlier country stages copied only the `dev/country` subtree and consequently
omitted that one shared input. The blocker was therefore an incomplete stage,
not an absent OpenNT source file. The managed `src/opennt` island now includes
the text-only `dev/country` closure as hash-verified `upstream-copy` inputs.
Fresh stages verify that closure and the shared include against
`SOURCE-MANIFEST.json` before copying both unchanged into the disposable
country source directory. The OpenNT input checkout remains unmodified.

### Managed-Input Audit

`toolchain-probe/country-managed-input-audit-v1` completed on 2026-08-07
against the managed source island. It verified exactly 19 country-local
`upstream-copy` entries plus the shared `copyrigh.inc` entry before staging.
The resulting disposable directory contains only those source inputs and the
stage manifest. It contains no `COUNTRY.SYS`, map, object, executable, or
other generated input. This audit establishes current source provenance only;
it is intentionally distinct from the older `country-stage-v4` assembly/link
record below.

## Build

```text
OpenNT revision: 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c
stage:           toolchain-probe/country-stage-v4
assembly:         JWasm -omf -Zm
link:             Open Watcom DOS compatibility linker, nocaseexact
conversion:       source-built opennt-reloc, load segment 0000
```

`country.asm` assembled in two passes with zero warnings and zero errors. The
original makefile's single-object link shape was preserved.

The current managed-input reproduction used
`toolchain-probe/country-managed-input-audit-v1`. It assembled the verified
20-file stage with JWasm, linked its `obj/country.obj` using the existing
Open Watcom compatibility path, and converted the MZ image with the
source-built `opennt-reloc` helper at load segment `0000`. WLink emitted its
structural `W1014` (no stack segment) and `W1023` (default start address)
warnings; both are retained as evidence rather than suppressed. The resulting
object, linked image, and headerless image exactly match the earlier v4 hashes
below. `opennt-reloc-test` also passed its self-owned MZ relocation and header
removal fixture before this reproduction was accepted.

| Artifact | Result |
| --- | --- |
| OMF `country.obj` SHA-256 | `BF81CE220752650D3A42F2E660266A621285011EA97F08BE117EAF777C51AB74` |
| linked `country.exe` | 27,129 bytes |
| linked SHA-256 | `7FD488AB6346409BE7D9368E475CEFC358AF7FAA18DCFFDBF0E3FC0B5D87E416` |
| relocated `COUNTRY.SYS` | 27,097 bytes |
| `COUNTRY.SYS` SHA-256 | `9DDF2B738307B5950699513E6B8221E2044BB686487A8C216AF7C6FD3F853B54` |

## Qualification

The candidate is neither asserted bit-identical to a retail file nor added to
a runtime payload. Before runtime support is claimed, a test must load this
file through the selected NTDOS profile, request at least one country/code-page
record, and validate malformed-data and unsupported-code-page behavior.
