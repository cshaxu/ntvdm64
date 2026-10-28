# DOS kernel message inputs and code-page 863 data

This read-only shard-2 group covers four pending guest-source paths. The
[observer](dos-kernel-country-observer.py) and [receipt](dos-kernel-country-observations.json)
check both pinned original editions, both frozen audit catalogs, historical
recipe/source edges, and captured ZIP and installed COUNTRY.SYS identities.
All four source files retain 100% original bytes. That is not a whole guest
behavior restoration estimate.

| Path | Proposed role | Evidence and remaining boundary |
| --- | --- | --- |
| `dos/v86/doskrnl/bios/msbio.skl` | excluded candidate: historical guest build input only | Original NTIO recipe derives `msbio.cl1` and related message classes from this skeleton and locale message file. Guest initialization, diagnostics, error reporting and shipped-image provenance remain in scope. |
| `dos/v86/doskrnl/dos/msdos.skl` | excluded candidate: historical guest build input only | Original NTDOS recipe derives `msdos.cl1` and `.cl2` from the skeleton and locale messages. DOS error semantics and localization are not excluded. |
| `dos/v86/doskrnl/dos/dos.tag` | excluded candidate: historical change log only | Records original DOS kernel changes, not the implementation or a runtime authority. DOS kernel behavior remains independently required. |
| `dos/v86/dev/country/nls863.inc` | partial candidate: original guest data input with exact packaged data | Original `country.asm` includes it, and the original recipe builds COUNTRY.SYS from `country.asm`. Its 256-byte French-Canadian code-page-863 collating sequence occurs exactly once at recorded offset 23517 in the captured ZIP, installed, and source-tree prebuilt COUNTRY.SYS (all three image hashes match). Exact sequence matching does not prove the image was produced from this source or that runtime lookup selects that sequence. Other tables and case mappings, configured country/code page, guest APIs, encoding and errors remain unverified. |

The separate historical `bin86/country.sys` has a different recorded identity;
this does not establish a failure or justify replacing the user's installed
file. The earlier [country package review](../shard-0/bin86-country-config-review.md)
traces that distinction. No build recipe was run; some have deletion targets.
No guest, installation, product, process, shared ledger or Ninja tree changed.
The coordinator should replay and review all four proposed dispositions.
