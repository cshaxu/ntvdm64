# T229 S2 P5 — DEM demCreate Direct/Readonly conformance

## Source contract

OpenNT `src/opennt/base/mvdm/dos/dem/demfile.c:demCreateCommon` implements
`50:03 demCreate`: it reads `DS:SI` and `CX` attributes, opens with
`GENERIC_READ|GENERIC_WRITE`, `FILE_SHARE_READ|WRITE|DELETE`, and
`CREATE_ALWAYS`, then returns its opaque handle in `AX:BP` and zero-length
file size in `BX:CX`.  There is no `DX` pipe result for create.

## CLI disposition

Direct is an admitted namespace-content mutation and creates only the
fixture-owned host file.  Readonly is rejected at the shared file-view policy
before `CreateFile`, preserving the source-shaped `AX=5, CF=1` outcome.
Overlay remains an existing ABI/provider seam; no Overlay behavior is added.

## Verification

The formal whole-provider fixture tests both modes through ingress, checked
260-byte guest pathname copy, checked current-PDB owner read, provider
selection, result ABI, and paired `50:02` close.

| Mode | Result |
| --- | --- |
| Direct | file exists; `AX:BP` nonzero token; `BX=0`, `CX=0`, CF clear; paired close succeeds |
| Readonly | file remains absent; `AX=5`, CF set before host creation |

The formal Ninja target and all Direct/Readonly fixture modes exited zero on
2026-08-18.  This closes only the stated `50:03` success/refusal contract;
`50:22 demCreateNew` remains separately pending.