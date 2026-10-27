# COMMAND and WRITE original build-input edges

The [67-path table](guest-source-active-edges.csv) records 21 COMMAND/common
DOS source or include paths and 46 WRITE source or include paths in shard 0.
Every proposed path is pending in the prior ledger and byte-equal to a pinned
original. The [input receipt](guest-source-active-inputs.json) identifies the
original source paths and hashes used while traversing the dependency chains.
Some traversed files have recorded newline-only differences; they are not
included in the proposed exclusions. In particular the historical COMMAND
`init.asm` format difference is not silently treated as an excluded file.

Procedure: start from object names actually present in original `command.lnk`
or `write.lnk`; find each explicit object/source dependency in its makefile;
then follow literal assembly includes and quoted C includes through the
source directory and original DOS/WOW include roots. Every retained edge
records its source path and line. A source listed as an assembly dependency
can be an included segment rather than a standalone translation unit; the
table does not equate those roles. Header traversal establishes a historical
input relation, not that every conditional branch was active.

COMMAND's makefile lines 514–535 links its named objects with `link16` into
`command.exc` and relocates that result to `command.com`. Its included
`dos/v86/make.inc:44,54,84–91` selects MASM and CL16 inference rules. WRITE's
makefile lines 89–113 names its object groups, 120–124 supplies MASM/CL16
inference, and 145–152 runs `link16 @write.lnk`, `rc16`, then copies the guest
image to `write16.exe`. Explicit rules may select conditional source variants;
this audit does not evaluate LANG/DBCS/OS2/debug combinations or execute a
guest compiler.

These are candidates for file-role exclusion as host build inputs only. No
guest source is modified/rebuilt, no current immutable image is claimed to
derive from these source bytes, and no DOS/WRITE behavior is declared passed.
The combined review additionally checks selected host graph inputs and
observed include dependencies before accepting a candidate.
