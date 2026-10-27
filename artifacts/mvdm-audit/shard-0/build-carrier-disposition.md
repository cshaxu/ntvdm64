# Build carrier disposition: configuration and compilation are distinct

The companion CSV records 16 shard-0 paths and their freshly checked frozen
SHA-256 identities. Four original `sources` files remain active configuration
inputs: DEM, CCPU386, communications and XMS. `Get-OriginalSources` in
`tools/build/New-T310OriginalSoftpcNinja.ps1` reads their `SOURCES` C-file lists;
the corresponding manifest assignments and calls are at lines 141--155 and
171--222 in the reviewed generator. CMake invokes this generator through
`ntvdm_generate`. A Ninja target-input listing alone cannot account for these
configuration-time reads.

These four files are **partial build-carrier integrations**, not fully restored
NT build environments and not proofs that each source member behaves correctly.
The parser does not execute their original target, include, architecture,
UMTEST or shared-NT-build directives. CCPU additionally removes `ntstubs.c`
and adds `localfm.c`; XMS adds `xmsmemr.c` for the selected CPU40 backing. The
source members, alternate profiles and their existing findings remain in the
separate source-level reviews. No numeric code-restoration percentage is assigned
to declarative build metadata; executable source-retention estimates remain
separate from this role classification.

The other twelve files are excluded **as historical recipe inputs only**:
four `makefile.def` trampolines, one `DIRS=src` recursion list and seven
generated architecture-specific `_objects.mac` lists. Their inspected contents
contain no runtime algorithm. Current CMake and the T310/T404 Ninja generators
own component traversal, object creation, archives and links. Exclusion does not
remove any debugger, DOS, disk, XMS, support or WOW capability from the T goal;
each listed source/provider remains independently classified. Files are retained
unchanged in the mirror.

This batch is source/build inspection, not a product test. It changes no product
or guest input. Review conclusions must not convert an untested source member
into a passing runtime capability or use missing Ninja dependencies as proof
that a configuration file is unused.
