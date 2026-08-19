# T229 S2 P7 — DEM demDelete Direct/Readonly conformance

OpenNT `demDelete` (`demfile.c`) takes an OEM pathname from `DS:DX`, calls
`DeleteFileOem`, and uses `demClientError` on failure. The CLI whole provider
preserves that path/CF ABI while placing mutation policy before the host call.

The formal fixture reuses the file created by the source-derived create path:
Direct deletes it and returns CF clear with no GPR write; Readonly returns
`AX=5, CF=1` before `DeleteFile` and leaves the file absent. The route includes
ingress, checked guest pathname copying, provider dispatch and result ABI.
Overlay remains a seam only; no new behavior is admitted.

All formal Direct/Readonly fixture modes exited zero on 2026-08-18.