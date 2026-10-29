# T473 S11 P1 owner-supplied mirror restoration review

Owner request (2026-10-29): audit the newly restored original mirrors and include
accepted changes in a commit. This is within the active whole-recovery audit;
it does not admit any previously unselected runtime provider.

Four files match `O:/repos.external/OpenNT/base/mvdm` byte-for-byte, as bound by
manifest.json. Effective differences against HEAD, ignoring line endings:

- inc/vdmtib.h: remove SUPPORT-DIV-008's include rewrite, restoring the historical
  absolute NT include. This header is not in either current compiler dependency
  database. It is not portable if newly selected; any future admission must
  resolve the original include dependency explicitly.
- softpc.new/host/src/nt_aorc.c: restore original K&R declarations, including
  default-int segment setters and void setSS_BASE_LIMIT_AR. Current selected
  CPU40 uses its separate register providers, not this historical facade. This
  restoration does not establish that the facade has a compatible modern ABI.
- wow32/wkglobal.c and wow32/wklocal.c: only original text/line-ending restoration;
  git diff --ignore-space-at-eol reports no code changes for either file.

Build-selection review: neither worker nor WOW32 Ninja source graph selects the
four source paths, and neither Ninja dependency database references this vdmtib.h.
Cached Ninja dry runs for ntvdm.exe/nthook32.dll/VDMREDIR.dll and wow32.dll both
report no work. Therefore there is no affected executable to publish or a new
runtime behavior to claim. Existing S11 candidate runtime results are separate.

All four now have 100% original source retention / zero original-source diff.
This is NOT a claim of full functional recovery: retain the existing per-file
capability disposition and later-stage owner obligations. SUPPORT-DIV-008 is
retired for this header; DIV-102 remains for the still-adapted selected declaration
and storage files, with nt_aorc removed from its active path list.

Review verdict: accept these four mirror restorations and this bounded evidence.
Do not fold unrelated S11 runtime candidates or Win95 tool changes into this
source-only commit. No guest files, build selection, ABI export or runtime image
changed. No guest was rebuilt. This commit does not close S11 or T473.

Verification: actual cached Ninja builds of the same worker/provider targets
returned no work; documentation governance and staged diff whitespace checks
passed. This is a source-only P delivery, not stage or runtime acceptance.
