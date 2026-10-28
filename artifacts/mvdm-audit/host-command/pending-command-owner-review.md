# COMMAND host pending C/H: current role decisions

At the review snapshot these three rows were pending using the actual shard decision fields. Later the coordinator marked cmdexit.c and cmdkeyb.h partial; cmdpif.h remains pending because other profiles and consumers were not established. The current source files match the corresponding OpenNT files by SHA-256. Recommendations below are bounded to identified roles and evidence; they do not edit shared ledgers or establish whole-feature completeness.

## `cmdexit.c`: partial candidate

The original permanent COMMAND source issues `SVC_CMDEXITVDM`; `nt_bop.c` routes the service to `CmdDispatch`, whose table maps it to `cmdExitVDM`; the latter calls `TerminateVDM`. Nonpermanent and single-command branches have separate paths. `shard-1/command-exit-owner-audit.md` documents the source chain and the historical link map; it notes that its inspected cached executable differs from a package snapshot, so that map cannot prove installed image identity.

Current and OpenNT `cmdexit.c` SHA-256: `3FB4616667B2B73F00ED56D5944A786BE741F5DB511E6F0D5F6A751593A418C9`. A current cached selected `cmdexit.obj` SHA-256 `92DD31EDE313ACB0D4312C4B72865C7CDD9DE7937F8AF047378E27BF1A2F76C1` was linked to `cmdexit-selected-object-fixture.c` SHA-256 `ED5F438CAC6AB840B295D54FD4BBE1E7DE496737BB569DD745999E80DE7F64FB`; its controlled `TerminateVDM` seam observed one call and the test returned 0. The i686 GCC link emitted `Warning: corrupt .drectve at end of def file` but succeeded. The seam intentionally returns instead of terminating; this tests only direct forwarding, not actual worker exit, broker coordination, console handoff, guest dispatch or product binary provenance. Recommend partial, retaining all these lifecycle obligations.

## `cmdkeyb.h`: partial candidate

Current and OpenNT header SHA-256 `B83738295ED02D762748EEF550058332078CE24B94ED76FD1A77C2EB8F77A361`. The current `cmdkeyb.c` includes this exact header; the recorded selected-object dependency and source call chain are in `shard-3/cmd-keyboard-layout-role-review.md`. Its constants supply host keyboard-layout registry paths and KB16/KEYBOARD.SYS names; the implementation adapts registry and product path providers. It is an active host declaration, not a guest-only file. Existing configured, malformed and absent registry fixtures have their stated limited scope; they do not prove all layout changes, buffer sizes, task lifetime or installed image identity. Recommend partial for this selected declaration role, not full keyboard coverage.

## `cmdpif.h`: excluded candidate for duplicate declaration only

Current and OpenNT header SHA-256 `1C629826EBA5A5437C481E8B4827298E5F178FA0EAA9CEACEB517CC80FE2E4C9`. The header says it duplicates `nt_pif.h`; it declares an additional `fgprio` member which is absent from the selected `softpc.new/host/inc/nt_pif.h`. A current source search found `cmdpif.c`, `cmdmisc.c` and host PIF implementation include `nt_pif.h`; no source include of `cmdpif.h` was found in inspected `src/mvdm` and `src/ntvdm-exe`. `shard-2/parallel-review-next.md` records the selected dependency census and previously inspected image bindings for parser and caller. Recommend exclusion of this historical duplicate declaration from the examined product build only, **not** of PIF behavior or the parser. Other build profiles, generated consumers and out-of-tree callers were not established; replacing the selected layout with the duplicate would require separate ABI proof.

No guest image or runtime product was modified. Current package and full end-to-end behavior still require separate evidence.
