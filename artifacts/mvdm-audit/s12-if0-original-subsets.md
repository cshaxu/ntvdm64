# S12 — original literal-zero conditional subsets

The owner requires retaining each original conditional structure and replacing
excluded bodies with a numbered reason. The 49 bodies in 37 selected host C
files are under original literal `#if 0`; a lexical comment-aware nesting pass
preserved the enclosing directives and any top-level `#else` / `#elif` and
confirmed each removed body matches the corresponding OpenNT source after
newline normalization. Input files were checked against the current Git
baseline before editing. The 39 MVDM branches use MVDM-HOST-DIV-496 and the
10 non-MVDM branches OPENNT-HOST-128. The
[per-branch record](s12-if0-original-subsets.json) gives paths, original line
ranges, body hashes, alternate preservation and original provenance.

Seven other literal-zero blocks in `nt_com.c` and `nt_fulsc.c` were held
because later `__LINE__` use makes physical-line changes potentially
observable. Five already marked multimedia blocks were left intact. This
batch neither establishes that other conditionals are inactive nor modifies
any guest source or guest binary. Build and image comparison are pending.

Verification: cached x86/x64 builds, including explicit VDMREDIR and both
hooks, passed. Self-review reconstructed the 49 replacements from the prior
Git blobs and checked all 37 files byte-for-byte; no unrelated source bytes
changed. The first comparison found WOW32's code differed by one byte: the
`clres.c` selected `#else` path uses a macro that embeds source line numbers.
The registered `#line 689` on that active path preserves the original line;
after rebuilding, all ten images matched the published images exactly or
only in the four COFF/debug timestamps. Thus no new runtime package or
repeat product regression was required for this batch. Documentation checks
are run separately. No guest was changed.
