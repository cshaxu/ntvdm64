# T194 S12 source-built normal-return asset recovery 001

Date: 2026-08-12  
Packet: M0 T194 S12  
Disposition: exact asset already present; S11 absence conclusion superseded.

## Correction

S11's repository-wide filename search did not descend to the deeply nested
historical stage output. Direct verification finds two retained copies:

- `artifacts/toolchain-runs/share-tools16-opennt-v1/base/mvdm/dos/v86/cmd/share/SHARE.EXE`
- `artifacts/toolchain-runs/t177-share-normal-return-r1/base/mvdm/dos/v86/cmd/share/share.EXE`

Each is 882 bytes with SHA-256
`69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC`.
The latter's recovery manifest also records the unchanged OpenNT source path,
revision, MASM and LINK16 hashes, exact recipe, empty transform list and
internal-research disposition.

## Result

No recovery build, copy, rename, profile change or execution was needed. The
source-built normal-return target is available for an explicitly declared
future mapping to canonical `TARGET.EXE`. S11 remains historical evidence of
the inadequate shallow search; this successor is the current asset fact.

## Follow-up

T194 S13 may materialize one new v6 BYOB root from the verified components,
mapping this exact image only to `TARGET.EXE`, and validate it through the
existing profile parser. That copy/materialization remains distinct from a
guest observation.
