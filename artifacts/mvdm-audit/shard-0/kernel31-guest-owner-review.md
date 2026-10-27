# Kernel 3.1 historical input roles — shard 0

The [26-path table](kernel31-guest-owner-review.csv) covers the proven subset
of pending shard-0 files in `wow16/kernel31`. The
[input receipt](kernel31-guest-owner-inputs.json) records current/original
hashes for the source and literal include traversal. Every proposed exclusion
is byte-equal to its original; newline-only differences encountered during
traversal are not promoted as exclusions.

Procedure: read the actual bare object stems in original `krnl286.lnk` and
`krnl386.lnk`, stopping before the output image/options; resolve matching
explicit object/source rules in the original makefile, then follow literal
assembly and quoted C includes. Each proposed path retains its exact first
source/include edge and linker response. Included assembly segments are not
misrepresented as independent translation units. Candidate output is confined
to this shard's pending kernel31 paths, not all reachable shared headers.

Original makefile lines 179–185 select link16, MASM and CL16. Its 286P and
386P branches use separate link responses/OBJ2/OBJ3 at 306–360; they are not
interchangeable. `kernstub.asm` additionally has the explicit MASM/link16
stub target at 377–382. The kernel target depends on it and default
`kernel.def:6` names `STUB 'KERNSTUB.EXE'`. This is a historical guest stub,
not a modern host PE entry or a new guest replacement.

The `strings.asm` object relation does not establish which localization was
copied into it. LANG/COUNTRY/DBCS, DEBUG/DEST and original source conditional
branches remain unexecuted. Default NT builds select 386; recording a 286
historical source relation neither activates a new profile nor excludes its
capability. Unlinked or unproved source/tool/support files and existing
guest binaries remain pending, including the current kernel image's source
derivation. No guest is modified, built or executed by this review.
